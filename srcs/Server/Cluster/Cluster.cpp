/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 13:31:43 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 12:49:34 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Cluster.hpp"

bool b_clusterRun = true;

//**************************//
//			DEFAULT			//
//**************************//

Cluster::Cluster(std::vector<Server *> servers) {
	if (servers.empty())
		throw Cluster::NoServerCreated();
	_servers.assign(servers.begin(), servers.end());
	_fdSize = _servers.size();
	_selectRet = 0;
	_maxFD = 0;
}
Cluster::~Cluster(void) {
}
Cluster::Cluster(const Cluster &cpy) {
	*this = cpy;
}
Cluster &Cluster::operator=(const Cluster &cpy) {
	if (this == &cpy)
		return (*this);
	_servers = cpy._servers;
	_sockets = cpy._sockets;
	_sendInfoSockets = cpy._sendInfoSockets;
	_readFD = cpy._readFD;
	_writeFD = cpy._writeFD;
	_errorFD = cpy._errorFD;
	_maxFD = cpy._maxFD;
	_fdSize = cpy._fdSize;
	_selectRet = cpy._selectRet;
	_clusterFD = cpy._clusterFD;
	return (*this);
}

//**************************//
//		CLUSTER UTILS  		//
//**************************//

int Cluster::ClusterSelect(int maxFD, fd_set &readFD, fd_set &writeFD, fd_set &errorFD, struct timeval &timeout) {
	return (select(maxFD + 1, &readFD, &writeFD, &errorFD, &timeout));
}
std::string Cluster::getDot() {
	static int i = -1;
	std::string _dots[3] = {".", "..", "..."};
	++i;
	if (i > 2)
		i = 0;
	return (_dots[i]);
}
int Cluster::clear() {
	for (std::map<int, Server *>::iterator it = _sockets.begin(); it != _sockets.end(); ++it) {
		it->second->cleanServer();
	}
	for (std::vector<Server *>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
		delete *it;
	}
	return (0);
}
Server *Cluster::getServerBySocket(int socket) {
	for (std::vector<Server *>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
		std::cerr << "Compare [" << (*it)->getFD() << "] && [" << socket << "]" << '\n';
		if ((*it)->getFD() == socket)
			return ((*it));
	}
	return (NULL);
}

//**************************//
//			SIGNALS	 		//
//**************************//

void Cluster::signalHandler(int signal) {
	if (signal == SIGINT) {
		std::cout << DIM << "\n\n\rExit WebServ" << RESET << '\n';
		b_clusterRun = false;
	}
}

//**************************//
//		CLUSTER STATES 		//
//**************************//

int Cluster::Accept() {
	// Accept
	// Make pair Socket & Server (Socket = Accepted Socket)
	// Un serveur peut accepter plusieurs socket donc pas de 'SetAcceptedSocket() dans le serveur.cpp'
	for (std::vector<Server *>::iterator it = _servers.begin(); _selectRet && it != _servers.end(); ++it) {
		if (FD_ISSET((*it)->getFD(), &_readFD)) {
		 	int	socket = (*it)->acceptSocket();
			//std::cerr << YELLOW << "[CLUSTER]" << GREEN << "[ACCEPT SOCKET : " << socket << YELLOW << "]" << RESET << '\n';
			formatInfo("Cluster", FT::to_string((*it)->getPort()), " Accepted socket: ", FT::to_string(socket), 0);
			if (socket < 0) {
				std::cerr << "Error Socket Accept" << '\n';
			}
			else if (socket >= 0) {
				FD_SET(socket, &_clusterFD);
				FT::emplace(_sockets, std::pair<int, Server *>(socket, *it));
				if (socket > _maxFD)
					_maxFD = socket;
			}
			_selectRet = 0;
			break;
		}
	}
	return (0);
}
int Cluster::Send() {
	// Send
	for (std::vector<int>::iterator it = _sendInfoSockets.begin(); _selectRet && it != _sendInfoSockets.end(); ++it) {
		if (FD_ISSET(*it, &_writeFD)) {
			int ret = _sockets[*it]->sendInfo(*it);
			if (ret < 0) {
				FD_CLR(*it, &_clusterFD);
				FD_CLR(*it, &_readFD);
				_sockets.erase(*it);
				_sendInfoSockets.erase(it);
			}
			else if (!ret)
				_sendInfoSockets.erase(it);
			_selectRet = 0;
			break;
		}
	}
	return (0);
}
int Cluster::Receive() {
	// Receive
	for (std::map<int, Server *>::iterator it = _sockets.begin(); _selectRet && it != _sockets.end(); ++it) {
		if (FD_ISSET(it->first, &_readFD)) {
			int ret = it->second->receiveInfo(it->first);
				if (!ret) {
				// process headers & methods
				it->second->Connection(it->first);
				_sendInfoSockets.push_back(it->first);
			}
			else if (ret < 0) {
				FD_CLR(it->first, &_clusterFD);
				FD_CLR(it->first, &_readFD);
				_sockets.erase(it->first);
				it = _sockets.begin();
			}
			_selectRet = 0;
			break;
		}
	}
	return (0);
}

//**************************//
//			CLUSTER  		//
//**************************//

int Cluster::init() {
	int		currentServerFD = 0;
	size_t	i = 0;

	FD_ZERO(&_clusterFD);
	_fdSize = _servers.size();
	_maxFD = 0;
	if (signal(SIGINT, signalHandler) == SIG_ERR) {
		throw ("Error : Cant't catch signal");
	}
	displayDimBar(DIM_BAR_SIZE);
	std::cout << DIM << "Server name: " << RESET << UNDERLINE << BOLD << _servers[0]->getName() << RESET << "\n\n";
	for (std::vector<Server *>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
		if (((*it)->initServer()))
		{
			std::cerr << RED << "Error : Port " << (*it)->getPort() << " already used !" << std::endl << RESET;
			continue;
		}
		else {
			currentServerFD = (*it)->getFD();
			FD_SET(currentServerFD, &_clusterFD);
			if (currentServerFD > _maxFD)
				_maxFD = currentServerFD;
			std::cout << GREEN << "Server : " << CYAN << i << GREEN \
			<< " Initialized | " << "IP : " << CYAN << ((strToIP((*it)->getIP()) == 0) ? "0.0.0.0" : (*it)->getIP()) \
			<< GREEN << " | PORT : " << CYAN << (*it)->getPort() << RESET << '\n';
		}
		++i;
	}
	if (!i || !currentServerFD || !_maxFD) {
		std::cerr << RED << "All ports are used, close them and restart webserv ! " << RESET << '\n';
		clear();
		return (1);
	}
	i = 0;
	displayDimBar(DIM_BAR_SIZE);
	for (std::vector<Server *>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
		std::cout << BOLD << "Locations: " << RESET << CYAN << "SERVER " << i << " [" << (*it)->getPort() << "]" << RESET << "\n\n";
		NEW_LOCATIONS(*it).printLocations(NULL);
		NEW_LOCATIONS(*it).createLocations();
		++i;
	}
	displayDimBar(DIM_BAR_SIZE);
	return (0);
}
int Cluster::run() {

	// Cluster Run all Servers
	while (b_clusterRun) {
		_selectRet = 0;
		struct timeval timeout;

		// Waiting a connection
		while (_selectRet == 0) {
			timeout.tv_sec = 1;
			timeout.tv_usec = 0;
			memcpy(&_readFD, &_clusterFD, sizeof(_clusterFD));
			FD_ZERO(&_writeFD);
			for (std::vector<int>::iterator it = _sendInfoSockets.begin(); it != _sendInfoSockets.end(); ++it) {
				FD_SET(*it, &_writeFD);
			}
			std::cerr << "\r                            " << std::flush;
			std::cout << "\rWaiting for a connection" << getDot() << std::flush;
			_selectRet = ClusterSelect(_maxFD, _readFD, _writeFD, _errorFD, timeout);

			if (!b_clusterRun)
				return (0);
		}
		// Found connection
		if (_selectRet > 0) {
			std::cout << "\n\n";
			Accept();
			Receive();
			Send();
		}
		else {
			std::cout << RED << "\nSelect: " << "error" << RESET << '\n';
			for (std::map<int, Server *>::iterator it = _sockets.begin(); it != _sockets.end(); ++it) {
				it->second->closeAcceptedClient(it->first);
			}
			_sockets.clear();
			_sendInfoSockets.clear();
			FD_ZERO(&_clusterFD);
			for (std::vector<Server *>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
				FD_SET((*it)->getFD(), &_clusterFD);
			}
		}
	}
	return (0);
}
