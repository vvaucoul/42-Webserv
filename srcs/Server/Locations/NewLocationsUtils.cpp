/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NewLocationsUtils.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/12 16:18:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 12:54:01 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "NewLocations.hpp"

//**************************//
//   	LOCATIONS UTILS     //
//**************************//

bool NewLocations::isFileExtension(std::string const &file) {
	if (file.find(".") != NPOS)
		return (true);
	else
		return (false);
}
