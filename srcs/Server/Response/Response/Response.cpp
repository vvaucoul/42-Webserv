/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/15 17:20:11 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 13:06:23 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Response.hpp"

//**************************//
//			DEFAULT			//
//**************************//

Response::Response (Request *request,
					std::string const &default_file,
					std::pair<bool, t_rate_limit> const &rateLimit,
					Server *locServer,
					Server *rootServer) {

	// Set References
	{
		_clRequest = request;
		_clServer = locServer;
		_clRootServer = rootServer;
		_clientRateLimit = rateLimit;
	}
	// Update extention
	{
		if (_clRequest->requestIsInExtention().second) {
			_clServer = _clRequest->requestIsInExtention().first.data;
		}
	}

	// Update file path
	{
		if (_clRequest->isSearchingFile())
			_filePath = default_file;
		else
			_filePath = NEW_LOCATIONS(_clServer).getLocationPage((*_clRequest)["Page"]).second;
		_locationPath = NEW_LOCATIONS(_clServer).searchLocation((*_clRequest)["Page"]);
		if (!IS_VALID_LOCATION(_locationPath))
			_locationPath = NEW_LOCATIONS(_clServer).getLastValidLocation((*_clRequest)["Page"]);
		if (isFile(_filePath) &&
		(removeChars(_locationPath.path, '/') != removeChars(_clRootServer->getRoot(), '/') && nbChar((*_clRequest)["Page"], '/') > 1)) {
			_clRequest->updateRequestLocationValidation(true);
		}
		_isFile = isFile(default_file);
	}

	// Init htmlChecker & auto_index
	// Set code to 200
	{
		_htmlChecker = new HtmlChecker(default_file, _filePath);
		_useAutoIndex = _clServer->getAutoIndex();
		_code = 200;
	}

	setErrorValues();
	resetValues();
}
Response::Response (Response const &cpy) {
	*this = cpy;
}
Response &Response::operator=(Response const &cpy) {
	if (this == &cpy)
		return (*this);
	_clRequest = cpy._clRequest;
	_clServer = cpy._clServer;
	_htmlChecker = cpy._htmlChecker;
	_filePath = cpy._filePath;
	_isFile = cpy._isFile;
	_locationPath = cpy._locationPath;
	_useAutoIndex = cpy._useAutoIndex;
	_clientRateLimit = cpy._clientRateLimit;
	_code = cpy._code;
	_responsesMap = cpy._responsesMap;
	_errors = cpy._errors;
	_methodMap = cpy._methodMap;
	return (*this);
}
Response::~Response () {
}

//**************************//
//		   RESPONSES		//
//**************************//

std::string		Response::ProcessResponse() {
	std::string response;

	checkRequestValues();
	DoResponse();
	_code = _methodMap.code;
	response = ResponseHeaderManager();
	response += "\r\n";
	response += ResponseContentManager();
	delete (_htmlChecker);
	return (response);
}

// before generate header & content, execute programs (methods, cgi etc...)
void 			Response::DoResponse() {
	if (CODE_OK(_code)) {
		try {
			if (USE_CGI && METHOD == "GET") {
				CGI cgi = CGI(_clRequest, _clServer, _clRootServer, _filePath, NULL);
				_cgiResult = cgi.getMapResult();
				_code = cgi.getStatusCode();
				_methodMap = (t_methodResponse){(size_t)_code, cgi.getResult(), CODE_OK(_code)};
			}
			else {
				_clMethods = new Methods(METHOD, *_htmlChecker, _clRequest->getConfig(), this, _clServer, _clRootServer);
				_methodMap = _clMethods->processMethods();
				delete (_clMethods);
			}
		}
		catch (std::exception &e) {
			std::cerr << e.what() << std::endl;
			if (!strcmp(e.what(), "CGIContentException"))
				delete (_clMethods);
			_methodMap = (t_methodResponse){500, "", false};
		}
	}
	else {
		_methodMap = (t_methodResponse){(size_t)_code, "", false};
	}
}

void 			Response::checkRequestValues() {
	if (checkContinue()) {
		_code = 100;
	}
	else if (checkProtocol()) {
		_code = 101;
	}
	else if (checkFileCreation()) {
		_code = 201;
	}
	else if (checkNoContent()) {
		_code = 204;
	}
	else if (checkMovedPermanently()) {
		_code = 301;
	}
	else if (checkSeeOther()) {
		_code = 303;
	}
	else if (checkNotModified()) {
		_code = 304;
	}
	else if (checkPermanentRedirect()) {
		_code = 308;
	}
	else if (checkBadRequest()) {
		_code = 400;
	}
	else if (checkUnauthorized()) {
		_code = 401;
	}
	else if (checkForbidden()) {
		_code = 403;
	}
	else if (checkMethod()) {
		_code = 405;
	}
	else if (checkURITooLong()) {
		_code = 414;
	}
	else if (checkPage()) {
		_code = 404;
	}
	else if (chekNotAcceptable()) {
		_code = 406;
	}
	else if (checkGone()) {
		_code = 410;
	}
	else if (checkLength()) {
		_code = 411;
	}
	else if (checkPayload()) {
		_code = 413;
	}
	else if (checkMediaType()) {
		_code = 415;
	}
	else if (checkUnprocessableEntity()) {
		_code = 422;
	}
	else if (checkUpgradeRequired()) {
		_code = 426;
	}
	else if (checkTooManyRequests()) {
		_code = 429;
	}
	else if (checkInternalServerError()) {
		_code = 500;
	}
	else if (checkNotImplemented()) {
		_code = 501;
	}
	else if (checkServiceUnavailable()) {
		_code = 503;
	}
	else if (checkHTTPVersion()) {
		_code = 505;
	}
	else if (checkInsufficientStorage()) {
		_code = 507;
	}
	else {
		_code = 200;
	}
}

//**************************//
//		   	UTILS			//
//**************************//

void 	Response::setErrorValues() {
	_errors.insert(FT::getPair(100, "Continue"));
	_errors.insert(FT::getPair(101, "Switching Protocols"));
	_errors.insert(FT::getPair(103, "Early Hints"));
	_errors.insert(FT::getPair(200, "OK"));
	_errors.insert(FT::getPair(201, "Created"));
	_errors.insert(FT::getPair(202, "Accepted"));
	_errors.insert(FT::getPair(203, "Non-Authoritative Information"));
	_errors.insert(FT::getPair(204, "No Content"));
	_errors.insert(FT::getPair(205, "Reset Content"));
	_errors.insert(FT::getPair(206, "Partial Content"));
	_errors.insert(FT::getPair(300, "Multiple Choices"));
	_errors.insert(FT::getPair(301, "Moved Permanently"));
	_errors.insert(FT::getPair(302, "Found"));
	_errors.insert(FT::getPair(303, "See Other"));
	_errors.insert(FT::getPair(304, "Not Modified"));
	_errors.insert(FT::getPair(307, "Temporary Redirect"));
	_errors.insert(FT::getPair(308, "Permanent Redirect"));
	_errors.insert(FT::getPair(400, "Bad Request"));
	_errors.insert(FT::getPair(401, "Unauthorized"));
	_errors.insert(FT::getPair(402, "Payment Required"));
	_errors.insert(FT::getPair(403, "Forbidden"));
	_errors.insert(FT::getPair(404, "Not Found"));
	_errors.insert(FT::getPair(405, "Method Not Allowed"));
	_errors.insert(FT::getPair(406, "Not Acceptable"));
	_errors.insert(FT::getPair(407, "Proxy Authentication Required"));
	_errors.insert(FT::getPair(408, "Request Timeout"));
	_errors.insert(FT::getPair(409, "Conflict"));
	_errors.insert(FT::getPair(410, "Gone"));
	_errors.insert(FT::getPair(411, "Length Required"));
	_errors.insert(FT::getPair(412, "Precondition Failed"));
	_errors.insert(FT::getPair(413, "Payload Too Large"));
	_errors.insert(FT::getPair(414, "URI Too Long"));
	_errors.insert(FT::getPair(415, "Unsupported Media Type"));
	_errors.insert(FT::getPair(416, "Range Not Satisfiable"));
	_errors.insert(FT::getPair(417, "Expectation Failed"));
	_errors.insert(FT::getPair(418, "I'm a teapot"));
	_errors.insert(FT::getPair(422, "Unprocessable Entity"));
	_errors.insert(FT::getPair(425, "Too Early"));
	_errors.insert(FT::getPair(426, "Upgrade Required"));
	_errors.insert(FT::getPair(428, "Precondition Required"));
	_errors.insert(FT::getPair(429, "Too Many Requests"));
	_errors.insert(FT::getPair(431, "Request Header Fields Too Large"));
	_errors.insert(FT::getPair(451, "Unavailable For Legal Reasons"));
	_errors.insert(FT::getPair(500, "Internal Server Error"));
	_errors.insert(FT::getPair(501, "Not Implemented"));
	_errors.insert(FT::getPair(502, "Bad Gateway"));
	_errors.insert(FT::getPair(503, "Service Unavailable"));
	_errors.insert(FT::getPair(504, "Gateway Timeout"));
	_errors.insert(FT::getPair(505, "HTTP Version Not Supported"));
	_errors.insert(FT::getPair(506, "Variant Also Negotiates"));
	_errors.insert(FT::getPair(507, "Insufficient Storage"));
	_errors.insert(FT::getPair(508, "Loop Detected"));
	_errors.insert(FT::getPair(510, "Not Extended"));
	_errors.insert(FT::getPair(511, "Network Authentication Required"));
	_errors.insert(FT::getPair(520, "Unknown Error"));
}
void 	Response::resetValues() {
	_responsesMap.clear();

	const char *values[RESPONSE_HEADER_NB_VALUES + 1] = RESPONSE_HEADER_VALUES;
	size_t	i = 0;

	for (i = 0; values[i]; ++i) {
		_responsesMap.insert(FT::getPair((values[i]), ""));
	}
}

//**************************//
//		  RETURN CODE		//
//**************************//

bool	Response::checkContinue() {
	if (_clServer->isWaitingForContent()) {
		return (true);
	}
	return (false);
}

bool 	Response::checkProtocol() {
	return (false);
}
bool 	Response::checkFileCreation() {
	return (false);
}
bool 	Response::checkNoContent() {
	return (false);
}
bool 	Response::checkMovedPermanently() {
	return (false);
}
bool 	Response::checkSeeOther() {
	return (false);
}
bool 	Response::checkNotModified() {
	return (false);
}
bool	Response::checkPermanentRedirect() {
	if (_clServer->getIsRedirect()) {
		return (true);
	}
	return (false);
}
bool	Response::checkBadRequest() {
	// vérifier si la method est acceptee ou pas
	std::string request = METHOD;

	if (_clRequest->getRequestConfig().isInvalidHeader())
		return (true);

	// Post Chunked invalid content
	if (_clRequest->getRequestConfig().isInvalidContent())
		return (true);

	if (!request.compare("GET")
	|| !request.compare("DELETE")
	|| !request.compare("POST")) {
		return (false);
	}
	return (true);
}
bool	Response::checkUnauthorized() {
	return (false);
}
bool	Response::checkForbidden() {
	return (false);
}
bool	Response::checkPage() {
	std::string request = _filePath;
	if (!METHOD.compare("DELETE"))
		return (false);
	else if (!METHOD.compare("POST"))
		return (false);

	if (!_clRequest->requestLocationIsValid())
		return (true);
	if (!isFile(request)) {
		if (_useAutoIndex && isDir(_filePath)) {
			return (false);
		}
		else {
			return (true);
		}
	}
	return (false);
}
bool 	Response::checkMethod() {
	t_locationNode nodeAcceptMethod = NEW_LOCATIONS(_clServer).getFirstLocationALlowMethod(_locationPath, METHOD);

	if (IS_VALID_LOCATION(nodeAcceptMethod)) {
		return (false);
	} else {
		return (true);
	}
	return (false);
}
bool 	Response::chekNotAcceptable() {
	// check si la réponse convient à Accept-Charset et Accept-Language.

	std::string lang = _clRequest->getConfig()["Accept-Language"];
	std::string supp_lang[] = LANGUAGES;

	if (lang.empty() || !lang.compare("*"))
		return (false);
	else {
		for (size_t i = 0; i < NB_LANGUAGES; ++i) {
			if (!supp_lang[i].compare(lang)
			|| contain(supp_lang[i], lang))
				return (false);
		}
	}
	return (true);
}
bool 	Response::checkGone() {
	// check si la ressource n'est plus disponible sur le serveur et que ça sera définitif

	return (false);
}
bool 	Response::checkLength() {
	// check si content-length est là ou pas

	return (false);
}
bool 	Response::checkPayload() {
	// check si la taille de la requete est superieure au limite définies par le serveur

	if (METHOD == "POST") {
		if (PAYLOAD <= 0)
			return (false);
		if ((long long)RequestConfig().getCPOriginalSize() >= PAYLOAD
			||  atoll(_clRequest->getRequestConfig()["Content-Length"].c_str()) > (long long)PAYLOAD) {
			return (true);
		}
	}
	return (false);
}
bool 	Response::checkURITooLong() {
	// check la taille de l'uri et si c'est trop long, return
	if (PAGE.length() > URI_LENGTH_MAX)
		return (true);
	return (false);
}
bool 	Response::checkMediaType() {
	// check si le media est supporté (text/plain, media, jpeg) etc ..

	return (false);
}
bool 	Response::checkUnprocessableEntity() {
	// check si tout est bon mais que le code renvoie une erreur lors du parcours requête à réponse

	return (false);
}
bool 	Response::checkUpgradeRequired() {
	return (false);
}
bool 	Response::checkTooManyRequests() {
	if (_clientRateLimit.first) {
		return (true);
	}
	return (false);
}
bool 	Response::checkInternalServerError() {
	// fourre tout quand y'a un soucis;

	return (false);
}
bool 	Response::checkNotImplemented() {
	// check si méthode à été implémentée (Get & Head obligé)

	return (false);
}
bool 	Response::checkServiceUnavailable() {
	// check si serveur est prêt à traiter la requête

	return (false);
}
bool 	Response::checkHTTPVersion() {
	// check si version http est supportée;

	if (_clRequest->getConfig()["Protocol"].compare("HTTP/1.1"))
		return (true);
	else
		return (false);
}
bool 	Response::checkInsufficientStorage() {
	return (false);
}
