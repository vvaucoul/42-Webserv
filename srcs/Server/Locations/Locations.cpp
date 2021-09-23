/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Locations.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/27 11:17:52 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 12:51:12 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Locations.hpp"
//**************************//
//			DEFAULT  		//
//**************************//

Locations::Locations() {

}
Locations::~Locations() {
	_allLocations.clear();
}

//**************************//
//		  LOCATIONS  		//
//**************************//

std::string Locations::fixSlash(std::string const &str) {
	std::string ret = "";
	for (size_t i = 0; i < str.size(); i++) {
		if (str[i] == '/' && str[i + 1] == '/')
			continue;
		else
			ret += str[i];
	}
	return (ret);
}

//**************************//
//	  LOCATIONS  MANAGER	//
//**************************//

// ### INIT ### //

static std::string makeLocationPath(Server *curr, Server *prev, std::string const &currentPath) {
	std::string path;

	if (removeChars(curr->getRoot(), '/').compare(removeChars(prev->getLocationName(), '/'))) {
		path = currentPath + "/" + curr->getRoot() + "/" + curr->getLocationName();
	}
	else {
		path = currentPath + "/" + curr->getLocationName();
	}
	path = fixSlash(path);
	return (path);
}
void Locations::initLocations(Server *root, std::string const &currentPath, size_t depth) {
	std::vector<Server *> locations = root->getLocations();

	// Add server as first location, to prevent segfault with empty location file
	if (depth == 0) {
		_allLocations.push_back((t_locationNode){root, std::vector<t_locationNode>(), std::vector<t_locationNode>(), root->getRoot(), depth, true});
		++depth;
	}
	for (std::vector<Server *>::iterator it = locations.begin(); it != locations.end(); ++it) {
		_allLocations.push_back((t_locationNode){*it, std::vector<t_locationNode>(), std::vector<t_locationNode>(), makeLocationPath(*it, root, currentPath), depth, true});
		if ((*it)->hasLocations())
			initLocations(*it, makeLocationPath(*it, root, currentPath), depth + 1);
	}
}

// ### UTILS ### //

std::string Locations::truncPath(std::string const &str, size_t const &indexLocation) {
	size_t pos1 = 0;
	size_t pos2 = 0;

	pos1 = nfind(str, "/", indexLocation);
	if (pos1 == NPOS)
		pos1 = 0;
	pos2 = nfind(str, "/", indexLocation + 1);
	if (pos2 == NPOS)
		pos2 = str.length();

	return (str.substr(pos1, pos2 - pos1));
}

t_locationNode Locations::processLocations(std::string const &path, E_LocationsSearch const &type) {

	std::string newPath = path;

	// Exceptions Start
	{
		if (path == "/") {
			std::string nodePath = getPathWithSearchType(_allLocations[0], type);
			t_locationNode rNode = (t_locationNode){_allLocations[0].data, std::vector<t_locationNode>(), std::vector<t_locationNode>(), nodePath, _allLocations[0].depth, _allLocations[0].found};

			return (rNode);
		}
		if (path[0] == '/')
			newPath = path.substr(1, path.length() - 1);
	}

	// Path Is File
	if (contain(path, ".")) {
		if (isFile(fixSlash(_allLocations[0].data->getRoot() + "/" + path))) {
			t_locationNode rNode = getLastValidLocationPathByDepth(path);
			std::string nodePath = getPathWithSearchType(rNode, type);

			rNode = (t_locationNode){rNode.data, std::vector<t_locationNode>(), std::vector<t_locationNode>(), nodePath, rNode.depth, rNode.found};
			return (rNode);
		}
		else
			return (makeSafeNullLocation());
	} else {
		std::vector<Server *> allNodes;
		std::string currentLocation;
		size_t i = 0;
		t_locationNode currentNode;

		do {
			currentLocation = truncPath(newPath, i);
			if (currentLocation.empty()) {
				std::string nodePath;

				// make path with  E_LocationsSearch
				nodePath = getPathWithSearchType(currentNode, type);
				t_locationNode rNode = (t_locationNode){currentNode.data, std::vector<t_locationNode>(), std::vector<t_locationNode>(), nodePath, currentNode.depth, currentNode.found};
				return (rNode);
			}
			currentNode = getLocationByDepth(currentLocation, i + 1);
			if (!locationIsValid(currentNode))
				return (makeSafeNullLocation());
			allNodes = currentNode.data->getLocations();
			++i;
		} while (!currentLocation.empty());
	}
	return (makeSafeNullLocation());
}

t_locationNode Locations::searchLocation(std::string const &location, E_LocationsSearch const &type) {

	// cas spécial si on chercher aucune page, return le serveur
	// todo, si une location / existe, renvoyer le contenu de la location / et pas serveur
	if (!location.compare("/")) {
		return (_allLocations[0]);
	}

	for (std::vector<t_locationNode>::iterator it = _allLocations.begin(); it != _allLocations.end(); ++it) {
		switch ((int)type) {
			case LS_LOCATION:
			if (!removeChars(it->data->getLocationName(), '/').compare(removeChars(location, '/')))
				return (*it);
			break;
			case LS_PAGE:
			if (!removeChars(it->data->getHTMLPath(), '/').compare(removeChars(location, '/')))
				return (*it);
			break;
			case LS_ERRORPAGE:
			if (!removeChars(getErrorPageByCode(it->data->getErrorPages(), _code), '/').compare(removeChars(location, '/')))
				return (*it);
			break;
			case LS_CGI:

			if (!removeChars(it->data->getCGIPath(), '/').compare(removeChars(location, '/')))
				return (*it);
			break;
		}
	}
	// return le premier serveur si rien est trouvé, a vérifier
	return (makeSafeNullLocation());
}
std::pair<t_locationNode, std::string> Locations::searchLocationPath(std::string const &location, E_LocationsSearch const &type) {
	for (std::vector<t_locationNode>::iterator it = _allLocations.begin(); it != _allLocations.end(); ++it) {
		if (!removeChars(it->data->getLocationName(), '/').compare(removeChars(location, '/'))) {
			switch ((int)type) {
				case LS_LOCATION:
					return (FT::getPair(*it, fixSlash(it->path + "/")));
				case LS_PAGE:
					return (FT::getPair(*it, fixSlash(it->path + "/" + it->data->getHTMLPath())));
				case LS_ERRORPAGE:
					return (FT::getPair(*it, fixSlash(it->path + "/" + getErrorPageByCode(it->data->getErrorPages(), _code))));
				case LS_CGI:
					return (FT::getPair(*it, fixSlash(it->path + "/" + it->data->getCGIPath())));
			}
		}
	}
	return (FT::getPair(makeSafeNullLocation(), ""));
}
std::string	Locations::getLocationPath(std::string const &location, E_LocationsSearch const &type) {
	t_locationNode loc = searchLocation(location, type);

	if (!locationIsValid(loc))
		return ("");
	std::string path = loc.path;
	switch ((int)type) {
		case LS_LOCATION:
			return (fixSlash(path + "/" + loc.data->getLocationName()));
		case LS_PAGE:
			return (fixSlash(path + "/" + loc.data->getHTMLPath()));
		case LS_ERRORPAGE:
			return (fixSlash(path + "/" + getErrorPageByCode(loc.data->getErrorPages(), _code)));
		case LS_CGI:
			return (fixSlash(path + "/" + loc.data->getCGIPath()));
	}
	return ("");
}
std::string Locations::searchFile(std::string const &file) {
	for (std::vector<t_locationNode>::iterator it = _allLocations.begin(); it != _allLocations.end(); ++it) {
		std::string f = (it)->path + "/" + file;
		if (isFile(fixSlash(f))) {
			return (fixSlash(f));
		}
	}
	return ("");
}
void Locations::printLocations() {
	for (std::vector<t_locationNode>::iterator it = _allLocations.begin(); it != _allLocations.end(); ++it) {
		for (size_t i = 0; i < it->depth; i++) {
			std::cout << "  ";
		}
		std::cout << DIM << "L-> " << RESET << BOLD << it->data->getLocationName() << RESET << DIM << " (root: " << it->path << ")" << RESET << '\n';
	}
}
