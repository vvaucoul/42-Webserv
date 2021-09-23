/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/16 17:42:46 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 19:12:34 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Server.hpp"

//**************************//
//			DEFAULT 		//
//**************************//

Server::Server() {
	_srvFD = 0;
	_ip = "";
	_port = 0;
	_addrlen = sizeof(_address);
	_clRequest = NULL;
}
Server::Server(std::string const &ip, int const &port){
	_ip = ip;
	_port = port;
	::Server();
}
Server::Server(Server *parent_location) : _parent_location(parent_location) {
	_is_extension = false;
	_is_location = false;
	_is_location_dir = false;
	_port = 0;
	_auto_index = false;
	_is_redirect = false;
	_isWaitingForContent = false;
	_client_max_body_size = 0;
	_clRequest = NULL;
}
Server::~Server() {
	for (std::vector<Server *>::iterator it = _locations.begin(); it != _locations.end(); ++it) {
		delete *it;
	}
	for (std::vector<Server *>::iterator it = _extensions.begin(); it != _extensions.end(); ++it) {
		delete *it;
	}
	if (_clRequest)
		delete _clRequest;
}
Server::Server(const Server &srv) {
	*this = srv;
}
Server &Server::operator=(const Server &srv) {
	if (this == &srv)
		return (*this);
	_locations = srv._locations;
	_is_location = srv._is_location;
	_location_name = srv._location_name;
	_is_location_dir = srv._is_location_dir;
	_port = srv._port;
	_ip = srv._ip;
	_name = srv._name;
	_html_path = srv._html_path;
	_error_pages = srv._error_pages;
	_html_error_path = srv._html_error_path;
	_method = srv._method;
	_auto_index = srv._auto_index;
	_redirect_link = srv._redirect_link;
	_root = srv._root;
	_cgi_path = srv._cgi_path;
	_client_max_body_size = srv._client_max_body_size;
	_cgi_extension = srv._cgi_extension;
	_address = srv._address;
	_fdSet = srv._fdSet;
	_fdSize = srv._fdSize;
	_maxFD = srv._maxFD;
	_addrlen = srv._addrlen;
	_fdList = srv._fdList;
	_srvFD = srv._srvFD;
	_is_redirect = srv._is_redirect;
	_clRequest = srv._clRequest;
	_requests = srv._requests;
	_accept_files_path = srv._accept_files_path;
	_extensions = srv._extensions;
	_extension_name = srv._extension_name;
	_is_extension = srv._is_extension;
	_parent_location = srv._parent_location;
	_set_vars = srv._set_vars;
	_rateLimits = srv._rateLimits;
	_isWaitingForContent = srv._isWaitingForContent;
	return (*this);
}

//**************************//
//			SERVER	 		//
//**************************//

//	Process :
//	[1] - AcceptSocket
//	[2] - ReceiveInfo
//	[3] - Connection
//	[4] - SendInfo

int Server::acceptSocket() {

	int clientSocket;

	if ((clientSocket = accept(_srvFD, NULL, NULL)) < 0)
		std::cerr << RED << "Client Socket Error : " << strerror(errno) << RESET << '\n';
	else {
		if (fcntl(clientSocket, F_SETFL, O_NONBLOCK))
			std::cerr << RED << "FCNTL Error : " << strerror(errno) << RESET << '\n';
		else
			_requests.insert(std::make_pair(clientSocket, ""));
	}
	return (clientSocket);
}
int Server::receiveInfo(int acceptedSocket) {
	char buffer[RCV_SIZE] = {0};
	int rcvRet = recv(acceptedSocket, buffer, RCV_SIZE - 1, 0);

	if (rcvRet == -1 || rcvRet == 0) {
		close(acceptedSocket);
		cleanSocketRequest(acceptedSocket);
		if (!rcvRet)
			std::cout << "Connection closed by client" << '\n';
		else
			std::cout << "Error read, connection closed" << '\n';
		return (-1);
	}

	formatInfo("Server", FT::to_string(_port), " Received: \n",
	(std::string(GREEN) + (std::string(buffer).empty() ? "" : std::string(buffer).substr(0, 400) + (std::string(buffer).size() > 400 ? "..." : ""))), 0);

	std::cout << RESET "\n\n";

	AddSocketRequest(acceptedSocket, std::string(buffer, rcvRet));
	int rcv = receiveCorrectHeader(getSocketRequest(acceptedSocket));
	if (!rcv) {
		if (!_isWaitingForContent)
			_rateLimits[acceptedSocket].addRateLimit(acceptedSocket);
	}
	return (rcv);
}
int Server::receiveCorrectHeader(std::string const &str) {

	if (str.find("GET") != NPOS
	|| (str.find("POST") != NPOS)
	|| (str.find("HEAD") != NPOS)
	|| (str.find("OPTIONS") != NPOS)
	|| (str.find("PUT") != NPOS)
	|| (str.find("TRACE") != NPOS)
	|| (str.find("DELETE") != NPOS)) {
	size_t start = str.find("\r\n\r\n");

	if (start != NPOS) {
		if (str.find("Content-Length: ") == NPOS) {
			if (str.find("Transfer-Encoding: chunked") != NPOS) {
				std::string sub = str;

				sub.substr(sub.size() - 5, sub.size());
				if (!sub.compare("0\r\n\r\n")) {
					return (0);
				}
				else
					return (1);
			}
			else
				return (0);
		}

		size_t len = std::atoi(str.substr(str.find("Content-Length: ") + 16, 10).c_str());
		if (str.size() >= len + start + 4)
			return (0);
		else
			return (1);
		}
	}
	else {
		if (str.find("Content-Disposition") != NPOS) {
			return (1);
		}
		else
			return (0);
	}

	return (1);
}
int Server::Connection(int acceptedSocket) {

	RequestConfig	config = RequestConfig(getSocketRequest(acceptedSocket));
	Request			request(config, NEW_LOCATIONS(this).searchLocation(config["Page"]),
							EXTENSIONS(this, _root).FileIsInExtension(config["Page"]).first,
							_root);
	if (_clRequest)
		delete _clRequest;
	_clRequest = new Request(request);
	config.printRequestConfig();
	return (0);
}
int Server::sendInfo(int acceptedSocket) {

	t_locationNode node = NEW_LOCATIONS(this).searchLocation(_clRequest->getConfig()["Page"]);
	std::string location, request;

	if (IS_VALID_LOCATION(node)) {
		location = node.path;
		if (_clRequest->isSearchingFile()) {
			request = NEW_LOCATIONS(this).getRootLocation().data->getRoot() + "/" + _clRequest->getConfig()["Page"];
		} else {
			request = NEW_LOCATIONS(this).getRootLocation().data->getRoot() + "/" + _clRequest->getConfig()["Page"] + "/" + node.data->getHTMLPath();
		}
	} else {
		node = NEW_LOCATIONS(this).getLastValidLocation(_clRequest->getConfig()["Page"]);
		request = NEW_LOCATIONS(this).getRootLocation().data->getRoot() + "/" + _clRequest->getConfig()["Page"];
	}
	request = fixSlash(request);

	Response	response(_clRequest, request,
		std::pair<bool, t_rate_limit>(_rateLimits[acceptedSocket].rateLimitAsBeenExceded(acceptedSocket),
		_rateLimits[acceptedSocket].getLastRateLimit(acceptedSocket)),
		node.data, this);

	std::string strResponse = "";

	try {
		strResponse = response.ProcessResponse();
	}
	catch (std::exception &e) {
		std::cerr << RED << "Error SendInfo : " <<  e.what() << RESET << '\n';
	}

	displayDimBar(DIM_BAR_SIZE);

	std::cout << GREEN << "[RESPONSE] : \n" << (strResponse.size() > 400 ? strResponse.substr(0, 400) + "\n\n......": strResponse) << RESET;

	displayDimBar(DIM_BAR_SIZE);

	int sendRet = send(acceptedSocket, strResponse.c_str(), strResponse.length(), 0);
	cleanSocketRequest(acceptedSocket);

	if (sendRet == -1) {
		close(_srvFD);
		cleanSocketRequest(acceptedSocket);
		_srvFD = 0;
		return (1);
	}
	else if (!sendRet) {
		cleanSocketRequest(acceptedSocket);
		return (0);
	}
	return (0);
}

int	Server::initServer() {
	if ((_srvFD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		displayError("Socket", strerror(errno));
		return (1);
	}
	memset(&_address, 0, sizeof(SOCKET_IN));
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = ft_stohl(_ip);
	_address.sin_port = htons(_port);
	if ((bind(_srvFD, (SOCKET *)&_address, sizeof(_address))) < 0) {
		displayError("Bind",strerror(errno));
		return (1);
	}
	if ((listen(_srvFD, MAX_PENDING_CONNECTION)) < 0) {
		displayError("Listen", strerror(errno));
		return (1);
	}
	return (0);
}

//**************************//
//		SERVER CLEAN  		//
//**************************//

int Server::cleanServer() {
	if (_srvFD > 0) {
		close(_srvFD);
		_srvFD = -1;
	}
	return (0);
}
int Server::closeServer() {
	close (_srvFD);
	_srvFD = -1;
	return (0);
}
int Server::closeAcceptedClient(int fd) {
	if (fd > 0)
		close (fd);
	return (0);
}

//**************************//
//		SERVER UTILS  		//
//**************************//

void Server::displayError(std::string const &prefix, std::string const &msg) {
	std::cerr << "Error : " << prefix << " : " << msg << '\n';
}
void Server::printServerConfig(int tabs) {
	for (int i = 0; i < tabs; ++i)
		std::cout << "\t";
	std::cout << "_is_extension : [" << _is_extension << "]" << std::endl;
	for (int i = 0; i < tabs; ++i)
		std::cout << "\t";
	std::cout << "_extension_name : [" << _extension_name << "]" << std::endl;
	for (int i = 0; i < tabs; ++i)
		std::cout << "\t";
	std::cout << "_is_location : [" << _is_location << "]" << std::endl;
	for (int i = 0; i < tabs; ++i)
		std::cout << "\t";
	std::cout << "_location_name : [" << _location_name << "]" << std::endl;
	for (int i = 0; i < tabs; ++i)
		std::cout << "\t";
	std::cout << "_is_location_dir : [" << _is_location_dir << "]" << std::endl;
	for (int i = 0; i < tabs; ++i)
		std::cout << "\t";
	std::cout << "_port : [" << _port << "]" << std::endl;
	for (int i = 0; i < tabs; ++i)
		std::cout << "\t";
	std::cout << "_ip : [" << _ip << "]" << std::endl;
	for (int i = 0; i < tabs; ++i)
		std::cout << "\t";
	std::cout << "_name : [" << _name << "]" << std::endl;
	for (int i = 0; i < tabs; ++i)
		std::cout << "\t";
	std::cout << "_html_path : [" << _html_path << "]" << std::endl;
	for (int i = 0; i < tabs; ++i)
		std::cout << "\t";
	//std::cout << "_html_error_path : [" << _html_error_path << "]" << std::endl;
	for (int i = 0; i < tabs; ++i)
		std::cout << "\t";
	std::cout << "_method : [";
	for (int i = 0; i < (int)_method.size(); ++i)
		std::cout << "'" << _method[i] << "', ";
	std::cout << "]" << std::endl;
	for (int i = 0; i < tabs; ++i)
		std::cout << "\t";
	std::cout << "_auto_index : [" << _auto_index << "]" << std::endl;
	for (int i = 0; i < tabs; ++i)
		std::cout << "\t";
	std::cout << "_root : [" << _root << "]" << std::endl;
	for (int i = 0; i < tabs; ++i)
		std::cout << "\t";
	std::cout << "_cgi_path : [" << _cgi_path << "]" << std::endl;
	for (int i = 0; i < tabs; ++i)
		std::cout << "\t";
	std::cout << "_client_max_body_size : [" << _client_max_body_size << "]" << std::endl;
	for (int i = 0; i < tabs; ++i)
		std::cout << "\t";
	std::cout << "\n";
	for (int i = 0; i < tabs; ++i)
		std::cout << "\t";
	std::cout << "- hasExtensions() : [" << hasExtensions() << "]" << std::endl;
	for (int i = 0; i < (int)_extensions.size(); ++i)
	{
		for (int j = 0; j < tabs; ++j)
			std::cout << "\t";
		std::cout << "Extension[" << i << "] : " << std::endl;
		_extensions[i]->printServerConfig(tabs + 1);

	}
	std::cout << "\n";
	for (int i = 0; i < tabs; ++i)
		std::cout << "\t";
	std::cout << "- hasLocations() : [" << hasLocations() << "]" << std::endl;
	for (int i = 0; i < (int)_locations.size(); ++i)
	{
		for (int j = 0; j < tabs; ++j)
			std::cout << "\t";
		std::cout << "Location[" << i << "] :" << std::endl;
		_locations[i]->printServerConfig(tabs + 1);
	}
}

//**************************//
//		SERVER REQUEST 		//
//**************************//

void		Server::cleanSocketRequest(int acceptedSocket) {
	_requests[acceptedSocket].clear();
}
void		Server::AddSocketRequest(int acceptSocket, std::string buffer) {
	_requests[acceptSocket] += buffer;
}
std::string	Server::getSocketRequest(int acceptSocket) {
	return (_requests[acceptSocket]);
}

//**************************//
//		  LOCATION			//
//**************************//

bool Server::hasLocationNamed(const std::string location_name)
{
	for (int i = 0; i < getLocationsSize(); ++i)
	{
		if (location_name == getLocationN(i)->getLocationName())
			return (true);
	}
	return (false);
}

//**************************//
//		  EXTENSION			//
//**************************//

bool Server::hasExtensionNamed(const std::string extension_name) {
	for (int i = 0; i < getExtensionsSize(); ++i)
	{
		if (extension_name == getExtensionN(i)->getExtensionName())
			return (true);
	}
	return (false);
}
Server* Server::getFileExtensionData(std::string fileName) {
	size_t found;

	//no "." in fileName
	if ((found = fileName.find(".")) == std::string::npos)
		return (NULL);
	std::string fileExtension(fileName, found + 1);
	for (int i = 0; i < getExtensionsSize(); ++i)
	{
		if (fileExtension == getExtensionN(i)->getExtensionName())
			return (getExtensionN(i));
	}
	return (NULL);
}
