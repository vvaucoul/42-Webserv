/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NewLocations.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/10 21:12:09 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 12:53:44 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "NewLocations.hpp"

//**************************//
//			DEFAULT  		//
//**************************//

NewLocations::NewLocations(Server *srv) : _srv(srv) {
	_rootServer = getRootServer();
	_root = _rootServer->getRoot();
	initLocations(_rootServer);
}
NewLocations::NewLocations(NewLocations const &cpy) {
	*this = cpy;
}
NewLocations &NewLocations::operator=(NewLocations const &cpy) {
	if (this == &cpy)
		return (*this);
	_srv = cpy._srv;
	_rootServer = cpy._rootServer;
	_locations = cpy._locations;
	return (*this);
}
NewLocations::~NewLocations () {

}

//**************************//
//			UTILS	  		//
//**************************//

// Get Root Server with any location
Server *NewLocations::getRootServer() {
	Server *curr = _srv;

	while (curr->getParentLocation()) {
		curr = curr->getParentLocation();
	}
	return (curr);
}

// Create Locations
static bool createLocations(std::vector<t_locationNode> v) {
	bool locationCreated = false;

	for (std::vector<t_locationNode>::iterator it = v.begin(); it != v.end(); ++it) {
		if (!isDir(it->path)) {
			system(std::string(std::string("mkdir -p ") + it->path.c_str()).c_str());
			formatInfo("LOCATIONS", "", " " + it->path + RESET BOLD " has been created", "", 0);
			locationCreated = true;
		}
		createLocations((*it).next);
	}
	return (locationCreated);
}
void NewLocations::createLocations() {
	bool locationCreated = false;
	std::cout << "\n";

	locationCreated = ::createLocations(_locations[0].next);
}
std::string NewLocations::fixSearchPathName(std::string const &requestPage) {
	std::string nstr;

	if (nbChar(requestPage, '/') >= 1) {
		std::string tr;

		tr = requestPage.substr(0, requestPage.find_first_of("/"));
		if (removeChars(tr, '/') == removeChars(_locations[0].data->getRoot(), '/')) {
			size_t pos = requestPage.find_first_of("/");
			nstr = requestPage.substr(pos, requestPage.length() - pos);
			return (nstr);
		}

	} else {
		return (requestPage);
	}
	return (requestPage);
}

//**************************//
// SEARCH LOCATION - EXTRAS //
//**************************//

std::pair<t_locationNode, std::string> NewLocations::getLocationPage(std::string const &requestPage) {
	t_locationNode node;
	std::string path;

	node = searchLocation(requestPage);
	if (!IS_VALID_LOCATION(node)) {
		return (FT::getPair(SAFE_NULL_LOCATION, ""));
	} else if (!isFile(requestPage))
		path = fixSlash(node.path + "/" + node.data->getHTMLPath());
	else
		path = requestPage;
	return (FT::getPair(node, path));
}

//**************************//
//		SEARCH LOCATION	 	//
//**************************//

std::pair<t_locationNode, std::string> NewLocations::getErrorLocation(t_locationNode const &node, size_t const &code) {
	t_locationNode curr = node;

	while (IS_VALID_LOCATION(curr)) {
		std::map<int, std::string> errors = curr.data->getErrorPages();
		if (!errors[code].empty()) {
			return (FT::getPair(curr, errors[code]));
		}
		if (!curr.data->getParentLocation())
			break;
		curr = convertServerToLocation(curr.data->getParentLocation());
	}
	return (FT::getPair(SAFE_NULL_LOCATION, ""));
}
t_locationNode NewLocations::getRootLocation(void) {
	return (_locations[0]);
}
t_locationNode NewLocations::getFirstLocationALlowMethod(t_locationNode const current, std::string const &method) {
	t_locationNode cur;

	cur = current;
	while (IS_VALID_LOCATION(cur)) {
		std::vector<std::string> v = cur.data->getMethod();
		for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it) {
			if ((*it) == method)
				return (cur);
		}
		if (cur.data->getParentLocation())
			cur = convertServerToLocation(cur.data->getParentLocation());
		else
			cur = SAFE_NULL_LOCATION;
	}
	return (SAFE_NULL_LOCATION);
}
t_locationNode NewLocations::getLastValidLocation(std::string const &requestPage) {

	if (requestPage == "/" || requestPage == _locations[0].data->getHTMLPath())
		return (_locations[0]);

	std::vector<std::string> URI_Locations = convertURIToTab(fixSlash(requestPage));
	t_locationNode node = _locations[0];
	t_locationNode previous;
	for (std::vector<std::string>::iterator it = URI_Locations.begin(); it != URI_Locations.end() - 1; ++it) {

		previous = node;
		node = hasOneOfNextLocation(node.next, *it);
		if (node.found) {
		}
		else if (!node.found) {
			return (previous);
		}
	}
	return (node);
}
t_locationNode NewLocations::hasOneOfNextLocation(std::vector<t_locationNode> currentLocation, std::string const &requestLocation) {
	if (currentLocation.empty())
		return (SAFE_NULL_LOCATION);
	for (std::vector<s_locationNode>::iterator it = currentLocation.begin(); it != currentLocation.end(); ++it) {
			if (fixName((*it).data->getLocationName()) == fixName(requestLocation))
				return (*it);
	}
	return (SAFE_NULL_LOCATION);
}
t_locationNode NewLocations::searchLocation(std::string const &requestPage) {

	// Return Root (dossier == / || page == index page)
	if (requestPage == "/" || fixName(requestPage) == fixName(_locations[0].data->getHTMLPath()))
		return (_locations[0]);

	if (isDir(std::string(SERVER_ROOT + "/" + requestPage))) {

		std::vector<std::string> URI_Locations = convertURIToTab(fixSlash(requestPage));
		t_locationNode node = _locations[0];

		for (std::vector<std::string>::iterator it = URI_Locations.begin(); it != URI_Locations.end(); ++it) {
			node = hasOneOfNextLocation(node.next, *it);

			if (node.found) {
			}
			else if (!node.found) {
				return (SAFE_NULL_LOCATION);
			}
		}
		return (node);
	}
	else if (isFile(std::string(SERVER_ROOT + "/" + requestPage))) {
		std::vector<std::string> URI_Locations = convertURIToTab(fixSlash(requestPage));
		t_locationNode node = _locations[0];

		for (std::vector<std::string>::iterator it = URI_Locations.begin(); it != URI_Locations.end() - 1; ++it) {

			node = hasOneOfNextLocation(node.next, *it);
			if (node.found) {
			}
			else if (!node.found) {
				return (SAFE_NULL_LOCATION);
			}
		}
		return (node);
	}
	else {
		return (SAFE_NULL_LOCATION);
	}

	return (SAFE_NULL_LOCATION);
}

std::string	NewLocations::getLocationPathFromServer(Server *srv) {
	return (makeLocationPathFromServer(srv));
}

//**************************//
//	  LOCATIONS - CREATION	//
//**************************//

std::string NewLocations::makeLocationPathFromServer(Server *srv) {
	std::string location, root, ori_root, prevLocation, prevRoot;

	std::string path;
	Server *curr = srv;

	if (!curr->getParentLocation())
		return (fixSlash(srv->getRoot() + "/"));
	while (curr->getParentLocation()) {
		location = removeChars(curr->getLocationName(), '/');
		ori_root = fixSlash(curr->getRoot() + "/");
		root = removeChars(curr->getRoot(), '/');

		if (curr->getParentLocation()) {
			prevLocation = removeChars(curr->getParentLocation()->getLocationName(), '/');
			prevRoot = removeChars(curr->getParentLocation()->getRoot(), '/');
		} else {
			prevLocation = "";
			prevRoot = "";
		}

		if (root.compare(prevLocation)) {
			path.insert(0, ori_root + "/" + location + "/");
		} else {
			path.insert(0, "/" + location + "/");
		}
		curr = curr->getParentLocation();
	}
	if (path.empty())
		return (fixSlash(SERVER_ROOT + srv->getRoot() + "/"));
	return (fixSlash(SERVER_ROOT + path));
}
size_t NewLocations::getLocationDepthFromServer(Server *srv) {
	Server *curr = srv;
	size_t depth = 0;

	while (curr->getParentLocation()) {
		depth++;
		curr = curr->getParentLocation();
	}
	return (depth);
}
std::vector<t_locationNode> NewLocations::makeNextLocations(Server *srv) {
	std::vector<t_locationNode> next;
	std::vector<Server *> v;

	if (!srv)
		return (std::vector<t_locationNode>());
	else if (!srv->hasLocations())
		return (std::vector<t_locationNode>());
	else if (srv->getLocations().empty())
		return (std::vector<t_locationNode>());
	v = srv->getLocations();
	for (std::vector<Server *>::iterator it = v.begin(); it != v.end(); ++it) {
		next.push_back(convertServerToLocation((*it)));
	}
	return (next);
}
t_locationNode NewLocations::makePrevLocations(Server *srv) {
	t_locationNode node;
	Server *prev;

	if (!srv)
		return (SAFE_NULL_LOCATION);
	if (!srv->getParentLocation())
		return (SAFE_NULL_LOCATION);
	prev = srv->getParentLocation();
	node.data = prev;
	node.depth = getLocationDepthFromServer(prev);
	node.path = makeLocationPathFromServer(prev);
	node.found = true;
	node.prev.push_back(SAFE_NULL_LOCATION);
	return (node);
}
t_locationNode NewLocations::convertServerToLocation(Server *srv) {
	t_locationNode node;

	node.data = srv;
	node.depth = getLocationDepthFromServer(srv);
	node.path = makeLocationPathFromServer(srv);
	node.found = true;
	node.prev.push_back(makePrevLocations(srv));
	node.next = makeNextLocations(srv);
	return (node);
}
void NewLocations::initLocations(Server *root) {
	_locations.push_back(convertServerToLocation(root));
}

//**************************//
//		SEARCH - UTILS	 	//
//**************************//

bool NewLocations::isInValidLocations(std::string const &requestPage) {
	std::string curr;
	size_t depth = nbChar(requestPage, '/') - 1;
	size_t pos_start = 0;
	size_t pos_end = 0;

	if (!depth)
		return (true);

	std::string file, location;
	pos_start = nrfind(requestPage, "/", 1);
	pos_end = requestPage.length() - pos_start;
	file = requestPage.substr(pos_start, pos_end);
	pos_start = 0;
	pos_end = nrfind(requestPage, "/", 1) - pos_start;
	location = fixSlash(requestPage.substr(pos_start, pos_end) + "/");

	for (std::vector<t_locationNode>::iterator it =_locations.begin(); it != _locations.end(); ++it) {
		if (it->depth <= 0)
			continue;

		std::string ref, check;
		ref = fixSlash(removeChars(it->path, '/'));
		check = fixSlash(removeChars(location, '/'));

		if ((isFile(fixSlash(_locations[0].data->getRoot() + "/" + ref + file))
			|| isDir(fixSlash(_locations[0].data->getRoot() + "/" + ref + file)))
			&& ref == check)
			return (true);
	}
	return (false);
}
bool NewLocations::LocationExit(std::string const &locationName, size_t const depth) {
	for (std::vector<t_locationNode>::iterator it = _locations.begin(); it != _locations.end(); ++it) {
		if (it->depth == depth && locationName == removeChars(it->data->getLocationName(), '/') && !it->data->getLocationName().empty())
			return (true);
	}
	return (false);
}
std::vector<std::string> NewLocations::convertURIToTab(std::string const &requestPage) {
	return (strToTab(requestPage, '/'));
}
std::string NewLocations::fixName(std::string const &name) {
	std::string n = fixSlash(name);
	n = removeChars(n, '/');
	return (n);
}

//**************************//
//   LOCATIONS PRINT UTILS  //
//**************************//

void	NewLocations::printLocations(t_locationNode *location) {
	std::vector<t_locationNode> allNodes;

	if (!location) {
		allNodes = _locations;
	} else {
		allNodes = (*location).next;
	}
	for (std::vector<t_locationNode>::iterator it = allNodes.begin(); it != allNodes.end(); ++it) {
		printServerNode(*it);
		printLocations(&(*it));
	}
}
void 	NewLocations::printServerNode(t_locationNode const &node) {
	for (size_t i = 0; i < node.depth; i++) {
		std::cout << DIM " " RESET;
	}
	std::cout << DIM << "L-> " << RESET << BOLD;

	if (!node.data->getMethod().empty()) {
		std::cout << CYAN "[";
		std::vector<std::string> methods = node.data->getMethod();
		for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); ++it) {
			std::cout << *it;
			if ((it + 1) != methods.end())
			std::cout << ", ";
		}
		std::cout << "]" RESET BOLD " ";
	}
	std::string name;
 	name = node.data->getLocationName();
	if (node.depth == 0)
		name = "/";
	std::cout << name << RESET << DIM << " (root: " << node.path << ")" << RESET << '\n';
}
void 	NewLocations::printNode(t_locationNode const &node) {
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
