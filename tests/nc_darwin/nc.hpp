/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nc.hpp                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:13:58 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/19 10:20:13 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NC_HPP
# define NC_HPP

# include <iostream>
# include <fstream>

# include <fcntl.h>
# include <unistd.h>

# include <vector>
# include <string.h>
# include <stdlib.h>

class NC {
private:
		std::string _file;
		size_t		_port;
		std::string	_ip;
		std::string	_output;
		std::string	_cmd;

		void ConvertFileToCurl();
		void ReadFile();
		void SendCurlCMD();

public:
	NC (std::string const &file, size_t const port, std::string const ip);
	NC (NC const &cpy);
	NC &operator=(NC const &cpy);
	virtual ~NC ();

	void ProcessNC();

	class InvalidFile : public std::exception {
		virtual const char *what() const throw() {return ("Error File");};
	};

};

#endif /* end of include guard: NC_HPP */
