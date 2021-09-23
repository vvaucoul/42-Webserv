/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checkConfig.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/19 22:57:05 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 10:15:50 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHECKCONFIG_HPP
#define CHECKCONFIG_HPP

# include <stdlib.h>
# include <iostream>
# include <stdio.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <dirent.h>
# include <errno.h>
# include <string.h>

# include <algorithm>
# include <vector>

# define RESET "\e[0m"
# define RED "\e[1;31m"
# define GREEN "\e[1;32m"
# define YELLOW "\e[1;33m"
# define BLUE "\e[1;34m"
# define MAGENTA "\e[1;35m"
# define CYAN "\e[1;36m"

# define BOLD "\e[1m"
# define DIM "\e[2m"
# define UNDERLINE "\e[4m"
# define BLINK "\e[5m"

#endif /* end of include guard: CHECKCONFIG_HPP */
