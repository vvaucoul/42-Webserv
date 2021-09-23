/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 14:45:19 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 13:03:48 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Methods.hpp"

//**************************//
//			DEFAULT			//
//**************************//

Methods::Methods (std::string const &method,
	HtmlChecker const &htmlChecker,
	std::map<std::string, std::string> const &responsesMap,
	Response *response, Server *srv, Server *rootServer)
: _currentMethod(method){
	if (method.empty())
		throw ("Method empty !");
	_htmlChecker = new HtmlChecker(htmlChecker);
	_responsesMap = responsesMap;
	_response = response;
	_clServer = srv;
	_clRootServer = rootServer;
	_deleteRessourceDeleted = false;
}
Methods::Methods (Methods const &cpy) {
	*this = cpy;
}
Methods &Methods::operator=(Methods const &cpy) {
	if (this == &cpy) {
		return (*this);
	}
	else {
		_currentMethod = cpy._currentMethod;
		_htmlChecker = cpy._htmlChecker;
		_responsesMap = cpy._responsesMap;
		_response = cpy._response;
		_deleteRessourceDeleted = cpy._deleteRessourceDeleted;
		_clServer = cpy._clServer;
		_clRootServer = cpy._clRootServer;
	}
	return (*this);
}
Methods::~Methods () {
	if (_htmlChecker)
		delete _htmlChecker;
}

//**************************//
//			UTILS			//
//**************************//

std::string Methods::getCorrectFilePath(std::string const &str) {
	std::string file = str.substr(nrfind(str, "/", 1), str.length());

	std::string path;
	{
		NewLocations loc = NEW_LOCATIONS(_clRootServer);
		t_locationNode node = loc.searchLocation(str);

		if (loc.isFileExtension(str)) {
			path = fixSlash(_clRootServer->getRoot() + "/" + str);
		}
		else {
			path = fixSlash(_clRootServer->getRoot() + "/" + str);
		}
	}
	if (isFile(path))
		return (path);
	else
		return ("");
}

//**************************//
//			METHODS			//
//**************************//

t_methodResponse Methods::processMethods() {
	if (!_currentMethod.compare("GET"))
		return (Get());
	else if (!_currentMethod.compare("POST"))
		return (Post());
	else if (!_currentMethod.compare("DELETE"))
		return (Delete());
	else
		return ((t_methodResponse){400, "", true});
}

//**************************//
//			GET				//
//**************************//

t_methodResponse Methods::Get() {

	if (_response->getServer()->getAutoIndex()) {
		try {
			AutoIndex index(_response->getServer()->getIP(), _response->getAutoIndexPath(_response->getFilePath()));
			return ((t_methodResponse){200, index.getPage(), true});
		}
		catch (std::exception &e) {
			std::cerr << RED << "Error : " << e.what() << RESET << '\n';
		}
		return ((t_methodResponse){400, "", true});
	}
	return ((t_methodResponse){200, _htmlChecker->getContentFile(), true});
}

//**************************//
//			POST			//
//**************************//

std::map<std::string, std::string> Methods::ConvertX_WWW_FORM_URLENCODED(std::string const &content) {
	std::map<std::string, std::string> formUrlencodedMap;

	if (!content.empty()) {
		std::string current = "", key = "", value = "";
		size_t i = 0;
		while (content[i]) {
			if (content[i] != '=' && content[i] != '&')
			current += content[i];
			if (content[i] == '=') {
				key = current;
				current.clear();
			}
			else if (content[i] == '&') {
				value = current;
				current.clear();
				formUrlencodedMap.insert(formUrlencodedMap.end(), FT::getPair(key, value));
				current = value = key = "";
			}
			++i;
		}
	}
	else {
		formUrlencodedMap = std::map<std::string, std::string>();
	}
	return (formUrlencodedMap);
}

t_methodResponse Methods::PostFormData(t_Post &post) {

	std::vector<t_Post_Boundary> v =	post.boundary;
	std::string							contentRet;
	size_t								code = 204;

	for (std::vector<t_Post_Boundary>::iterator it = v.begin(); it != v.end(); ++it ) {
		if (!(*it).boundaryValues[POST_FORM_DATA_FILENAME].empty()) {
			try {
				code = writePostFile((*it).boundaryValues[POST_FORM_DATA_CONTENT], (*it).boundaryValues[POST_FORM_DATA_FILENAME], 0);
				contentRet += (*it).boundaryValues[POST_FORM_DATA_FILENAME] + " ";
			}
			catch (std::exception &e) {
				std::cerr << RED << "Error: " << e.what() << RESET << '\n';
				return ((t_methodResponse){500, "", false});
			}
		}
	}
	return ((t_methodResponse){code, contentRet + (code == 205 ? "Modified !" : code == 202 ? "Accepted !" : "created !"), true});
}
t_methodResponse Methods::PostURLEncoded(t_Post &post) {
	CGI cgi(_response->getRequest(), _clServer, _clRootServer, post.ressourceLocation, &post);
	_response->updateCGIResult(cgi.getMapResult());
	size_t code = cgi.getStatusCode();
	return ((t_methodResponse){(size_t)code, cgi.getResult(), CODE_OK(code)});
}
std::vector<t_Post_Boundary> Methods::CreateMultipartBoundary(std::string const &content, std::string const &boundary) {


	std::vector<t_Post_Boundary> p;
	std::map<std::string, std::string> m;

	std::string MultiPartContent = "";

	std::string nContent = content;

	size_t header_start, header_end;
	size_t content_start, content_end;

	header_start = 0;

	header_end = content.find("Content-Type: ") + 14;
	header_end = content.find("\n", header_end) + 2;

	content_start = header_end + 1;
	content_end = content.find(std::string(boundary), content_start) - header_end - 3 - 2;

	std::string noRContent = removeChars(content, '\r');
	std::vector<std::string> v = strToTab(noRContent.substr(header_start, header_end - header_start), '\n');

	for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it) {
		std::string line = *it;
			if ((line == boundary || contain(line, boundary)) && !m.empty()) {
				if (!MultiPartContent.empty())
				p.push_back((t_Post_Boundary){boundary, m});
				m.clear();
				MultiPartContent.clear();
				continue ;
			} else if (contain(line, ":")) {
			size_t pos = 0;
			size_t pos2 = 0;
			std::string key, value;

			pos = line.find(":");
			key = line.substr(0, pos);

			pos = line.find(" ", pos) + 1;
			pos2 = line.find(";", pos);
			if (pos2 == NPOS)
				pos2 = line.length();

			value = line.substr(pos, pos2 - pos);
			value = removeChars(value, '"');


			m.insert(m.end(), std::pair<std::string, std::string>(key, value));

			if (pos2 == line.length())
				continue;
			pos = pos2;
			while (pos2 != NPOS && pos2 < line.length()) {

				pos = line.find(" ", pos) + 1;
				pos2 = line.find("=", pos);

				key = line.substr(pos, pos2 - pos);

				pos = pos2 + 1;
				pos2 = line.find(";", pos);
				if (pos2 == NPOS) {
					pos2 = line.length();
					value = line.substr(pos, pos2 - pos);
					value = removeChars(value, '"');
					m.insert(m.end(), std::pair<std::string, std::string>(key, value));
					break ;
				}

				value = line.substr(pos, pos2 - pos);
				value = removeChars(value, '"');


				m.insert(m.end(), std::pair<std::string, std::string>(key, value));
			}
		}
	}

	// Content
	MultiPartContent = content.substr(content_start, content_end);
	m.insert(m.end(), std::pair<std::string, std::string>("Content", MultiPartContent));
	p.push_back((t_Post_Boundary){boundary, m});

	size_t i = 0;
	for (std::vector<t_Post_Boundary>::iterator it = p.begin(); it != p.end(); ++it) {
		for (std::map<std::string, std::string>::iterator itMap = (*it).boundaryValues.begin(); itMap != (*it).boundaryValues.end(); ++itMap) {
		}
		++i;
	}
	return (p);
}
t_Post Methods::InitPost() {
	bool				isChunked;
	E_PostHeaderType	contentType = NONE;
	std::string			boundary;
	std::string			ressourceLocation;
	std::string			content, originalContent;
	std::map<std::string, std::string>	formUrlencodedMap;
	std::vector<t_Post_Boundary> VectorBoundary;

	std::string currentRessource;

	isChunked = _response->getRequest()->getConfig()["Transfert-Encoding"] == "Chunked" ? true : false;
	currentRessource = _response->getRequest()->getConfig()["Content-Type"];
	boundary = "";
	if (!currentRessource.empty()) {
		if (contain(currentRessource, "multipart/form-data")) {
			contentType = FORM_DATA;
			boundary = subsstr(currentRessource, "boundary=", "", true);;
		} else if (contain(currentRessource, "application/x-www-form-urlencoded")) {
			contentType = X_WWW_FORM_URLENCODED;
		} else if (contain(currentRessource, "raw")
			|| contain(currentRessource, "text-plain")) {
				contentType = RAW;
		} else if (contain(currentRessource, "binary")) {
			contentType = BINARY;
		} else if (contain(currentRessource, "graphql")) {
			contentType = GRAPHQL;
		} else
			contentType = NONE;
	}
	else
		contentType = NONE;
	// Ressource Location
	{
		ressourceLocation = fixSlash(_clRootServer->getRoot() + "/" + _response->getRequest()->getConfig()["Page"]);
	}

	// content
	{
		content = _response->getRequest()->getConfig()["Content"];
		originalContent = _response->getRequest()->getConfig()["Original-Content"];
	}

	// form url encoded
	{
		if (contentType == X_WWW_FORM_URLENCODED) {
			formUrlencodedMap = ConvertX_WWW_FORM_URLENCODED(content);
		}
	}
	// Form Data (Boundary)
	{
		if (contentType == FORM_DATA) {
			VectorBoundary = CreateMultipartBoundary(originalContent, boundary);
		}
	}
	return ((t_Post){isChunked, contentType, VectorBoundary, ressourceLocation, content, formUrlencodedMap, true});
}
int Methods::writePostFile(std::string const &fileContent, std::string const &fileName, size_t const index) {

	if (_clServer->getAcceptFilesPath().empty()) {
		if (_clServer->getIsExtension()) {
			Server *parent = _clServer->getParentLocation();
			if (parent->getAcceptFilesPath().empty())
				return (202);
		}
		else
			return (202);
	}

	bool modified = false;

	std::string locationPath = NEW_LOCATIONS(_clServer).getLocationPathFromServer(_clServer);
	std::string folder = _clServer->getAcceptFilesPath();
	std::string res = std::string(fixSlash(locationPath + "/" + folder + "/" + fileName)) + (index > 0 ? FT::to_string(index) : "");
	int	fd;

	if (!isDir(fixSlash(locationPath + "/" + "/" + folder))) {
		system(std::string(std::string("mkdir -p ") + fixSlash(locationPath + "/" + "/" + folder)).c_str());
	}

	if (isFile(res.c_str())) {
		modified = true;
		unlink(res.c_str());
	}
	fd = open(res.c_str(), O_RDWR | O_CREAT, 0644);
	if (fd <= 0) {
		throw PostWriteFileError();
	} else {
		write(fd, fileContent.c_str(), fileContent.length());
		return (modified == true ? 202 : 201);
	}
	return (201);
}
t_methodResponse Methods::Post() {
	t_Post	post = InitPost();

	if (post.isValid == false) {
		return ((t_methodResponse){404, "", false});
	}
	else if (_response->getRequest()->getRequestConfig()["Content"].empty()) {
		return ((t_methodResponse){404, "", false});
	}
	else {
		switch ((int)post.contentType) {
			case NONE: {
				return ((t_methodResponse){404, "", false});
			}
			case FORM_DATA: {
				return (PostFormData(post));
			}
			case X_WWW_FORM_URLENCODED: {
				return (PostURLEncoded(post));
			}
			case RAW: {
				return ((t_methodResponse){406, "", false});
			}
			case BINARY: {
				return ((t_methodResponse){406, "", false});
			}
			case GRAPHQL: {
				return ((t_methodResponse){406, "", false});
			}
		}
	}
	return ((t_methodResponse){404, "", false});
}

//**************************//
//			DELETE			//
//**************************//

std::string Methods::DeleteGetCode(std::string const &file, size_t const code) {
	switch (code) {
		case 200:
			return (DeleteGetCode200(file));
		case 204:
			return (DeleteGetCode204(file));
	}
	return ("");
}
std::string Methods::DeleteGetCode204(std::string const &file) {
	return (
	std::string("<html>\r\n<body>\r\n<h1>Error: File " \
	+ file \
	+ " not deleted</h1>\r\n</body>\r\n</html>\n"));
}
std::string Methods::DeleteGetCode200(std::string const &file) {
	return (
	std::string("<html>\r\n<body>\r\n<h1>File " \
	+ file \
	+ " deleted.</h1>\r\n</body>\r\n</html>\n"));
}
t_methodResponse Methods::Delete() {


	struct stat st;
	std::string ressourceToRemove = getCorrectFilePath(_responsesMap["page"]);
	if (ressourceToRemove.empty())
		ressourceToRemove = _response->getFilePath();
	if (!isFile(ressourceToRemove))
		return ((t_methodResponse){204, DeleteGetCode204(ressourceToRemove), true});


	if (_deleteRessourceDeleted) {
		_deleteRessourceDeleted = false;
		return ((t_methodResponse){200, DeleteGetCode200(ressourceToRemove), true});
	}
	else if (ressourceToRemove.empty()) {
		return ((t_methodResponse){204, DeleteGetCode204(ressourceToRemove), true});
	}

	if (stat(ressourceToRemove.c_str(), &st) >= 0) {
		if (isDir(ressourceToRemove))
		{
			rmdir(ressourceToRemove.c_str());
			_deleteRessourceDeleted = true;
			return ((t_methodResponse){200, DeleteGetCode200(ressourceToRemove), true});
		}
		else if (isFile(ressourceToRemove)) {
			int ret = unlink(ressourceToRemove.c_str());

			if (ret < 0) {
				_deleteRessourceDeleted = false;
				return ((t_methodResponse){204, DeleteGetCode204(ressourceToRemove), true});
			}
			else {
				_deleteRessourceDeleted = true;
				return ((t_methodResponse){200, DeleteGetCode200(ressourceToRemove), true});
			}
		}
	}
	else {
		_deleteRessourceDeleted = false;
		return ((t_methodResponse){204, DeleteGetCode204(ressourceToRemove), true});
	}
	_deleteRessourceDeleted = false;
	return ((t_methodResponse){204, DeleteGetCode204(ressourceToRemove), true});
}
