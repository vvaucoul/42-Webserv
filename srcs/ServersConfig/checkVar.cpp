/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checkVar.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-faou <mle-faou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/26 13:25:45 by mle-faou          #+#    #+#             */
/*   Updated: 2021/09/20 14:48:37 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServersConfig.hpp"

bool isNumber(std::string to_check, int line_number)
{
	for (int i = 0; i < (int)to_check.length(); ++i)
	{
		if (i == 0 && to_check[i] == '-') {
			std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : '" << to_check << "' can't be negative" << std::endl;
			return (false);
		}
		if (to_check[i] < '0' || to_check[i] > '9')
		{
			std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : '" << to_check << "' is not a number" << std::endl;
			return (false);
		}
	}
	return (true);
}

int checkIsSet(Server* to_config, std::string attribut, int line_number)
{
	if (to_config->isSet(attribut))
	{
		std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : '" << attribut << "' already set" << std::endl;
		return (1);
	}
	return (0);
}

int checkArgNumber(int size, int expected_arg_number, int line_number, std::string attribut)
{
	if (size != expected_arg_number)
	{
		std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : '" << attribut << "' type needs one value" << std::endl;
		return (1);
	}
	return (0);
}

int ServersConfig::checkVar(Server* to_config, std::vector<std::string>& line, int line_number)
{
	if (line[0] == "port")
	{
		if (checkIsSet(to_config, line[0], line_number))
			return (1);
		if (checkArgNumber(line.size(), 2, line_number, line[0]))
			return (1);
		if (!isNumber(line[1], line_number))
			return (1);
		to_config->setPort(atoi(line[1].c_str()));
		if (to_config->getPort() < 0 || to_config->getPort() > 65535) {
			std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : '" << line[1] << "' is out of port range" << std::endl;
			return (1);
		}
		to_config->addSettedVariable(line[0]);
	}
	else if (line[0] == "host")
	{
		if (checkIsSet(to_config, line[0], line_number))
			return (1);
		if (checkArgNumber(line.size(), 2, line_number, line[0]))
			return (1);
		to_config->setIP(line[1]);
		to_config->addSettedVariable(line[0]);
	}
	else if (line[0] == "server_name")
	{
		if (checkIsSet(to_config, line[0], line_number))
			return (1);
		if (checkArgNumber(line.size(), 2, line_number, line[0]))
			return (1);
		to_config->setName(line[1]);
		to_config->addSettedVariable(line[0]);
	}
	else if (line[0] == "accept_files")
	{
		if (checkIsSet(to_config, line[0], line_number))
			return (1);
		if (checkArgNumber(line.size(), 2, line_number, line[0]))
			return (1);
		Server *actual_server = to_config;
		while(actual_server)
		{
			std::vector<std::string> tmp = actual_server->getMethod();
			if ((std::find(tmp.begin(), tmp.end(), "post") != tmp.end())
				|| (std::find(tmp.begin(), tmp.end(), "Post") != tmp.end())
				|| (std::find(tmp.begin(), tmp.end(), "POST") != tmp.end()))
			{
				to_config->setAcceptFilesPath(line[1]);
				to_config->addSettedVariable(line[0]);
				return (0);
			}
			actual_server = actual_server->getParentLocation();
		}
		std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : '" << line[0] << "' cannot be used because method post is not allowed" << std::endl;
		return (1);
	}
	else if (line[0] == "index")
	{
		if (checkIsSet(to_config, line[0], line_number))
			return (1);
		if (checkArgNumber(line.size(), 2, line_number, line[0]))
			return (1);
		if (line[1][0] == '.' && line[1][1] == '/')
			to_config->setHTMLPath(std::string(line[1] , 2));
		else
			to_config->setHTMLPath(line[1]);
		to_config->addSettedVariable(line[0]);
	}
	else if (line[0] == "error_page")
	{
		if (checkArgNumber(line.size(), 3, line_number, line[0]))
			return (1);
		int key_to_add = atoi(line[1].c_str());
		if (!(key_to_add >= 100 && key_to_add <= 511))
		{
			std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : Unknown Error" << std::endl;
			return (1);
		}
		if (to_config->isErrorPageSet(key_to_add))
		{
			std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : Error " << key_to_add << " is already set" << std::endl;
			return (1);
		}
		std::string path = fixSlash(NEW_LOCATIONS(to_config).getLocationPathFromServer(to_config) + "/" + line[2]);
		if (!isFile(path))
		{
			std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : Error page '" << line[2] << "' does not exist" << std::endl;
			return (1);
		}
		to_config->addErrorPage(key_to_add, path);
	}
	else if (line[0] == "allow_methods")
	{
		if (checkIsSet(to_config, line[0], line_number))
			return (1);
		if (line.size() < 2)
		{
			std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : '" << line[0] << "' type needs one value" << std::endl;
			return (1);
		}
		std::vector<std::string> tmp = line;
		tmp.erase(tmp.begin());
		std::string legal_methods_tmp[] = { "GET", "POST", "DELETE", "PUT", "get", "post", "delete", "put", "Get", "Post", "Delete", "Put" };
		std::vector<std::string> legal_methods(legal_methods_tmp, legal_methods_tmp + 12); // update is line above is changed
		for (int i = 0; i < (int)tmp.size(); ++i)
		{
			if (std::find(legal_methods.begin(), legal_methods.end(), tmp[i]) == legal_methods.end())
			{
				std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : unknown method '" << tmp[i] << "'" << std::endl;
				return (1);
			}
		}
		to_config->setMethod(tmp);
		to_config->addSettedVariable(line[0]);
	}
	else if (line[0] == "autoindex")
	{
		if (checkIsSet(to_config, line[0], line_number))
			return (1);
		if (checkArgNumber(line.size(), 2, line_number, line[0]))
			return (1);
		if (line[1] == "on")
			to_config->setAutoIndex(true);
		else if (line[1] == "off")
			to_config->setAutoIndex(false);
		else
		{
			std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : '" << line[0] << "' type only accept values 'on' or 'off'" << std::endl;
			return (1);
		}
		to_config->addSettedVariable(line[0]);
	}
	else if (line[0] == "root")
	{
		if (checkIsSet(to_config, line[0], line_number))
			return (1);
		if (checkArgNumber(line.size(), 2, line_number, line[0]))
			return (1);
		//if (line[1][0] == '.' && line[1][1] == '/')
		//	to_config->setRoot(std::string(line[1] , 2));
		//else
		//to_config->setRoot(line[1]);
		to_config->setRoot(fixSlash(line[1]));
		to_config->addSettedVariable(line[0]);
	}
	else if (line[0] == "cgi_path")
	{
		if (checkIsSet(to_config, line[0], line_number))
			return (1);
		if (checkArgNumber(line.size(), 2, line_number, line[0]))
			return (1);
		if (line[1][0] == '.' && line[1][1] == '/')
			to_config->setCGIPath(std::string(line[1] , 2));
		else
			to_config->setCGIPath(line[1]);
		if (!isFile(to_config->getCGIPath()))
		{
			std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : '" << to_config->getCGIPath() << "' is not a file" << std::endl;
			return (1);
		}
		to_config->addSettedVariable(line[0]);
	}
	else if (line[0] == "cgi_extension")
	{
		if (checkIsSet(to_config, line[0], line_number))
			return (1);
		if (checkArgNumber(line.size(), 2, line_number, line[0]))
			return (1);
		std::vector<std::string> tmp = line;
		tmp.erase(tmp.begin());
		to_config->setCGIExtension(tmp);
		to_config->addSettedVariable(line[0]);
	}
	else if (line[0] == "client_body_buffer_size")
	{
		if (checkIsSet(to_config, line[0], line_number))
			return (1);
		if (checkArgNumber(line.size(), 2, line_number, line[0]))
			return (1);
		if (!isNumber(line[1], line_number))
			return (1);
		to_config->setClientMaxBodySize(atoll(line[1].c_str()));
		to_config->addSettedVariable(line[0]);
	}
	else if (line[0] == "redirect")
	{
		if (checkIsSet(to_config, line[0], line_number))
			return (1);
		if (checkArgNumber(line.size(), 2, line_number, line[0]))
			return (1);
		to_config->setIsRedirect(true);
		to_config->setRedirectLink(line[1]);
		to_config->addSettedVariable(line[0]);
	}
	else
	{
		std::cerr << "Invalid line in configuration file.\nline[" << line_number << "] : Unknown type '" << line[0] << "'" << std::endl;
		return (1);
	}

	return (0);
}
