/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/16 17:58:15 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 13:06:57 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServersConfig/ServersConfig.hpp"
#include "Server/Server.hpp"
#include "Server/Cluster/Cluster.hpp"
#include "Server/Request/RequestConfig.hpp"
#include "Server/Response/AutoIndex/AutoIndex.hpp"
#include "utils/utils.hpp"
#include "Server/Locations/NewLocations.hpp"

int main(int argc, char **argv)
{
	try
	{
		ServersConfig config = ServersConfig(argc, argv);
		Cluster *cluster = new Cluster(config.getServers());
		if (cluster->init()) {
			std::cerr << RED << "Cluster initialisation error" << RESET << std::endl;
			delete cluster;
			return (1);
		}
		cluster->run();
		cluster->clear();
		delete cluster;
	}
	catch (std::exception &e)
	{
		std::cerr << RED << "Exception occured : " << e.what() << RESET << std::endl;
	}
	return (0);
}
