/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/22 15:19:10 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/18 21:44:02 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <iostream>
# include <stdexcept>
# include <sys/time.h>

# include "../../Server.hpp"
# include "../../Request/Request.hpp"
# include "../../Locations/Locations.hpp"
# include "../../../utils/utils.hpp"
# include "../HtmlChecker/HTMLChecker.hpp"
# include "../CGI/CGI.hpp"
# include "../Methods/Methods.hpp"
# include "../../Extensions/Extensions.hpp"

# define LANGUAGES { "fr-FR", "fr", "en-US", "en" }
# define NB_LANGUAGES 4

# define URI_LENGTH_MAX 64

# define METHOD _clRequest->getConfig()["Method"]
# define PAGE _clRequest->getRequestConfig()["Page"]

# define M_GET 0
# define M_POST 1
# define M_DELETE 2
# define M_ERROR 3

# define METHODS_INDEX(method) !method.compare("GET") ? 0 : !method.compare("POST") ? 1 : !method.compare("DELETE") ? 2 : 3
# define CODE_OK(code) (code >= 200 && code < 300) ? true : false
# define CODE_404(code) (code == 404) ? true : false

# define USE_CGI _clServer->useCGI()
# define USE_AUTOINDEX _clServer->getAutoIndex()
# define ERROR_PAGE(node) node.data->getErrorPages()
# define PAYLOAD _clServer->getClientMaxBodySize()

# define CODE_REDIRECT 308

# define RESPONSE_HEADER_VALUES {	"METHOD", "ALLOW", "CONTENT-LANGUAGE", \
									"CONTENT-LENGTH", "CONTENT-LOCATION", \
									"CONTENT-TYPE", "DATE", "LAST-MODIFIED", \
									"LOCATION", "RETRY-AFTER", "SERVER", \
									"TRANSFER-ENCODING", "WWW-AUTHENTICATE", \
									"CONNECTION" "ACCEPT-LANGUAGE" \
								}
# define RESPONSE_HEADER_NB_VALUES 14

/*
	- Allow
	- Date
	- Content *
	- Last Modified
	- Location
	- retry-After
	- server
	- transfer-Encoding
	- www-Authenticate
*/

class Server;
class Methods;

class Response {
protected:

	Request														*_clRequest;
	Server														*_clServer;
	Server														*_clRootServer;
	HtmlChecker													*_htmlChecker;

	Methods														*_clMethods;

	t_locationNode												_locationPath;

	std::string													_filePath;
	bool														_isFile;
	bool														_useAutoIndex;

	//	rate Limit
	std::pair<bool, t_rate_limit>								_clientRateLimit;

	int															_code;

	// Response Header
	std::map<std::string, std::string>							_responsesMap;
	std::map<size_t, std::string>								_errors;
	std::pair<std::map<std::string, std::string>, std::string>	_cgiResult;
	t_methodResponse											_methodMap;

private:
	void 			DoResponse();

	// Response HEADER
	std::string		ResponseHeaderManager();
	std::string		SetHeaderValues();

	// Response CONTENT
	std::string		ResponseContentManager();
	std::string		ResponseContentGet();
	std::string		ResponseContentPost();
	std::string		ResponseContentDelete();

	// ERRORS Pages
	std::string		generateErrorPage();
	std::string 	getHTMLErrorPage(std::string locErrorPath);
	std::string 	generateErrorContent();

public:
	Response (Request *request, std::string const &file,
		std::pair<bool, t_rate_limit> const &rateLimit,
		Server *locServer,
		Server *rootServer);
	Response (Response const &cpy);
	Response &operator=(Response const &cpy);
	virtual ~Response ();

	HtmlChecker	*getHTMLChecker() {
		return (_htmlChecker);
	}
	Request			*getRequest() {
		return (_clRequest);
	}
	Server			*getServer() {
		return (_clServer);
	}

	// Core Response

	std::string	ProcessResponse();

	// Utils

	std::string getAutoIndexPath(std::string const &path) {
		try
		{
			std::string ret = path.substr(0, path.rfind("/"));
			return (ret);
		}
		catch (std::exception &e)
		{
			std::cerr << RED << "GetAutoIndex ERROR : " << e.what() << '\n';
			return ("");
		}
		return ("");
	}
	std::string getFilePath() {
		return (_filePath);
	}
	bool		isMethod(std::string const &method) {
		return (!method.compare(_clRequest->getConfig()["Method"]));
	}
	void		updateCGIResult(std::pair<std::map<std::string, std::string>, std::string> map) {
		_cgiResult = map;
	}
	// Set Header

	void 		setErrorValues();
	void		setValues();
	void 		resetValues();

	void 		setMethod();
	void 		setAllow();
	void 		setContentLanguage();
	void 		setContentLength();
	void 		setContentLocation();
	void 		setContentType(std::string const &force);
	void 		setDate();
	void 		setLastModified();
	void 		setLocation();
	void 		setRetryAfter();
	void 		setServerName();
	void 		setTransferEncoding();
	void 		setWWAuthenticate();
	void		setConnection();
	void 		setAcceptLanguage();

	// Check Header

	void 		checkRequestValues();
	bool		checkContinue();
	bool 		checkProtocol();
	bool		checkPage();
	bool 		checkFileCreation();
	bool 		checkNoContent();
	bool 		checkMovedPermanently();
	bool 		checkSeeOther();
	bool 		checkNotModified();
	bool		checkBadRequest();
	bool		checkPermanentRedirect();
	bool		checkUnauthorized();
	bool		checkForbidden();
	bool	 	checkMethod();
	bool	 	chekNotAcceptable();
	bool	 	checkGone();
	bool	 	checkLength();
	bool	 	checkPayload();
	bool	 	checkURITooLong();
	bool	 	checkMediaType();
	bool	 	checkUnprocessableEntity();
	bool	 	checkUpgradeRequired();
	bool	 	checkTooManyRequests();
	bool	 	checkInternalServerError();
	bool	 	checkNotImplemented();
	bool	 	checkServiceUnavailable();
	bool	 	checkHTTPVersion();
	bool	 	checkInsufficientStorage();
};

#endif
