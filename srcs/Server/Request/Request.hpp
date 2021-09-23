/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/22 12:47:45 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 12:55:16 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <iostream>

# include "RequestConfig.hpp"
# include "../../utils/utils.hpp"

class Request {
private:

	RequestConfig						_config;
	std::map<std::string, std::string>	_requestConfig;
	std::string							_root;
	t_locationNode						_location;
	t_extensionNode						_extention;

	bool								_usePostRequest;
	bool								_useDeleteRequest;

	bool								_requestLocationIsValid;
	bool								_searchingFile;
	std::pair<t_extensionNode, bool>	_requestIsInExtension;
	std::map<std::string, size_t>		_methods;

	bool		checkProtocol();
	void 		checkPage();
	void		checkFile();
	void		checkExtension();

public:
	Request (void);
	Request (RequestConfig const &config, t_locationNode const &location, t_extensionNode const &extention, std::string const &root);
	Request (Request const &cpy);
	Request &operator=(Request const &cpy);
	virtual ~Request (void);

	std::map<std::string, std::string> getConfig() {return (_requestConfig);}
	RequestConfig getRequestConfig() {return (_config);}

	bool processRequest();
	void printRequest(std::string const &request);
	std::map<std::string, size_t> initMethods();

	std::string	getRequestValue(std::string const &value);
	void updateRequestConfig();

	bool		requestLocationIsValid() { return (_requestLocationIsValid); }
	std::pair<t_extensionNode, bool> requestIsInExtention() { return (_requestIsInExtension); }
	void 		updateRequestLocationValidation(bool const &value) { _requestLocationIsValid = value; }
	// request Post

	void 		processPostRequest(std::string const &rcvinfo);
	bool		isPostRequest();
	bool		isDeleteRequest();
	bool		checkMethod();
	bool		isSearchingFile() { return (_searchingFile); }

	// debug

	std::map<std::string, std::string> debugGetRequest();


public:
	std::string &operator[](std::string const &key);

};

#endif
