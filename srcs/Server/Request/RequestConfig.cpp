/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestConfig.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-faou <mle-faou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/22 15:09:52 by mle-faou          #+#    #+#             */
/*   Updated: 2021/09/20 12:56:00 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestConfig.hpp"

//**************************//
//			DEFAULT 		//
//**************************//

RequestConfig::RequestConfig(void) {
	_isWaitingForContent = false;
	_invalidContent = false;
	_invalidHeader = false;
}
RequestConfig::RequestConfig(RequestConfig const &cpy) {
	*this = cpy;
}
RequestConfig &RequestConfig::operator=(RequestConfig const &cpy) {
	if (this == &cpy)
		return (*this);
	_config = cpy._config;
	_invalidHeader = cpy._invalidHeader;
	_invalidContent = cpy._invalidContent;
	_cp = cpy._cp;
	_content = cpy._content;
	_isWaitingForContent = cpy._isWaitingForContent;
	return (*this);
}
RequestConfig::~RequestConfig(void) {

}

//**************************//
//			UTILS	 		//
//**************************//

void RequestConfig::ParseContent(std::string request) {
	size_t pos = request.find("\r\n\r\n") + 4;

	if (pos == NPOS) {
		return ;
	}
	std::string originalContent = request.substr(pos, request.length() - pos);
	_config.insert(_config.end(), FT::getPair("Original-Content", originalContent));
}

//**************************//
//		REQUEST CONFIG 		//
//**************************//

RequestConfig::RequestConfig(std::string request) {
	_isWaitingForContent = false;
	_invalidHeader = false;
	_invalidContent = false;

	ParseContent(request);
	request = removeChars(request, '\r');

	// Method
	{
		size_t pos = 0;

		if ((pos = request.find(' ')) == NPOS) {
			std::cerr << RED <<"Error Request config ! " << RESET << '\n';
			return ;
		}

		std::string method = request.substr(0, pos);
		method = cleanResize(method);
		_config.insert(_config.end(), FT::getPair("Method", method));
	}
	// page
	{
		size_t pos = 0;

		if ((pos = request.find(' ') + 1) == NPOS) {
			std::cerr << RED <<"Error Request config ! " << RESET << '\n';
			_invalidHeader = true;
		}

		std::string page = subcstr(request, pos, ' ');
		page = cleanResize(page);
		_config.insert(_config.end(), FT::getPair("Page", fixSlash(page)));
	}
	// protocol
	{
		size_t pos = 0;

		if ((pos = nfind(request, " ", 2)) == NPOS) {
			std::cerr << RED <<"Error Request config ! " << RESET << '\n';
			_invalidHeader = true;
		}
		std::string protocol = subcstr(request, pos, '\n');
		protocol = cleanResize(protocol);
		_config.insert(_config.end(), FT::getPair("Protocol", protocol));
	}
	// other values
	{
		std::string requestHeader;
		std::string requestContent;
		size_t pos = request.find("\n\n");

		requestHeader = request.substr(0, pos);
		if (pos != NPOS) {
			requestContent = request.substr(pos + 2, request.length());
			_content = requestContent;
		}
		std::vector<std::string> v = strToTab(requestHeader, '\n');
		v.erase(v.begin());
		for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it) {
			std::string key, value;

			if ((*it) == "\n"
			|| (*it) == "\n\n"
			|| (*it).empty())
				break;

			if (nbChar(*it, ':') <= 0) {
				_invalidHeader = true;
			}
			key = it->substr(0, it->find_first_of(':'));
			value = it->substr(it->find_first_of(' ') + 1, it->length());

			key = cleanResize(key);
			value = cleanResize(value);

			if (key.empty() || value.empty()) {
				_invalidHeader = true;
			}
			else
				_config.insert(_config.end(), FT::getPair(key, value));
		}
	}
	// Unchunk request Content
	{
		if (REQUEST_METHOD == "POST"
		&& _config["Transfer-Encoding"] == "chunked") {
			displayDimBar(DIM_BAR_SIZE);
			_cp.AddChunk(_content);
			_content = _cp.getResult();
			_invalidContent = _cp.getError();
			displayDimBar(DIM_BAR_SIZE);
		}
		_config.insert(_config.end(), FT::getPair("Content", _content));
	}
	// Wait Post Content Multiple Request
	{
		if (REQUEST_METHOD == "POST" && (size_t)atoi(_config["Content-Length"].c_str()) > _content.length()) {
			_isWaitingForContent = true;
			return ;
		} else
			_isWaitingForContent = false;
	}
}

//**************************//
//			PRINT	 		//
//**************************//

void RequestConfig::printRequestConfig(void) {
	displayDimBar(DIM_BAR_SIZE);
	std::cout << YELLOW << "\n### CONFIGURATION - HTTP Header/Content ###\n" << '\n';
	std::cout << DIM << CYAN << "-----------------------------------------" << RESET << '\n';
	for (std::map<std::string, std::string>::iterator it = _config.begin(); it != _config.end(); ++it) {
		if ((it)->first == "Content")
			continue;
		std::cout << GREEN << "<" << (it)->first << ">" << "\r\t\t\t" << CYAN << "<" << (it)->second << ">" << std::endl << RESET;
	}
	std::cout << DIM << CYAN << "-----------------------------------------" << RESET << '\n';
	std::cout << GREEN << "<Content>\r\t\t\t" << _content << '\n';
	std::cout << DIM << CYAN << "-----------------------------------------" << RESET << '\n';
	displayDimBar(DIM_BAR_SIZE);
}

//**************************//
//	      OPERATORS	 		//
//**************************//

std::string &RequestConfig::operator[](std::string const &key) {
	return (_config[key]);
}
