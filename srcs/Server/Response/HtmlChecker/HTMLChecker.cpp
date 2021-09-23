/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTMLChecker.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/23 14:21:10 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 13:02:59 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "HTMLChecker.hpp"

//**************************//
//			DEFAULT			//
//**************************//

HtmlChecker::HtmlChecker (void) {
	printError("Wrong Constructor");
	_file = "";
	_default_file = "";
}
HtmlChecker::HtmlChecker (std::string const &file, std::string const &default_file)
: _file(file), _default_file(default_file) {
	_fileFD = -1;
	if ((_file.empty()
	&& (!isDir(_file)
	|| isFile(_file)))
	&& ((!_default_file.empty()
	&& (!isDir(_default_file)
	|| isFile(_default_file))))) {
		_file = _default_file;
	}
}
HtmlChecker::HtmlChecker (HtmlChecker const &cpy) {
	*this = cpy;
}
HtmlChecker &HtmlChecker::operator=(HtmlChecker const &cpy) {
	if (this == &cpy) {
		return (*this);
	}
	_file = cpy._file;
	_default_file = cpy._default_file;
	_fileFD = cpy._fileFD;
	return (*this);
}
HtmlChecker::~HtmlChecker () {
}

//**************************//
//		 	UTILS			//
//**************************//

bool HtmlChecker::fileIsValid() {
	return (!_file.empty());
}
void HtmlChecker::openFile() {
	if (!fileIsValid())
		return ;
	int openRet = open(_file.c_str(), O_RDONLY);
	if (openRet == -1) {
		_fileFD = -1;
	}
	else
		_fileFD = openRet;
}
void HtmlChecker::closeFile() {
	if (_fileFD >= 0)
		close(_fileFD);
	_fileFD = 0;
}
std::string HtmlChecker::readFile() {
	char buffer[HTML_READ_BUFFER] = {0};

	if (_fileFD < 0 || !fileIsValid())
		return ("");
	int readRet = read(_fileFD, buffer, HTML_READ_BUFFER - 1);
	buffer[HTML_READ_BUFFER - 1] = 0;
	if (readRet == -1) {
		return ("");
	}
	else {
		return (std::string(buffer));
	}
	return ("");
}
size_t HtmlChecker::readFileLength() {
	char buffer[HTML_READ_BUFFER] = {0};

	if (_fileFD < 0 || !fileIsValid())
		return (0);

	int readRet = read(_fileFD, buffer, HTML_READ_BUFFER - 1);
	buffer[HTML_READ_BUFFER - 1] = 0;
	if (readRet == -1) {
		throw HtmlChecker::HTML_READ_EXCEPTION();
	}
	else {
		return (readRet);
	}
	return (0);
}
std::string HtmlChecker::getFileExtension() {
	std::string extension;

	if (!fileIsValid())
		return ("");

	extension = _file.substr(_file.rfind(".") + 1, _file.size() - _file.rfind("."));
	return (extension);
}

//**************************//
//		 HTML CHECKER		//
//**************************//

size_t HtmlChecker::getFileLength() {
	openFile();
	size_t fileLength = readFileLength();
	closeFile();
	return (fileLength);
}
std::string HtmlChecker::getContentFile() {
	openFile();
	std::string content = readFile();
	closeFile();
	return (content);
}
std::string HtmlChecker::getContentLocation() {
	return (_file);
}
std::string HtmlChecker::getContentType() {
	std::string contentType = getFileExtension();

	if (contentType == "")
		return ("");
	else if (contentType == "html")
		return ("text/html");
	else if (contentType == "css")
		return ("text/css");
	else if (contentType == "javascript")
		return ("text/javascript");
	else if (contentType == "php")
		return ("text/php");
	else if (contentType == "js")
		return ("text/javascript");
	else if (contentType == "jpg")
		return ("image/jpeg");
	else if (contentType == "jpeg")
		return ("image/jpeg");
	else if (contentType == "png")
		return ("image/png");
	else if (contentType == "bmp")
		return ("image/bpm");
	else
		return ("text/plain");
	return ("");
}
struct tm *HtmlChecker::getLastModifiedFile() {
	struct stat st;

	return (gmtime(&st.st_mtime));
}
std::string HtmlChecker::getFileLocation() {
	std::string location = "";

	location = _file.substr(0, _file.rfind("/") + 1);
	return (location);
}

//**************************//
//		   EXCEPTION		//
//**************************//

const char *HtmlChecker::HTML_FILE_EXCEPTION::what() const throw() {
	return ("\t- HtmlChecker : Invalid HTML File");
}
const char *HtmlChecker::HTML_READ_EXCEPTION::what() const throw() {
	return ("\t- HtmlChecker : Can't read file");
}
const char *HtmlChecker::HTML_OPEN_EXCEPTION::what() const throw() {
	return ("\t- HtmlChecker : Can't open file");
}
const char *HtmlChecker::HTML_STAT_EXCEPTION::what() const throw() {
	return ("\t- HtmlChecker : Error stat");
}
