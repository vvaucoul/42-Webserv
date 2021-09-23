/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 13:30:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 12:49:47 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLUSTER_HPP
# define CLUSTER_HPP

# include <vector>
# include <signal.h>
# include "../Server.hpp"
# include "../../utils/utils.hpp"


class Cluster {
private:
	// Servers & Sockets
	std::vector<Server *> 		_servers;
	std::map<int, Server *>		_sockets; // Accept client connections
	std::vector<int>			_sendInfoSockets;

	// FD
	fd_set						_readFD;
	fd_set						_writeFD;
	fd_set						_errorFD;

	// FD Info
	int							_maxFD;
	int							_fdSize;
	int							_selectRet;

	// Cluster values
	fd_set						_clusterFD;

public:
	Cluster (std::vector<Server *> servers);
	Cluster(const Cluster &cpy);
	Cluster &operator=(const Cluster &cpy);
	virtual ~Cluster (void);

	// Cluster Setup
	int init();
	int run();
	int clear();

	// Cluster Connection
	int Accept();
	int Receive();
	int Send();

	int 		ClusterSelect(int maxFD, fd_set &readFD, fd_set &writeFD, fd_set &errorFD, struct timeval &timeout);
	static void	signalHandler(int signal);
	std::string	getDot();
	Server 		*getServerBySocket(int socket);

	class NoServerCreated : public std::exception {
		virtual const char *what() const throw() {return ("No server created");};
	};
};

#endif
