/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:13:03 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/06 14:20:29 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "nc.hpp"

int main(int argc, char const *argv[])
{
	if (argc <= 1) {
		std::cout << "usage: ./nc_darwin file.req port[Default=8080] ip[Default=127.0.0.1]" << '\n';
		return (1);
	} else if (!strcmp(argv[1], "-h")) {
		std::cout << "usage: ./nc_darwin file.req port[Default=8080] ip[Default=127.0.0.1]" << '\n';
		return (0);
	}
	else {
		try {
			std::string file = std::string(argv[1]);
			size_t port = argc >= 3 ? std::atoi(argv[2]) : 8080;
			std::string	ip = argc >= 4 ? std::string(argv[3]) : "127.0.0.1";

			NC *nc = new NC(file, port, ip);

			nc->ProcessNC();
			delete nc;
		}
		catch (std::exception &e) {
			std::cerr << "Error: " << e.what() << '\n';
		}
	}
	return (0);
}
