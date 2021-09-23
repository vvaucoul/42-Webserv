/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServersConfig.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-faou <mle-faou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/20 10:44:01 by mle-faou          #+#    #+#             */
/*   Updated: 2021/09/20 14:43:40 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServersConfig.hpp"

ServersConfig::ServersConfig() {

}

ServersConfig::ServersConfig(ServersConfig const &cpy) {
	*this = cpy;
}

ServersConfig &ServersConfig::operator=(ServersConfig const &cpy) {
	_servers = cpy._servers;
	return (*this);
}

ServersConfig::~ServersConfig() {
}

void ServersConfig::freeServer() {
	for (std::vector<Server *>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
		delete (*it);
	}
}

bool ServersConfig::isEndingWithRightExtension(const char *str)
{
	std::string to_compare = str;
	std::string ending = ENDING;

	if (to_compare.length() >= ending.length())
		return (0 == to_compare.compare(to_compare.length() - ending.length(), ending.length(), ending));
	else
		return (false);
}

ServersConfig::ServersConfig(int argc, char **argv)
{
	std::string to_open = "";
	if (argc == 1)
		to_open = "tests/config_default.wscfg";
	else if (argc == 2)
	{
		if (!isEndingWithRightExtension(argv[1]))
		{
			std::cerr << "Invalid file.\nFile Type : configFile.wscfg" << '\n';
			freeServer();
			throw ServersConfig::InvalidFileException();
		}
		to_open = argv[1];
	}
	else
	{
		std::cerr << "Invalid arguments.\nUsage : ./webserv (optionnal)-configFile-" << '\n';
		freeServer();
		throw ServersConfig::InvalidArgumentsException();
	}

	//std::cout << to_open << std::endl;
	std::ifstream configFile(to_open.c_str());

	if (!(configFile.is_open()))
	{
		std::cerr << "Unreadable file.\nError : Failed to open config file" << '\n';
		freeServer();
		throw ServersConfig::UnreadableFileException();
	}

	std::string line;
	int line_number = 0;

	while (getline(configFile,line))
	{
		line_number++;

		// std::cout << "a.line[" << line_number << "] : '" << line << "'" << std::endl;

		if (isEmptyLine(line, line_number))
			continue ;
		else if (isNewServerLine(line, line_number))
		{
			_servers.push_back(new Server(NULL));
			_servers[_servers.size() - 1]->setIsLocation(false);
			if (parseNew(configFile, &line_number, _servers[_servers.size() - 1]))
			{
				freeServer();
				throw ServersConfig::ParseFileException();
			}
		}
		else
		{
			std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : Unexpected line outside of server declaration" << std::endl;
			freeServer();
			throw ServersConfig::UndefinedLineException();
		}
	}

	configFile.close();
}

bool ServersConfig::isNewServerLine(std::string line, int line_number)
{
	int i = 0;

	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	if (!line[i] || line[i] == '#')
		return (false);
	if (line.compare(i, 6, "server") != 0)
		return (false);
	i += 6;
	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	if (line[i] != '{')
	{
		std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : unexpected chararacter after 'server'" << std::endl;
		freeServer();
		throw ServersConfig::InvalidLineException();
	}
	i++;
	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	if (line[i] && line[i] != '#')
	{
		std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : unexpected chararacter after '{'" << std::endl;
		freeServer();
		throw ServersConfig::InvalidLineException();
	}
	return (true);
}

bool ServersConfig::isNewLocationLine(std::string line, int line_number, std::string* new_name)
{
	int i = 0;
	std::string tmp_location_name = "";

	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	if (!line[i] || line[i] == '#')
		return (false);
	if (line.compare(i, 8, "location") != 0)
		return (false);
	i += 8;
	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	while (line[i] >= 32 && line[i] <= 126 && line[i] != '#' && line[i] != ' ' && line[i] != '\t')
	{
		tmp_location_name.push_back(line[i]);
		i++;
	}
	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	if (line[i] != '{')
	{
		std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : unexpected char after 'location'" << std::endl;
		freeServer();
		throw ServersConfig::InvalidLineException();
	}
	i++;
	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	if (line[i] && line[i] != '#')
	{
		std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : unexpected char after '{'" << std::endl;
		freeServer();
		throw ServersConfig::InvalidLineException();
	}
	*new_name = fixSlash('/' + tmp_location_name + '/');
	if (nbChar(*new_name, '/') > 2)
	{
		std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : location accept only one directory" << std::endl;
		freeServer();
		throw ServersConfig::InvalidLineException();
	}
	return (true);
}

bool ServersConfig::isNewExtensionLine(std::string line, int line_number, std::string* new_name)
{
	int i = 0;
	std::string new_extension_name = "";

	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	if (!line[i] || line[i] == '#')
		return (false);
	// if (line.compare(i, 12, "extension *.") != 0)
	// {
	// 	std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : extension is declared like 'extension *.[your extension]'" << std::endl;
	// 	freeServer();
	// 	throw ServersConfig::InvalidLineException();
	// }
	// i += 12;
	if (line.compare(i, 9, "extension") != 0)
		return (false);
	i += 9;
	if (line.compare(i, 3, " *.") != 0)
	{
		std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : extension is declared like 'extension *.[your extension]'" << std::endl;
		freeServer();
		throw ServersConfig::InvalidLineException();
	}
	i += 3;
	while (line[i] >= 32 && line[i] <= 126 && line[i] != '#' && line[i] != ' ' && line[i] != '\t')
	{
		new_extension_name.push_back(line[i]);
		i++;
	}
	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	if (line[i] != '{')
	{
		std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : unexpected char after 'extension'" << std::endl;
		freeServer();
		throw ServersConfig::InvalidLineException();
	}
	i++;
	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	if (line[i] && line[i] != '#')
	{
		std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : unexpected char after '{'" << std::endl;
		freeServer();
		throw ServersConfig::InvalidLineException();
	}
	*new_name = new_extension_name;
	return (true);
}

bool ServersConfig::isEndLine(std::string line, int line_number)
{
	int i = 0;

	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	if (!line[i] || line[i] == '#')
		return (false);
	if (line[i] != '}')
		return (false);
	i++;
	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	if (line[i] && line[i] != '#')
	{
		std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : unexpected char after '}'" << std::endl;
		freeServer();
		throw ServersConfig::InvalidLineException();
	}
	return (true);
}

bool ServersConfig::isEmptyLine(std::string line, int line_number)
{
	int i = 0;

	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	if (!line[i] || line[i] == '#')
		return (true);
	if (line[i] != ';')
		return (false);
	i++;
	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	if (line[i] && line[i] != '#')
	{
		std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : unexpected char after ';'" << std::endl;
		freeServer();
		throw ServersConfig::InvalidLineException();
	}
	return (true);
}

int ServersConfig::parseVar(Server* to_update, std::string line, int line_number)
{
	int i = 0;
	int quotes;
	std::vector<std::string> words;

	while (line[i] && line[i] != '#' && line[i] != ';')
	{
		while (line[i] && (line[i] == ' ' || line[i] == '\t'))
			i++;
		words.push_back("");
		quotes = 0;
		while (line[i] >= 32 && line[i] <= 126 && (!quotes && (line[i] != ';' && line[i] != '#' && line[i] != ' ' && line[i] != '\t')))
		{
			if (line[i] == '\'' && quotes == 0)
				quotes = 1;
			else if (line[i] == '\'' && quotes == 1)
				quotes = 0;
			else if (line[i] == '"' && quotes == 0)
				quotes = 2;
			else if (line[i] == '"' && quotes == 2)
				quotes = 0;
			else
				words[words.size() - 1].push_back(line[i]);
			i++;
		}
		while (line[i] && (line[i] == ' ' || line[i] == '\t'))
			i++;
	}
	if (line[i] != ';')
	{
		std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : missing ';'" << std::endl;
		freeServer();
		throw ServersConfig::ParseVarException();
	}
	i++;
	while (line[i] && (line[i] == ' ' || line[i] == '\t'))
		i++;
	if (line[i] && line[i] != '#')
	{
		std::cout << line[i] << std::endl;
		std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : unexpected char after ';'" << std::endl;
		freeServer();
		throw ServersConfig::ParseVarException();
	}
	if (checkVar(to_update, words, line_number)) {
		//delete (_servers[0]);
		freeServer();
		throw ServersConfig::ParseVarException();
	}
	return (0);
}

int ServersConfig::parseNew(std::ifstream& configFile, int *line_number, Server* to_config)
{
	std::string new_name;
	std::string line;

	while (getline(configFile, line))
	{
		(*line_number)++;
		// std::cout << "b.line[" << *line_number << "] : '" << line << "'" << std::endl;
		new_name = "";
		if (isEndLine(line, *line_number))
			return (0);
		if (isEmptyLine(line, *line_number))
			continue ;
		if (isNewLocationLine(line, *line_number, &new_name))
		{
			if (to_config->getIsExtension())
			{
				std::cerr << "Invalid line in configuration file.\nline[" << *line_number << "] : cannot declare a new location in an extension" << std::endl;
				freeServer();
				throw ServersConfig::InvalidLineException();
			}
			if (to_config->hasLocationNamed(new_name))
			{
				std::cerr << "Invalid line in configuration file.\nline[" << *line_number << "] : this location is already used" << std::endl;
				freeServer();
				throw ServersConfig::InvalidLineException();
			}
			to_config->addLocation();
			to_config->getLocationN(to_config->getLocationsSize() - 1)->setIsLocation(true);
			to_config->getLocationN(to_config->getLocationsSize() - 1)->setLocationName(new_name);
			if (parseNew(configFile, line_number, to_config->getLocationN(to_config->getLocationsSize() - 1)))
			{
				freeServer();
				throw ServersConfig::ParseNewException();
			}
		}
		else if (isNewExtensionLine(line, *line_number, &new_name))
		{
			if (to_config->getIsExtension())
			{
				std::cerr << "Invalid line in configuration file.\nline[" << *line_number << "] : cannot declare a new extension in an extension" << std::endl;
				freeServer();
				throw ServersConfig::InvalidLineException();
			}
			if (to_config->hasExtensionNamed(new_name))
			{
				std::cerr << "Invalid line in configuration file.\nline[" << *line_number << "] : this extension is already used" << std::endl;
				freeServer();
				throw ServersConfig::InvalidLineException();
			}
			to_config->addExtension();
			to_config->getExtensionN(to_config->getExtensionsSize() - 1)->setIsExtension(true);
			to_config->getExtensionN(to_config->getExtensionsSize() - 1)->setExtensionName(new_name);
			if (parseNew(configFile, line_number, to_config->getExtensionN(to_config->getExtensionsSize() - 1)))
			{
				freeServer();
				throw ServersConfig::ParseNewException();
			}
		}
		else
			parseVar(to_config, line, *line_number);
	}
	std::cerr << "Invalid line in configuration file.\nline[" << *line_number + 1 << "] : unexpected EOF, oppenned brackets" << std::endl;
	freeServer();
	throw ServersConfig::ParseNewException();
	return (1);
}

void ServersConfig::print(void)
{
	for (int i = 0; i < (int)_servers.size(); ++i)
	{
		std::cout << "Server[" << i << "] :" << std::endl;
		_servers[i]->printServerConfig(1);
	}
}
