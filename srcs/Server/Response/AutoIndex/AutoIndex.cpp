/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-faou <mle-faou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/26 17:56:27 by mle-faou          #+#    #+#             */
/*   Updated: 2021/09/20 12:59:02 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AutoIndex.hpp"

AutoIndex::AutoIndex(AutoIndex const &ai) {
	*this = ai;
}
AutoIndex &AutoIndex::operator=(AutoIndex const &ai) {
	if (this == &ai)
		return (*this);
	_page = ai._page;
	return (*this);
}

AutoIndex::AutoIndex(std::string addr, std::string path)
{
	DIR *dir;
	struct dirent *ent;
	std::vector<std::string> links;

	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			links.push_back(ent->d_name);
		}
		closedir (dir);
	}
	else
	{
		throw AutoIndex::UnreadableException();
	}

	if (links.size() == 0)
	{
		throw AutoIndex::UnreadableException();
	}

	_page += "<!DOCTYPE html>\n<html>\n<head>\n\t<title>";
	_page += path;
	_page += "</title>\n\t<style>\n\tbody\t{background-color: #242424;}\n\th1\t{color: #ffffff;}\n\tp\t{margin: 0;}\n\ta\t{color: #ffffff;}\n\t</style>\n</head>\n<body>\n<h1>INDEX - ";
	_page += path;
	_page += "</h1>\n<br>\n<p>\n";

	_page += "\t<p><a href=\"";
	_page += addr;
	_page += "/";
	_page += ".";
	_page += "\">";
	_page += ".";
	_page += "</a></p>\n";

	_page += "\t<p><a href=\"";
	_page += addr;
	_page += "/";
	_page += "..";
	_page += "\">";
	_page += "..";
	_page += "</a></p>\n";

	for (int i = 0; i < (int)links.size(); ++i)
	{
		if (links[i] != "." && links[i] != "..")
		{
			_page += "\t<p><a href=\"";
			_page += "/";
			_page += links[i];
			_page += "\">";
			_page += links[i];
			_page += "</a></p>\n";
		}
	}

	_page += "</p>\n</body>\n</html>\n";
}
