/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostMethod.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/04 17:39:16 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/06/04 17:50:33 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POSTMETHOD_HPP
# define POSTMETHOD_HPP

# include <iostream>
# include <vector>

# include "../../../utils/utils.hpp"

class PostMethod {
private:
	/* data */

public:
	PostMethod ();
	PostMethod(PostMethod const &cpy);
	virtual ~PostMethod ();
	PostMethod &operator=(PostMethod const & cpy);

	bool usePostMethod(std::vector<std::string> const &v);
	int	processPostMethod(std::vector<std::string> const &v);
};

#endif
