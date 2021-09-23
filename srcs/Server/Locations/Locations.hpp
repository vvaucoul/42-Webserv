/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Locations.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/27 11:17:59 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 12:51:22 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONS_HPP
# define LOCATIONS_HPP

# include <iostream>
# include "../../utils/utils.hpp"
# include "../Server.hpp"

# include <dirent.h>
# include <sys/types.h>

# define DEFAULT_WEB_PAGE "index.html"

class Server;

class Locations {
private:
	std::vector<t_locationNode> _allLocations;
	size_t						_code;

	// ### UTILS ### //
	std::vector<t_locationNode> ConvertServersToLocations(Server *current, std::string const &sRoot);
	t_locationNode				getLocationByDepth(std::string const &location, size_t const depth);
	std::string					getPathWithSearchType(t_locationNode const &currentNode, E_LocationsSearch const &type);
	std::string					truncPath(std::string const &str, size_t const &indexLocation);
	size_t						getMaxLocationDepth(std::vector<t_locationNode> v);

public:
	Locations ();
	Locations (Server const currentServer, std::string const &requestPage);
	Locations (Locations const &cpy);
	Locations &operator=(Locations const &cpy);
	virtual ~Locations ();

	std::string fixSlash(std::string const &str);

	// Manager
	void initLocations(Server *root, std::string const &currentPath,  size_t depth);
	void printLocations();
	std::vector<t_locationNode> getLocations() { return (_allLocations); }

	t_locationNode processLocations(std::string const &path, E_LocationsSearch const &type);

	// Recherche une donnée spécifique dans une location
	// Exemple, recherche tel page, tel page d'erreur, tel ...
	t_locationNode	searchLocation(std::string const &location, E_LocationsSearch const &type);

	// Recherche une donnée spécifique et return le path
	// Exemple, recherche index.html ou / -> return ./WebSite/index.html
	std::string		getLocationPath(std::string const &location, E_LocationsSearch const &type);

	// Recherche une location et renvoie le path complet de l'élément demandé
	// exemple, recherche location create, avec type == LS_Page, renvoie -> ./webSite/create/index.html
	// et la location avec les données du serveur + path de la location
	std::pair<t_locationNode, std::string> searchLocationPath(std::string const &location, E_LocationsSearch const &type);

	// Recherche un fichier spécifique a partir du root serveur
	// Exemple: file = to_delete.tmp -> return ./webSite/d_entry/delete/to_delete.tmp
	std::string searchFile(std::string const &file);

	// Créer les locations non existantes
	void		createLocations();


	t_locationNode	makeNullLocation() {
		return ((t_locationNode){NULL, std::vector<t_locationNode>(), std::vector<t_locationNode>(), "", 0, false});
	}
	t_locationNode makeSafeNullLocation() {
		return ((t_locationNode){_allLocations[0].data, std::vector<t_locationNode>(), std::vector<t_locationNode>(), _allLocations[0].path, 0, false});
	}

	// check si le path est une location ou non
	bool		requestLocation(std::string const &path);

	//bool		locationIsValid(t_locationNode node) { return (!node.path.empty() && node.data); }
	// renvoie la derniere location valide du path
	// SafeNull sinon
	std::pair<t_locationNode, std::string>	getLastValidLocationPath(std::string const &location) {

	//	std::cout << "location: " << location << '\n';
		std::string tmp = location;

		while (nbChar(tmp, '/') > 1) {
			tmp = tmp.substr(nrfind(tmp, "/", 2), nrfind(tmp, "/", 1));
		//	std::cout << "tmp: "<< tmp << '\n';
		}
		if (locationIsValid(searchLocationPath(tmp, LS_LOCATION).first)) {
		//	std::cout << "return valid location" << '\n';
			t_locationNode n = searchLocationPath(tmp, LS_LOCATION).first;
			return (FT::getPair((t_locationNode){n.data, std::vector<t_locationNode>(), std::vector<t_locationNode>(), n.path, n.depth, n.found}, tmp));
		}
		else {
		//	std::cout << "return INVALID location" << '\n';

			return (searchLocationPath(_allLocations[0].path, LS_LOCATION));
		}
		//return (FT::getPair(Locations::makeSafeNullLocation(), _allLocations[0].path));
}
	t_locationNode							getLocationByName(std::string const &location);
	t_locationNode							getLastValidLocationPathByDepth(std::string const &file);

	// Check tout le path s'il contient minimum 2 location
	// Si l'une des location accepte la method, return vrai sinon falses
	bool oneOnePathLocationAcceptMethod(std::string const &path, std::string const &method);

	// check si le root accepte la méthode,
	bool rootAcceptMethod(Server *root, std::string const &method);

	// return number of location in path
	// Exemple : ./webSite/d_entry/delete/ return 2 (d_entry + delete)
	std::pair<std::vector<t_locationNode>, size_t>	pathNumberLocations(std::string const &path);

	// check si la string location est valide
	bool	locationPathisValid(std::string const &location);

	// recupere la premiere location recursive au dessus avec un allow methods
	// compare avec la method et renvoie vrai ou false.
	// dans notre fichier de config, le dernier allow_methods surpasse tout ceux au dessus.
	bool getFirstLocationALlowMethod(std::string const &path, std::string const &method, Server *root);


	void 	printNode(t_locationNode const &node);

	void 						setCode(size_t const code) {
		_code = code;
	}
	std::string 				getErrorPageByCode(std::map<int, std::string> errorsPages, size_t code);

};

// new method to get locations
inline Locations	getLocation(Server *root, std::string const &sroot, size_t const code) {
	Locations loc;

	loc.setCode(code);
	loc.initLocations(root, sroot, 0);
	return (loc);
}

inline void 		printLocations(Server *root, std::string const &sroot) {
	Locations loc;

	loc.initLocations(root, sroot, 0);
	loc.printLocations();;
}
// fix first and last slash (for printTreeLocations func)
inline	std::string	fixSlashFL(std::string const &str) {
	std::string nstr = str;

	if (nstr[0] == '/') {
		nstr.erase(0);
	}
	if (nstr[nstr.length() - 1] == '/') {
		nstr.erase(nstr.length() - 1);
	}
	return (nstr);
}
inline std::string	fixSlash(std::string const &str) {
	Locations loc;

	return (loc.fixSlash(str));
}
inline void deleteDoublons(std::string &str, std::string const &toRemove);

// get last root in path, example : website/sub/sub2 -> return sub2
inline std::string getLastPathRoot(std::string const &str) {
	if (str.empty())
		return (str);
	if ((str.rfind("/")) == std::string::npos
		|| nbChar(str, '/') < 2) {
		return (str);
	}
	else {
		std::string nstr;

		nstr = str.substr(nrfind(str, "/", 2), str.length());
		return (nstr);
	}
	return (str);
}

// renvoie le path du contenu souhaité
// exemple: create/index.html -> renvoie ./webSite/create/index/html
// index.html -> renvoie ./WebSite/index.html
inline std::string getFilePathSearchContent(std::string const &content, Server *srv, std::string const &root) {

	if (nbChar(content, '/') < 2)
		return (content);

	std::string newPath;
	std::string lastLocation = content.substr(nrfind(content, "/", 2), nrfind(content, "/", 1));

	newPath = (getLocation(srv, root, 200).searchLocationPath(lastLocation, LS_LOCATION).second) + content.substr(nrfind(content, "/", 1), content.length());
	newPath = fixSlash(newPath);
	return (newPath);
}

// renvoie true si c est une extension de fichier
// sinon, false
inline bool isFileExtension(std::string const &file) {
	if (file.find(".") != NPOS)
		return (true);
	else
		return (false);
}

#endif
