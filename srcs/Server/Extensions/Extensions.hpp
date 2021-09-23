/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Extensions.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/26 14:42:42 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/01 14:19:04 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXTENSIONS_HPP
# define EXTENSIONS_HPP

# include <iostream>
# include "../../utils/utils.hpp"
# include "../Server.hpp"

class Server;

class Extensions {
private:
	std::vector<t_extensionNode> _allExtensionsNodes;

	Server						*_srv;
	std::string					_root;

	t_extensionNode				makeExtensionNode(Server *data, Server *root, bool found) {
		return ((t_extensionNode){root, data, found});
	}

public:
	Extensions ();
	Extensions (Extensions const &cpy);
	Extensions &operator=(Extensions const &cpy);
	virtual ~Extensions ();

	void 	initExtensions(Server *srv, std::string const &root);
	std::pair<t_extensionNode, bool>	FileIsInExtension(std::string const &file);
	void 	PrintExtensions();

};

inline Extensions	getExtensions(Server *srv, std::string const &root) {
	Extensions ext;

	ext.initExtensions(srv, root);
	return (ext);
};

#endif
