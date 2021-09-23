/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Extensions.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/26 14:43:03 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 19:06:52 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Extensions.hpp"

//**************************//
//		    DEFAULT			//
//**************************//

Extensions::Extensions() {

}
Extensions::Extensions(Extensions const &cpy) {
	*this = cpy;
}
Extensions::~Extensions() {

}
Extensions &Extensions::operator=(Extensions const &cpy) {
	if (this == &cpy)
		return (*this);
	_allExtensionsNodes = cpy._allExtensionsNodes;
	_srv = cpy._srv;
	_root = cpy._root;
	return (*this);
}

//**************************//
//		  EXTENSIONS		//
//**************************//

void 	Extensions::initExtensions(Server *srv, std::string const &root) {
	_srv = srv;
	_root = root;
	std::vector<Server *> locations = srv->getLocations();

	for (std::vector<Server *>::iterator it = locations.begin(); it != locations.end(); ++it) {
		if ((*it)->hasExtensions()) {
			std::vector<Server *> vExtensions = (*it)->getExtensions();
			for (std::vector<Server *>::iterator itExt = vExtensions.begin(); itExt != vExtensions.end(); ++itExt) {
				_allExtensionsNodes.push_back((t_extensionNode){*it, *itExt, true});
			}
		}
	}
}

std::pair<t_extensionNode, bool>	Extensions::FileIsInExtension(std::string const &file) {
	NewLocations loc = NEW_LOCATIONS(_srv);
	t_locationNode lNode = loc.searchLocation(file);
	std::string file_extension = getFileExtension(file);

	if (IS_VALID_LOCATION(lNode)
		&& !file_extension.empty()) {
		for (std::vector<t_extensionNode>::iterator it = _allExtensionsNodes.begin(); it != _allExtensionsNodes.end(); ++it) {
			if (!IS_VALID_LOCATION(loc.searchLocation(file)))
				continue;
			else {
				if ((*it).data->getExtensionName() == (file_extension)) {
					return (FT::getPair(*it, true));
				}
			}
		}
	}
	return (FT::getPair(makeExtensionNode(NULL, NULL, false), false));
}

//**************************//
//		    DEBUG			//
//**************************//

void 	Extensions::PrintExtensions() {

	if (_allExtensionsNodes.empty()) {
		std::cerr << "ERROR: empty extensions" << '\n';
		return ;
	}
	std::cout << "NB Extensions: " << _allExtensionsNodes.size() << '\n';

	for (std::vector<t_extensionNode>::iterator it = _allExtensionsNodes.begin(); it != _allExtensionsNodes.end(); ++it) {

		(*it).data->printServerConfig();

		std::cout << "Extension: " << (*it).data->getExtensionName() << '\n';
		std::cout << "Extension - root Location: " << (*it).rootLocation->getLocationName() << '\n';
		std::cout << "Extension - found: " << (*it).found << '\n';
	}
}
