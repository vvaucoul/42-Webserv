/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationsUtils.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/08 14:00:07 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 12:52:43 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Locations.hpp"

//**************************//
//      LOCATIONS UTILS 	//
//**************************//

void Locations::createLocations() {
	bool f = false;
	for (std::vector<t_locationNode>::iterator it = _allLocations.begin(); it != _allLocations.end(); ++it) {
		if (!isDir(it->path)) {
			system(std::string(std::string("mkdir -p ") + it->path.c_str()).c_str());
			formatInfo("Location - Utils", "", it->path + " has been created", "", 0);
			f = true;
		}
	}
	if (f)
		displayDimBar(DIM_BAR_SIZE);
}
bool Locations::requestLocation(std::string const &path) {
	std::string last;

	last = path.substr(nrfind(path, "/", 1), path.length());
	if (searchLocation(last, LS_LOCATION).found)
		return (true);
	else
		return (false);
}
std::pair<std::vector<t_locationNode>, size_t> Locations::pathNumberLocations(std::string const &path) {
	size_t nbLoc = 0;
	std::vector<t_locationNode> v;

	for (size_t i = 0; i < path.length(); i++) {
		std::string cLoc;
		size_t j = path[i] == '/' ? i + 1 : i;
		size_t len = 0;

		while (path[j] != '/' && path[j])
			++j;
		len = j - i;
		cLoc = path.substr(i, len);
		if (locationPathisValid(cLoc)) {
			++nbLoc;
			t_locationNode node = searchLocation(cLoc, LS_LOCATION);
			v.push_back(node);
		}
	}
	return (FT::getPair(v, nbLoc));
}
bool Locations::rootAcceptMethod(Server *root, std::string const &method) {
	std::vector<std::string> methods = root->getMethod();

	for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); ++it) {
		if (((*it) == method))
			return (true);
	}
	return (false);
}
bool Locations::oneOnePathLocationAcceptMethod(std::string const &path, std::string const &method) {
	std::pair<std::vector<t_locationNode>, size_t> nbLocations = pathNumberLocations(path);
	if (nbLocations.second < 2)
		return (false);
	else {
		std::vector<t_locationNode> v = nbLocations.first;
		for (std::vector<t_locationNode>::iterator it = v.begin(); it != v.end(); ++it) {
			if (FT::contain(it->data->getMethod(), method))
				return (true);
		}
	}
	return (false);
}
bool Locations::getFirstLocationALlowMethod(std::string const &path, std::string const &method, Server *root) {
	std::pair<std::vector<t_locationNode>, size_t> nbLocations = pathNumberLocations(path);

	std::vector<t_locationNode> v = nbLocations.first;
	for (std::vector<t_locationNode>::iterator it = v.begin(); it != v.end(); ++it) {
		if (it->data->getMethod().empty()) {
			continue ;
		}
		if (FT::contain(it->data->getMethod(), method))
			return (true);
		else
			return (false);
	}
	if (rootAcceptMethod(root, method)) {
		return (true);
	}
	else
		return (false);
}
bool Locations::locationPathisValid(std::string const &location) {
	for (std::vector<t_locationNode>::iterator it = _allLocations.begin(); it != _allLocations.end(); ++it) {
		if (!fixSlashFL(it->data->getLocationName()).compare(fixSlashFL(location))
		&& !fixSlashFL(location).empty()
		&& !fixSlashFL(it->data->getLocationName()).empty()) {
			return (true);
		}
	}
	return (false);
}
size_t	Locations::getMaxLocationDepth(std::vector<t_locationNode> v) {
	size_t depth = 0;
	(void)v;
	for (std::vector<t_locationNode>::iterator it = _allLocations.begin(); it != _allLocations.end(); ++it) {
		if ((*it).depth > depth)
			depth = (*it).depth;
	}
	return (depth);
}
t_locationNode Locations::getLocationByName(std::string const &location) {
	size_t maxDepth = getMaxLocationDepth(_allLocations);
	size_t currentDepth = 0;

	while (currentDepth < maxDepth) {
		for (std::vector<t_locationNode>::iterator it = _allLocations.begin(); it != _allLocations.end(); ++it) {
			if ((*it).depth != currentDepth
			|| (*it).data->getLocationName().empty())
				continue;
			if (contain((*it).data->getLocationName(), location)) {
				return (*it);
			}
			else
				continue;
		}
		++currentDepth;
	}
	return (makeNullLocation());
}

std::string Locations::getErrorPageByCode(std::map<int, std::string> errorsPages, size_t code) {
	for (std::map<int, std::string>::iterator it = errorsPages.begin(); it != errorsPages.end(); ++it) {
		if ((size_t)it->first == code)
			return (it->second);
	}
	return ("");
}

//**************************//
//  PROCESS LOCATIONS UTILS //
//**************************//


t_locationNode Locations::getLastValidLocationPathByDepth(std::string const &file) {

	t_locationNode node;
	std::string currentLocation;

	currentLocation = truncPath(file, 1);
	node = getLocationByName(currentLocation);
	std::vector<t_locationNode> v = ConvertServersToLocations(node.data, node.data->getRoot());

	for (std::vector<t_locationNode>::iterator it = v.begin(); it != v.end(); ++it) {
	}
	return (makeSafeNullLocation());
}

std::vector<t_locationNode> Locations::ConvertServersToLocations(Server *current, std::string const &sRoot) {
	std::vector<t_locationNode> v;
	Locations newLoc;

	newLoc.initLocations(current, sRoot, 0);
	v = newLoc.getLocations();
	return (v);
}
t_locationNode	Locations::getLocationByDepth(std::string const &location, size_t const depth) {
	for (std::vector<t_locationNode>::iterator it = _allLocations.begin(); it != _allLocations.end(); ++it) {
		if (!(*it).found
		|| location.empty()
		|| !(*it).data->getIsLocation())
			continue ;
		if ((*it).depth == depth) {
			if (contain(location, (*it).data->getLocationName())
			|| location == (*it).data->getLocationName())
				return (*it);
		}
	}
	return (makeSafeNullLocation());
}
std::string	Locations::getPathWithSearchType(t_locationNode const &currentNode, E_LocationsSearch const &type) {
	std::string path;
	{
		switch ((int)type) {
			case LS_LOCATION: {
				path = fixSlash(currentNode.path + "/");
				break;
			}
			case LS_PAGE: {
				path = fixSlash(currentNode.path + "/" + currentNode.data->getHTMLPath());
				break;
			}
			case LS_ERRORPAGE: {
				path = fixSlash(currentNode.path + "/" + getErrorPageByCode(currentNode.data->getErrorPages(), _code));
				break;
			}
			case LS_CGI: {
				path = fixSlash(currentNode.path + "/" + currentNode.data->getCGIPath());
				break;
			}
			default: {
				path = currentNode.path;
				break;
			}
		}
	}
	return (path);
}

//**************************//
//   LOCATIONS PRINT UTILS  //
//**************************//


void 	Locations::printNode(t_locationNode const &node) {
	std::cout << RESET << DIM << "-------------------------" << RESET << '\n';
	std::cout << CYAN << "Node: Found: " << YELLOW << node.found << '\n';
	if (!(node.found)) {
		std::cout << RESET << DIM << "-------------------------" << RESET << '\n';
		return ;
	}
	else {
		std::cout << CYAN << "Node: Location: " << YELLOW << node.data->getLocationName() << '\n';
		std::cout << CYAN << "Node: root: " << YELLOW << node.data->getRoot() << '\n';
		std::cout << CYAN << "Node: Path: " << YELLOW << node.path << '\n';
		std::cout << CYAN << "Node: Depth: " << YELLOW << node.depth << '\n';
	}
	std::cout << RESET << DIM << "-------------------------" << RESET << '\n';
}
