/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/25 14:43:32 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/18 20:38:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef METHODS_HPP
# define METHODS_HPP

# include <iostream>
# include <vector>

# include <unistd.h>
# include <sys/stat.h>
# include <sys/types.h>

# include "../../../utils/utils.hpp"
# include "../Response/Response.hpp"
# include "../HtmlChecker/HTMLChecker.hpp"

# define DEFAULT_FOLDER "./webSite"
# define METHODS_ALLOWED "GET, POST, DELETE"

# define POST_FORM_DATA_FILENAME "filename"
# define POST_FORM_DATA_CONTENT "Content"

class Response;

class Methods {
private:
	std::string 						_currentMethod;
	HtmlChecker 						*_htmlChecker;
	std::map<std::string, std::string> 	_responsesMap;
	Response							*_response;

	bool								_deleteRessourceDeleted;
	Server								*_clServer;
	Server								*_clRootServer;

public:
	Methods (std::string const &method,
		HtmlChecker const &htmlChecker,
		std::map<std::string, std::string> const &responsesMap,
		Response *response,
		Server *srv, Server *rootServer);
	Methods (Methods const &cpy);
	Methods &operator=(Methods const &cpy);
	virtual ~Methods ();

	t_methodResponse processMethods();

	t_methodResponse Get();
	t_methodResponse Post();
	t_methodResponse Delete();

	// methods utils
	std::map<std::string, std::string>	ConvertX_WWW_FORM_URLENCODED(std::string const &content);
	std::vector<t_Post_Boundary>	 	CreateMultipartBoundary(std::string const &content, std::string const &boundary);
	t_methodResponse					PostFormData(t_Post &post);
	t_methodResponse					PostURLEncoded(t_Post &post);
	int									writePostFile(std::string const &fileContent, std::string const &fileName, size_t const index);
	t_Post								InitPost();


	std::string 	DeleteGetCode(std::string const &file, size_t const code);
	std::string		DeleteGetCode200(std::string const &file);
	std::string 	DeleteGetCode204(std::string const &file);

	// utils
	std::string		getCorrectFilePath(std::string const &str);


	class PostWriteFileError : public std::exception {
		virtual const char *what() const throw() {return ("PostWriteFileError");};
	};
};

#endif
