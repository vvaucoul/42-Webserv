/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChunkProcess.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/03 19:09:24 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 12:56:43 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "ChunkProcess.hpp"

//**************************//
//			DEFAULT			//
//**************************//

ChunkProcess::ChunkProcess() {
	_result = "";
	_originalSize = 0;
	_resultSize = 0;
	_error = false;
}
ChunkProcess::~ChunkProcess() {

}
ChunkProcess::ChunkProcess(ChunkProcess const &cpy) {
	*this = cpy;
}
ChunkProcess &ChunkProcess::operator=(ChunkProcess const &cpy) {
	if (this == &cpy) {
		return (*this);
	}
	_result = cpy._result;
	_originalSize = cpy._originalSize;
	_resultSize = cpy._resultSize;
	_error = cpy._error;
	return (*this);
}

//**************************//
//		 CHUNK-PROCESS		//
//**************************//

void ChunkProcess::AddChunk(std::string const &chunk) {
	_result = parseChunk(chunk);
}
std::string ChunkProcess::parseChunk(std::string const &chunk) {

	std::string nChunk = chunk;
	std::string res;
	std::vector<std::string> v;
	size_t		size = 0;
	std::string parse = "";
	int currentParsing = 0;

	nChunk = removeChars(chunk, '\r');
	v = strToTab(nChunk, '\n');
	for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it) {
		std::string line = (*it);

		line = removeChars(line, '\r');
		line = removeChars(line, '\n');

		if (!currentParsing) {
			if (!(sIsnum(line))) {
				_error = true;
				break;
			}
			size = stoi_dec(line, 16);
			if (size == 0)
				break;
			_resultSize += size;
		} else {
			if (line.size() > size) {
				_error = true;
				break;
			}
			parse = line.substr(0, size);
			parse += "\r\n";

			res += parse;
		}
		++currentParsing;
		if (currentParsing > 1)
			currentParsing = 0;
	}
	return (res);
}
std::string ChunkProcess::getResult() {
	_result += "\r\n\n\r";
	return (_result);
}
