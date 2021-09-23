/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-faou <mle-faou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/26 17:55:04 by mle-faou          #+#    #+#             */
/*   Updated: 2021/09/20 12:57:25 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AUTOINDEX_HPP
#define AUTOINDEX_HPP

#include <dirent.h>

#include "../../Server.hpp"

class AutoIndex
{
private:
	std::string					_page;

public:
	AutoIndex(std::string addr, std::string path);
	AutoIndex(AutoIndex const &ai);
	AutoIndex &operator=(AutoIndex const &ai);
	~AutoIndex() {};

	std::string getPage(void) const {return (_page);};

	class UnreadableException : public std::exception {
		virtual const char *what() const throw() {return ("UnreadableException");};
	};
	class InvalidException : public std::exception {
		virtual const char *what() const throw() {return ("InvalidException");};
	};

};

#endif
