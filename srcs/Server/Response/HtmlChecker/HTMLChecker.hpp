/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTMLChecker.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/23 14:20:49 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/05/29 12:53:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HtmlChecker_HPP
# define HtmlChecker_HPP

# include <iostream>
# include <ctime>

# include "../../../utils/utils.hpp"

// C Includes
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <unistd.h>
# include <errno.h>
# include <string.h>

# define HTML_READ_BUFFER 8192
# define DEFAULT_FOLDER "./webSite"

class HtmlChecker {
private:
	std::string 	_file;
	int				_fileFD;

	std::string 	_default_file;

	void openFile();
	void closeFile();
	std::string readFile();
	std::string getFileExtension();

public:
	HtmlChecker (void);
	HtmlChecker (std::string const &file, std::string const &default_file);
	HtmlChecker (HtmlChecker const &cpy);
	HtmlChecker &operator=(HtmlChecker const &cpy);
	virtual ~HtmlChecker ();

	void 		getCorrectFilePath();
	bool 		fileIsValid();

	size_t 		getFileLength();
	size_t 		readFileLength();
	std::string getContentFile();
	std::string getContentLocation();
	std::string getContentType();
	struct tm	*getLastModifiedFile();
	std::string	getFileLocation();

	class HTML_FILE_EXCEPTION : public std::exception {
		virtual const char *what() const throw();
	};
	class HTML_READ_EXCEPTION : public std::exception {
		virtual const char *what() const throw();
	};
	class HTML_OPEN_EXCEPTION : public std::exception {
		virtual const char *what() const throw();
	};
	class HTML_STAT_EXCEPTION : public std::exception {
		virtual const char *what() const throw();
	};

	void 		printClass() {
		std::cout << "File : " << _file << '\n';
		std::cout << "FileFD : " << _fileFD << '\n';
		std::cout << "DefaultFile : " << _default_file << '\n';
	}
	void 		printError(std::string const &error) {
		std::cerr << RED << "\t- HtmlChecker : "<< error << RESET << '\n';
	}
};

#endif
