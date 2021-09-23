/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/29 19:33:04 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 13:01:27 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
#define CGI_HPP

#include "../../Server.hpp"
#include <string.h>

// linux specific
#include <sys/types.h>
#include <sys/wait.h>

#include <errno.h>

class CGI {

private:
	void init();
	void process();
	void parse();

	void printEnv();

private:
	Request								*_request;
	Server								*_srv;
	Server								*_rootSrv;
	std::string							_filePath;
	t_Post 								*_postRequest;

	std::map<std::string, std::string>	_env;

	std::string							_result;
	int									_code;
	std::map<std::string, std::string>	_header;
	std::string							_content;

	HtmlChecker							*_checker;

public:
	CGI(Request *request, Server *srv, Server *rootSrv, std::string const &filePath, t_Post *postRequest);
	CGI (CGI const &cpy);
	CGI &operator=(CGI const &cpy);
	virtual ~CGI (void);

public:

	std::map<std::string, std::string> getHeader(void) const {
		return (_header);
	};
	std::string getResult(void) const {
		return (_result);
	};
	int getStatusCode(void) const {
		return (_code);
	};
	std::pair<std::map<std::string, std::string>, std::string>	getMapResult() {
		return (FT::getPair(_header, _content));
	}
	class CGIInitialisationException : public std::exception {
		virtual const char *what() const throw() {return ("CGIInitialisationException");};
	};
	class CGIContentException : public std::exception {
		virtual const char *what() const throw() {return ("CGIContentException");};
	};
	class CGIError500 : public std::exception {
		virtual const char *what() const throw() {return ("CGIError500");};
	};
};

#endif
