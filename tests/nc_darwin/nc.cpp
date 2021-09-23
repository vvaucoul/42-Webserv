/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nc.cpp                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/24 14:53:42 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/19 10:20:15 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "nc.hpp"

NC::NC(std::string const &file, size_t const port, std::string const ip) :
_file(file), _port(port), _ip(ip) {
}

NC::~NC() {

}

NC::NC(NC const &cpy) {
	*this = cpy;
}

NC &NC::operator=(NC const &cpy) {
	if (this == &cpy) {
		return (*this);
	}
	_file = cpy._file;
	_port = cpy._port;
	_ip = cpy._ip;
	return (*this);
}

void NC::ReadFile() {
	int fd = open(_file.c_str(), O_RDONLY);
	char buffer;
	int fc = 0;

	if (fd <= 0)
		throw InvalidFile();
	do {
		fc = read(fd, &buffer, 1);
		_output += buffer;
	} while(fc > 0);

	std::cout << "\nFile: " << _output << '\n';
	usleep(500000);

}

void NC::ConvertFileToCurl() {
	for (std::string::iterator it = _output.begin(); it != _output.end(); ++it) {
		if ((*it) == '\r') {
			_output.erase(it);
		}
	}

	{
		size_t pos = _output.find(" ");
		std::string method = _output.substr(0, pos);
		size_t pos2 = _output.find(" ", pos + 1);
		std::string page = _output.substr(pos + 1, pos2 - pos - 1);

		_cmd = "curl -X " + method;
		_cmd += " " + _ip + ":" + std::to_string(_port) + page;
	}

	size_t pos = 0;
	size_t save = 0;
	while (pos != std::string::npos) {
		size_t last;
		std::string sub;

		pos = _output.find("\n", save) + 1;
		last = _output.find("\n", pos);
		sub = _output.substr(pos, last - pos);


		if (_output.substr(pos, last - pos) == ""
		|| _output.substr(pos, last - pos).empty()
		|| last == std::string::npos
		|| pos == std::string::npos
		|| last >= _output.length())
			break ;

		_cmd += " -H " + std::string("\"") + sub + std::string("\"");

		save = pos + 1;
	}
	{
		pos = _output.find("\n\n") + 2;
		std::string content = _output.substr(pos, _output.length() - pos - 2);

		_cmd += " -d " + std::string("\"") + content + std::string("\"");
	}

	std::cout << "\nNewFile: " << _cmd << '\n';

	usleep(500000);
}

void NC::SendCurlCMD() {
	system(_cmd.c_str());
	std::cout << "\nResponse: " << '\n';
}

void NC::ProcessNC() {
	std::cout << "ReadingFile..." << '\n';
	ReadFile();
	usleep(50000);
	std::cout << "Converting file..." << '\n';
	ConvertFileToCurl();
	usleep(50000);
	std::cout << "Sending Request..." << '\n';
	SendCurlCMD();
	usleep(50000);
}
