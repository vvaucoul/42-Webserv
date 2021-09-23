/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServersConfig.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-faou <mle-faou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 10:50:55 by mle-faou          #+#    #+#             */
/*   Updated: 2021/09/19 17:22:42 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERSCONFIG_HPP
#define SERVERSCONFIG_HPP

#include "../Server/Server.hpp"

#include <iostream>
#include <fstream>

#define ENDING ".wscfg"

class ServersConfig {
private:
	std::vector<Server*> _servers;

public:
	ServersConfig();
	ServersConfig(int argc, char **argv);
	ServersConfig(ServersConfig const &cpy);
	ServersConfig&operator=(ServersConfig const &cpy);
	virtual ~ServersConfig();

	std::vector<Server*> getServers(void) const {return (_servers);};
	void freeServer();

	int checkVar(Server* config, std::vector<std::string>& line, int line_number);
	bool isNewServerLine(std::string line, int line_number);
	bool isNewLocationLine(std::string line, int line_number, std::string* new_name);
	bool isNewExtensionLine(std::string line, int line_number, std::string* new_name);
	bool isEndLine(std::string line, int line_number);
	bool isEmptyLine(std::string line, int line_number);
	int parseVar(Server* to_update, std::string line, int line_number);
	int parseNew(std::ifstream& configFile, int *line_number, Server* to_config);
	std::vector<Server*> parseConfigFile(std::ifstream& configFile);
	bool isEndingWithRightExtension(const char *str);

	void print(void);

	class InvalidArgumentsException : public std::exception {
		virtual const char *what() const throw() {return ("InvalidArgumentsException");};
	};
	class InvalidFileException : public std::exception {
		virtual const char *what() const throw() {return ("InvalidFileException");};
	};
	class UnreadableFileException : public std::exception {
		virtual const char *what() const throw() {return ("UnreadableFileException");};
	};
	class ParseFileException : public std::exception {
		virtual const char *what() const throw() {return ("ParseFileException");};
	};
	class ParseNewException : public std::exception {
		virtual const char *what() const throw() {return ("ParseNewException");};
	};
	class ParseVarException : public std::exception {
		virtual const char *what() const throw() {return ("ParseVarException");};
	};
	class InvalidLineException : public std::exception {
		virtual const char *what() const throw() {return ("InvalidLineException");};
	};
	class UndefinedLineException : public std::exception {
		virtual const char *what() const throw() {return ("UndefinedLineException");};
	};
	class VarAlreadySetException : public std::exception {
		virtual const char *what() const throw() {return ("VarAlreadySetException");};
	};

};

#endif
