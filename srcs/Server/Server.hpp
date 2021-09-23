/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/16 17:42:54 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/19 18:58:20 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# pragma once

# include <string>
# include <iostream>
# include <algorithm>
# include <vector>
# include <map>

# include <stdio.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/select.h>
# include <arpa/inet.h>
# include <stdlib.h>
# include <netinet/in.h>
# include <unistd.h>
# include <fcntl.h>
# include <errno.h>
# include <string.h>
# include <sys/time.h>

# include "../utils/utils.hpp"
# include "Request/Request.hpp"
# include "Request/ChunkProcess/ChunkProcess.hpp"
# include "Response/Response/Response.hpp"
# include "Response/AutoIndex/AutoIndex.hpp"
# include "Locations/Locations.hpp"
# include "Locations/NewLocations.hpp"
# include "Response/PostMethod/PostMethod.hpp"
# include "RateLimit/RateLimit.hpp"
# include "Extensions/Extensions.hpp"

# define MAX_PENDING_CONNECTION 100

# define SOCKET_IN struct sockaddr_in
# define SOCKET struct sockaddr

# define RCV_SIZE 4096

class Server
{

//config part
private:
	std::vector<Server*>		_extensions;
	std::string					_extension_name;
	bool						_is_extension;

	std::vector<Server*>		_locations;
	Server						*_parent_location;
	std::string					_location_name;
	bool						_is_location;
	bool						_is_location_dir;

	std::string					_ip; // host -> std::string
	std::string					_name; // server_name -> std::string
	std::string					_html_path; // index -> std::string
	std::string					_html_error_path; // error_page -> std::string
	std::map<int, std::string>	_error_pages;
	std::vector<std::string>	_method; // allow_methods -> std::vector<std::string>
	std::string					_redirect_link;
	std::string					_root; // root -> std::string
	std::string					_cgi_path; // cgi_path -> std::string
	std::vector<std::string>	_cgi_extension; // cgi_extension -> std::vector<std::string>
	std::string					_accept_files_path;
	int							_port; // port -> int
	bool						_auto_index; // autoindex on/off -> bool
	bool						_is_redirect;
	int							_client_max_body_size; // client_body_buffer_size -> int

	std::vector<std::string>	_set_vars; // every variable already setted

//internal part
private:
	SOCKET_IN					_address;
	fd_set						_fdSet; // Special value for FD
	int							_fdSize;
	int							_maxFD;

	int							_addrlen;

	std::vector<int>			_fdList;
	int							_srvFD;

	// RateLimit Sockets
	std::map<int, RateLimit>	_rateLimits;

	// Request & Responses
	Request						*_clRequest;	// classe request

	// Post Boundary (Need to save all content)
	bool						_isWaitingForContent;

	// todo, map de request, id & msg
	// ex : connection post envoie 2 header et doit garder les 2
	std::map<int, std::string>	_requests;


	//std::vector<t_srv_request> _request;

public:
	Server();
	Server(Server *parent_location);
	Server(std::string const &ip, int const &port);
	Server(const Server &srv);
	Server &operator=(const Server &srv);
	virtual ~Server ();

	// Config

	void addExtension(void) {_extensions.push_back(new Server(this));}
	Server* getExtensionN(int n) const {return (_extensions[n]);}
	std::vector<Server*> getExtensions() const {return (_extensions);}
	int getExtensionsSize(void) const {return (_extensions.size());}
	bool hasExtensions(void) const {return (!_extensions.empty());}
	bool hasExtensionNamed(const std::string extension_name);
	Server* getFileExtensionData(std::string fileName);

	void addLocation(void) {_locations.push_back(new Server(this));}
	Server* getLocationN(int n) const {return (_locations[n]);}
	std::vector<Server*> getLocations() const {return (_locations);}
	int getLocationsSize(void) const {return (_locations.size());}
	Server *getParentLocation(void) const {return (_parent_location);}
	bool hasLocations(void) const {return (!_locations.empty());}
	bool hasLocationNamed(const std::string location_name);

	void setIsExtension(const bool is_extension) {_is_extension = is_extension;}
	void setExtensionName(const std::string extension_name) {_extension_name = extension_name;}
	void setIsLocation(const bool is_location) {_is_location = is_location;}
	void setLocationName(const std::string location_name) {_location_name = location_name;}
	void setIsLocationDir(const bool is_location_dir) {_is_location_dir = is_location_dir;}
	void setPort(const int port) {_port = port;}
	void setIP(const std::string ip) {_ip = ip;}
	void setName(const std::string name) {_name = name;}
	void setHTMLPath(const std::string html_path) {_html_path = html_path;}
	void setHTMLErrorPath(const std::string html_error_path) {_html_error_path = html_error_path;}
	void setMethod(const std::vector<std::string> method) {_method = method;}
	void setAutoIndex(const bool auto_index) {_auto_index = auto_index;}
	void setRoot(const std::string root) {_root = root;}
	void setCGIPath(const std::string cgi_path) {_cgi_path = cgi_path;}
	void setCGIExtension(const std::vector<std::string> cgi_extension) {_cgi_extension = cgi_extension;}
	void setClientMaxBodySize(const int client_max_body_size) {_client_max_body_size = client_max_body_size;}

	bool getIsExtension(void) const {return (_is_extension);}
	std::string getExtensionName(void) const {return (_extension_name);}
	bool getIsLocation(void) const {return (_is_location);}
	std::string getLocationName(void) const {return (_location_name);}
	bool getIsLocationDir(void) const {return (_is_location_dir);}
	int getPort(void) const {return (_port);}
	std::string getIP(void) const {return (_ip);}
	std::string getName(void) const {return (_name);}
	std::string getHTMLPath(void) const {return (_html_path);}
	std::string getHTMLErrorPath(void) const {return (_html_error_path);}
	std::vector<std::string> getMethod(void) const {return (_method);}
	bool getAutoIndex(void) const {return (_auto_index);}
	std::string getRoot(void) const {return (_root);}
	std::string getCGIPath(void) const {return (_cgi_path);}
	std::vector<std::string> getCGIExtension(void) const {return (_cgi_extension);}
	int getClientMaxBodySize(void) const {return (_client_max_body_size);}

	void setIsRedirect(const bool is_redirect) {_is_redirect = is_redirect;}
	bool getIsRedirect(void) const {return (_is_redirect);}
	void setRedirectLink(const std::string redirect_link) {_redirect_link = redirect_link;}
	std::string getRedirectLink(void) const {return (_redirect_link);}
	std::string getRedirectHTML(void) const {return ("<head>\r\n<meta http-equiv=\"Refresh\" content=\"0; URL=" + fixSlash("/" + getRedirectLink()) + "\"/>\r\n</head>\r\n");}

	void setAcceptFilesPath(const std::string accept_files_path) {_accept_files_path = accept_files_path;}
	std::string getAcceptFilesPath(void) const {return (_accept_files_path);}

	void setErrorPages(const std::map<int, std::string>	error_pages) {_error_pages = error_pages;}
	std::map<int, std::string> getErrorPages(void) const {return (_error_pages);}
	void addErrorPage(int key, std::string val) {FT::emplace(_error_pages, FT::getPair(key, val));}
	bool isErrorPageSet(int key) {return (_error_pages.find(key) != _error_pages.end());}

	bool useCGI(void) const {return (!_cgi_path.empty());}
	void printServerConfig(int tabs = 0);

	void addSettedVariable(const std::string to_add) {_set_vars.push_back(to_add);}
	bool isSet(const std::string to_add) {
		if (std::find(_set_vars.begin(), _set_vars.end(), to_add) != _set_vars.end())
			return (true);
		return (false);
	}

	// Functions

	int		initServer();
	int		initClients(void);
	void 	displayError(std::string const &prefix, std::string const &msg);
	int		sendServerContinue(int acceptedSocket, std::string const &firstRequest);

	int 	acceptSocket();

	int		receiveCorrectHeader(std::string const &str);

	int		receiveInfo(int acceptedSocket);
	int		sendInfo(int acceptedSocket);
	int 	closeServer();
	int 	closeAcceptedClient(int fd);
	int		cleanServer();

	int		Connection(int acceptedSocket);

	int		getFD() {
		return (_srvFD);
	}
	Request *getRequest() {
		return (_clRequest);
	}
	bool	isWaitingForContent() {
		return (_isWaitingForContent);
	}

	// Request Functions
	void		cleanSocketRequest(int acceptedSocket);
	void		AddSocketRequest(int acceptSocket, std::string buffer);
	std::string	getSocketRequest(int acceptSocket);

	// Recv Functions
	std::string readBuffer(char const *buffer);

	/*
	void addSRVRequest(size_t const &fd, std::string const &request);
	std::pair<int, t_srv_request> getRequestNewID(size_t const &fd);
	std::string getRequestString(size_t const &fd, size_t const &headerPos);
	std::string getLastRequest(size_t const &fd);
	std::vector<std::string> getTSRVRequests(size_t const &fd);
	t_srv_request getTSRVRequest(size_t const &fd);
	int	cleanRequest(size_t const &fd);
	*/
};

#endif
