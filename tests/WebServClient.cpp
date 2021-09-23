/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServClient.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/24 18:27:54 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/19 21:21:50 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <iostream>
# include <stdlib.h>
# include <stdio.h>
# include <sys/socket.h>
# include <stdlib.h>
# include <unistd.h>
# include <netinet/in.h>
# include <string.h>
# include <arpa/inet.h>

static int connection(int port, std::string file) {

	struct sockaddr_in	srvAddr;
	char				buffer[4096] = {0};

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		return (1);
	memset(&srvAddr, 0, sizeof(srvAddr));
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	srvAddr.sin_port = htons(port);

	int ret = connect(sock, (struct sockaddr *)&srvAddr, sizeof(srvAddr));

	if (ret < 0)
		return (1);

	std::cout << "SEND : " << file << '\n';

	send(sock, file.c_str(), file.size(), 0);
	read(sock, buffer, 4096 - 1);
	close (sock);
	return (0);
}

int main(int argc, char const *argv[])
{
	if (argc != 3) {
		std::cerr << "Bad arguments | Use : ./Client IP:PORT METHODE" << '\n';
		return (-1);
	}
	std::string ip = std::string(argv[1]).substr(0, std::string(argv[1]).find(":"));
	std::string port = std::string(argv[1]).substr(std::string(argv[1]).find(":") + 1, std::string(argv[1]).size());
	std::string methode = std::string(argv[2]);

	std::cout << "IP : " << ip << '\n';
	std::cout << "PORT : " << port << '\n';
	std::cout << "METHODE : " << methode << '\n';

	std::cout << "\n" << '\n';

	std::string file = methode + " / " + "HTTP/1.1" + "\r\n";
	file += "Host: " + std::string(argv[1]) + "\r\n";
	file += "Connection: keep-alive\r\n";
	file += "User-Agent: vvaucoul HTTP Client\r\n";
	file += "Accept: *\r\n";
	file += "Referer: http://" + std::string(argv[1]) + "/\r\n";
	file += "Accepted-Language: fr-FR\r\n";

	if (connection(atoi(port.c_str()), file)){
		std::cerr << "Error connection" << '\n';
		return (1);
	}

	return (0);
}
