/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/22 12:52:38 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 12:55:07 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

//**************************//
//			DEFAULT			//
//**************************//

Request::Request(void) {
	_methods = initMethods();
}
Request::Request(RequestConfig const &config, t_locationNode const &location, t_extensionNode const &extention, std::string const &root) {
	_config = config;
	_location = location;
	_extention = extention;
	_requestConfig = _config.getRequestConfig();
	_requestLocationIsValid = locationIsValid(location);
	_usePostRequest = isPostRequest();
	_useDeleteRequest = isDeleteRequest();
	_root = root;

	Request();
	checkFile();
	checkExtension();
}
Request::~Request(void) {
	_requestConfig.clear();
}
Request::Request(Request const &cpy) {
	*this = cpy;
}
Request &Request::operator=(Request const &cpy) {
	if (this == &cpy)
		return (*this);
	_config = cpy._config;
	_requestConfig = cpy._requestConfig;
	_root = cpy._root;
	_location = cpy._location;
	_extention = cpy._extention;
	_requestLocationIsValid = cpy._requestLocationIsValid;
	_searchingFile = cpy._searchingFile;
	_usePostRequest = cpy._usePostRequest;
	_useDeleteRequest = cpy._useDeleteRequest;
	_requestIsInExtension = cpy._requestIsInExtension;
	return (*this);
}

//**************************//
//			INIT			//
//**************************//

std::map<std::string, size_t> 	Request::initMethods() {
	std::map<std::string, size_t> methods;
	methods.insert(FT::getPair("GET", 0));
	methods.insert(FT::getPair("POST", 1));
	methods.insert(FT::getPair("DELETE", 2));
	return (methods);
}

void 							Request::updateRequestConfig() {
	_requestConfig.clear();
	_requestConfig = _config.getRequestConfig();
}

//**************************//
//			DEBUG			//
//**************************//

std::map<std::string, std::string> Request::debugGetRequest() {
	std::map<std::string, std::string> debugRequest;

	return (debugRequest);
}

//**************************//
//			REQUEST			//
//**************************//

bool 		Request::processRequest() {
	if (!checkProtocol())
		return (false);
	checkPage();
	return (true);
}
bool		Request::checkProtocol() {

	std::string protocol = getRequestValue("Protocol");

	if (!protocol.compare("HTTP/1.0") || !protocol.compare("HTTP/1.1"))
		return (true);
	return (false);
}
bool		Request::checkMethod() {
	return (false);
}
void 		Request::checkPage() {
	struct stat st;
	std::string page = getRequestValue("Page");

	if (stat(page.c_str(), &st) < 0) {
		std::cerr << "Error Stat : " << strerror(errno) << '\n';
	}
	else {
		if (S_ISDIR(st.st_mode)) {
		}
	}
}

void		Request::checkFile() {
	_searchingFile = searchingFile(getConfig()["Page"], _root);
}
void		Request::checkExtension() {
	std::string page = getRequestValue("Page");

	if (_extention.found) {
		_requestIsInExtension.first = _extention;
		_requestIsInExtension.second = true;
	} else {
		_requestIsInExtension.first = (t_extensionNode){NULL, NULL, false};
		_requestIsInExtension.second = false;
	}
}

//**************************//
//		  REQUEST POST		//
//**************************//

void 		Request::processPostRequest(std::string const &rcvinfo) {

	(void)rcvinfo;
	if ((_usePostRequest = isPostRequest()) == true) {
	}
}

bool		Request::isPostRequest() {
	if (!_config.getRequestConfig()["Method"].compare("POST")
	|| !_config.getRequestConfig()["Method"].compare("PUT")) {
		return (true);
	}
	return (false);
}
bool		Request::isDeleteRequest() {
	if (!_config.getRequestConfig()["Method"].compare("DELETE")) {
		return (true);
	}
	return (false);
}

//**************************//
//			UTILS			//
//**************************//

std::string	Request::getRequestValue(std::string const &value) {
	for (std::map<std::string, std::string>::iterator it = _requestConfig.begin(); it != _requestConfig.end(); ++it ) {
		if (it->first == value)
			return (it->second);
	}
	return ("");
}

//**************************//
//		   OPERATORS		//
//**************************//

std::string &Request::operator[](std::string const &key) {
	return (_requestConfig[key]);
}
