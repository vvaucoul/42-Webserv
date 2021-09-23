/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostMethod.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/04 17:39:45 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/19 22:04:08 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PostMethod.hpp"

//**************************//
//			DEFAULT 		//
//**************************//

PostMethod::PostMethod() {

}
PostMethod::PostMethod(PostMethod const &cpy) {
	*this = cpy;
}
PostMethod::~PostMethod() {

}
PostMethod &PostMethod::operator=(PostMethod const & cpy) {
	if (this == &cpy)
		return (*this);

	return (*this);
}

//**************************//
//		   POST / PUT 		//
//**************************//

bool PostMethod::usePostMethod(std::vector<std::string> const &v) {
	if (contain(v[0], "POST")
	|| contain(v[0], "PUT")) {
		if (v.size() >= 1)
			return (true);
	}
	return (false);
}

int	PostMethod::processPostMethod(std::vector<std::string> const &v) {
	std::string header = v[0];
	std::string form = v[1];
	return (0);
}
