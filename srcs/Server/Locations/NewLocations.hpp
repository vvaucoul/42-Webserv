/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NewLocations.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/10 21:07:49 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 12:53:55 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NEWLOCATIONS_HPP
#define NEWLOCATIONS_HPP

# include <iostream>
# include "../Server.hpp"

# define SAFE_NULL_LOCATION (t_locationNode){NULL, std::vector<s_locationNode>(), std::vector<t_locationNode>(), "", 0, false}
# define IS_VALID_LOCATION(location) location.found
# define E_SEARCH_PAGE_TO_STR(sp) (sp == 0 ? "Page" : sp == 1 ? "Directory" : "Invalid")
# define SERVER_ROOT _root

class NewLocations {
private:
	Server	*_srv;
	Server	*_rootServer;

	std::string					_root;
	std::vector<t_locationNode>	_locations;

private:
	Server	*getRootServer();
	void	initLocations(Server *root);

	// Server utils
	t_locationNode makePrevLocations(Server *srv);
	std::vector<t_locationNode> makeNextLocations(Server *srv);
	std::string makeLocationPathFromServer(Server *srv);
	size_t getLocationDepthFromServer(Server *srv);
	t_locationNode convertServerToLocation(Server *srv);

	// Search Locations Utils
	E_SEARCH_PAGE	setPageType(std::string const &requestPage);
	bool 			LocationExit(std::string const &locationName, size_t const depth);
	bool 			isInValidLocations(std::string const &requestPage);

	t_locationNode 	getLocationByLocationName(std::string const &name, size_t const depth);
	t_locationNode 	getLastLocationInFile(std::string const &requestPage);

	t_locationNode	searchLocationDirectory(std::string const &requestPage);
	t_locationNode 	searchLocationFile(std::string const &requestPage);

	t_locationNode	hasOneOfNextLocation(std::vector<t_locationNode> currentLocation, std::string const &requestLocation);

	std::vector<std::string> convertURIToTab(std::string const &requestPage);
	std::string fixName(std::string const &name);


public:
	NewLocations (Server *srv);
	NewLocations (NewLocations const &cpy);
	NewLocations &operator=(NewLocations const &cpy);
	virtual ~NewLocations ();

public:

	// Location creation
	void	createLocations();

	// Search Locationsmake
	t_locationNode searchLocation(std::string const &requestPage);
	t_locationNode getLastValidLocation(std::string const &requestPage);
	t_locationNode getFirstLocationALlowMethod(t_locationNode const current, std::string const &method);
	t_locationNode getRootLocation(void);
	std::pair<t_locationNode, std::string> getErrorLocation(t_locationNode const &node, size_t const &code);

	std::string	getLocationPathFromServer(Server *srv);

	// Extra Utils
	std::pair<t_locationNode, std::string> getLocationPage(std::string const &requestPage);

	// Print
	void	printLocations(t_locationNode *location);
	void 	printServerNode(t_locationNode const &node);
	void 	printNode(t_locationNode const &node);

// Public utils
public:

	bool isFileExtension(std::string const &file);
	std::string fixSearchPathName(std::string const &requestPage);

};

inline NewLocations getLocations(Server *srv) {
	return (NewLocations(srv));
}

#endif
