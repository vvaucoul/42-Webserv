/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChunkProcess.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 19:09:23 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/15 17:14:58 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHUNK_PROCESS_HPP
#define CHUNK_PROCESS_HPP

# include <iostream>
# include "../../../utils/utils.hpp"

class ChunkProcess {
private:
	std::string					_result;

	unsigned long int			_originalSize;
	unsigned long int			_resultSize;

	bool						_error;

private:
	std::string parseChunk(std::string const &chunk);

public:
	ChunkProcess ();
	ChunkProcess (ChunkProcess const &cpy);
	ChunkProcess &operator=(ChunkProcess const &cpy);
	virtual ~ChunkProcess ();

	void AddChunk(std::string const &chunk);
	std::string getResult();

	unsigned long int getResultSize() {
		return (_resultSize);
	}
	unsigned long int getOriginalSize() {
		return (_originalSize);
	}
	bool getError() {
		return (_error);
	}
};


#endif /* end of include guard:CHUNK_PROCESS */
