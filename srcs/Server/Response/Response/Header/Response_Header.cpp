/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response_Header.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/21 18:07:12 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 13:05:06 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../Response.hpp"

//**************************//
//		 	HEADER			//
//**************************//

std::string	Response::ResponseHeaderManager() {
	setValues();
	return (SetHeaderValues());
}
std::string	Response::SetHeaderValues() {
	std::string header;

	if (_responsesMap["METHOD"] != "") {
		header += _responsesMap["METHOD"] + "\r\n";
	}
	if (_responsesMap["ALLOW"] != "") {
		header += "Allow: " + _responsesMap["ALLOW"] + "\r\n";
	}
	if (_responsesMap["CONTENT-LANGUAGE"] != "") {
		header += "Content-Language: " + _responsesMap["CONTENT-LANGUAGE"] + "\r\n";
	}
	if (_responsesMap["CONTENT-LENGTH"] != "") {
		header += "Content-Length: " + _responsesMap["CONTENT-LENGTH"] + "\r\n";
	}
	if (_responsesMap["CONTENT-LOCATION"] != "") {
		header += "Content-Location: " + _responsesMap["CONTENT-LOCATION"] + "\r\n";
	}
	if (_responsesMap["CONTENT-TYPE"] != "") {
		header += "Content-Type: " + _responsesMap["CONTENT-TYPE"] + "\r\n";
	}
	if (_responsesMap["DATE"] != "") {
		header += "Date: " + _responsesMap["DATE"] + "\r\n";
	}
	if (_responsesMap["LAST-MODIFIED"] != "") {
		header += "Last-Modified: " + _responsesMap["LAST-MODIFIED"] + "\r\n";
	}
	if (_responsesMap["LOCATION"] != "") {
		header += "Location: " + _responsesMap["LOCATION"] + "\r\n";
	}
	if (_responsesMap["RETRY-AFTER"] != "") {
		header += "Retry-After: " + _responsesMap["RETRY-AFTER"] + "\r\n";
	}
	if (_responsesMap["SERVER"] != "") {
		header += "Server: " + _responsesMap["SERVER"] + "\r\n";
	}
	if (_responsesMap["TRANSFER-ENCODING"] != "") {
		header += "Transfer-Encoding: " + _responsesMap["TRANSFER-ENCODING"] + "\r\n";
	}
	if (_responsesMap["WWW-AUTHENTICATE"] != "") {
		header += "WWW-Authenticate: " + _responsesMap["WWW-AUTHENTICATE"] + "\r\n";
	}
	if (_responsesMap["CONNECTION"] != "") {
		header += "Connection: " + _responsesMap["CONNECTION"] + "\r\n";
	}
	if (_responsesMap["ACCEPT-LANGUAGE"] != "") {
		header += "Accept-Language: " + _responsesMap["ACCEPT-LANGUAGE"] + "\r\n";
	}

	return (header);
}
void Response::setValues() {
	setMethod();
	setAllow();
	setContentLanguage();
	setContentLength();
	setContentLocation();
	setContentType("");
	setDate();
	setLastModified();
	setLocation();
	setRetryAfter();
	setServerName();
	setTransferEncoding();
	setWWAuthenticate();
	setErrorValues();
	setConnection();
	setAcceptLanguage();
}

//**************************//
//	 SET HEADER RESPONSES	//
//**************************//

void 	Response::setMethod() {
	std::string requestHTTP = _clRequest->getRequestValue("Protocol");
	if (_methodMap.code != 200 && _methodMap.valid) {
		_code = _methodMap.code;
	}
	_responsesMap["METHOD"] = requestHTTP + " " + FT::to_string(_code) + " " + _errors[_code];
}
void 	Response::setAllow() {
	if (_code == 405
		|| (METHOD == "OPTIONS")) {
			std::vector<std::string> m = _clServer->getMethod();

			for (std::vector<std::string>::iterator it = m.begin(); it != m.end(); ++it) {
				_responsesMap["ALLOW"] += *it;
				if (it + 1 != m.end())
					_responsesMap["ALLOW"] += ", ";
			}
	}
}
void 	Response::setContentLanguage() {
}
void 	Response::setContentLength() {
	if ((CODE_OK(_code)) && METHOD == "POST") {
			if (USE_CGI)
				_responsesMap["CONTENT-LENGTH"] = FT::to_string(_methodMap.content.length());
			else if (!_methodMap.content.empty())
				_responsesMap["CONTENT-LENGTH"] = FT::to_string(_methodMap.content.length());
			else
				_responsesMap["CONTENT-LENGTH"] = "0";
		return ;
	}
	else if ((CODE_OK(_code)) && METHOD == "DELETE") {
		std::string retSuccessDelete = _clMethods->DeleteGetCode(_filePath, _code);
		_responsesMap["CONTENT-LENGTH"] = FT::to_string(retSuccessDelete.length());
		return ;
	}
	else {
		if (_code < 200 || _code >= 300) {
			if (_code == CODE_REDIRECT) {
				_responsesMap["CONTENT-LENGTH"] = FT::to_string(_clServer->getRedirectHTML().size());
			}
			else
				_responsesMap["CONTENT-LENGTH"] = FT::to_string(generateErrorPage().size());
		}
		else {
			if (_clServer->getAutoIndex()) {
				AutoIndex index(_clServer->getIP(), getAutoIndexPath(_filePath));
				_responsesMap["CONTENT-LENGTH"] = FT::to_string(index.getPage().length());
			}
			else if (USE_CGI) {
				_responsesMap["CONTENT-LENGTH"] = FT::to_string(_methodMap.content.length());
			}
			else {
				_responsesMap["CONTENT-LENGTH"] = FT::to_string(_htmlChecker->getFileLength());
			}
		}
	}
}
void 	Response::setContentLocation() {
	if (!CODE_OK(_code)) {
		_responsesMap["CONTENT-LOCATION"] = "";
		return ;
	}

	t_locationNode node = NEW_LOCATIONS(_clRootServer).searchLocation(NEW_LOCATIONS(_clRootServer).fixSearchPathName(_filePath));
	std::string location;
	if (IS_VALID_LOCATION(node)) {
		if (node.data->getIsLocation())
			location = fixSlash(node.data->getLocationName() + "/");
		else
			location = fixSlash(node.data->getRoot() + "/");
	} else {
		location = "";
	}
	_responsesMap["CONTENT-LOCATION"] = location;
}
void 	Response::setContentType(std::string const &force) {

	if (_code >= 400 && _code < 500)
		_responsesMap["CONTENT-TYPE"] = "text/html";
	else if (CODE_OK(_code) && USE_CGI) // Si CGI executé correctement, type de text = html
		_responsesMap["CONTENT-TYPE"] = "text/html";
	else if (_code == CODE_REDIRECT)
		_responsesMap["CONTENT-TYPE"] = "text/html";
	else if (force.empty())
		_responsesMap["CONTENT-TYPE"] = _htmlChecker->getContentType();
	else
		_responsesMap["CONTENT-TYPE"] = "";

	if (_responsesMap["CONTENT-TYPE"] == "text/html"
		&& CODE_OK(_code))
		_responsesMap["CONTENT-TYPE"] += "; charset=utf-8";
}
void 	Response::setDate() {
	struct timeval tv;
	struct timezone tz;
	struct tm		*tm;
	time_t			rawtime;
	char			buffer[96];

	if (!CODE_OK(_code)) {
		_responsesMap["DATE"] = "";
		return ;
	}
	time(&rawtime);
	gettimeofday(&tv, &tz);

	tm = gmtime(&rawtime);
	tm->tm_hour += 2; // set par rapport a la location & utc time
	strftime(buffer, 96, "%a, %d %b %Y %H:%M:%S GMT", tm);

	_responsesMap["DATE"] = std::string(buffer);
}
void 	Response::setLastModified() {
	char buffer[96];

	if ((CODE_OK(_code))) {
		strftime(buffer, 96, "%a, %d %b %Y %H:%M:%S GMT", _htmlChecker->getLastModifiedFile());
		_responsesMap["LAST-MODIFIED"] = std::string(buffer);
	}
	else
		_responsesMap["LAST-MODIFIED"] = "";
}
void 	Response::setLocation() {
	if (_code == 201
		|| _code == 301
		|| _code == 302
		|| _code == 303
		|| _code == 307
		|| _code == 308)
		_responsesMap["CONTENT-LOCATION"] = _clServer->getRedirectLink();
	else
		_responsesMap["LOCATION"] = "";
}
void 	Response::setRetryAfter() {
	if (_clientRateLimit.first) {
		struct timeval tv;
		struct timezone tz;
		struct tm		*tm;
		char			buffer[96];

		gettimeofday(&tv, &tz);

		tm = gmtime(&tv.tv_sec);
		tm = addTime(tm, RATE_LIMIT_TIME);
		strftime(buffer, 96, "%a, %d %b %Y %H:%M:%S GMT", tm);
		_responsesMap["RETRY-AFTER"] = std::string(buffer) + "\r\n" + "Retry-After: " + FT::to_string(RATE_LIMIT_TIME);
	}
	else
		_responsesMap["RETRY-AFTER"] = "";
}
void 	Response::setServerName() {
	_responsesMap["SERVER"] = _clServer->getName();
}
void 	Response::setTransferEncoding() {
	_responsesMap["TRANSFER-ENCODING"] = "identity";
}
void 	Response::setWWAuthenticate() {
	if (_code == 401)
		_responsesMap["WWW-AUTHENTICATE"] = "Basic realm=\"Accès au site de staging\", charset=\"UTF-8\"";
	else
		_responsesMap["WWW-AUTHENTICATE"] = "";
}
void	Response::setConnection() {
	if (_code >= 500)
		_responsesMap["CONNECTION"] = "close";
	else
		_responsesMap["CONNECTION"] = "keep-alive";
}
void 	Response::setAcceptLanguage() {

	if (_code == 406) {
		size_t i = 0;
		const char *ls[] = LANGUAGES;

		for (i = 0; i < NB_LANGUAGES - 1; i++) {
			_responsesMap["ACCEPT-LANGUAGE"] += std::string(ls[i]) + ", ";
		}
		_responsesMap["ACCEPT-LANGUAGE"] += std::string(ls[i]);
	}
}
