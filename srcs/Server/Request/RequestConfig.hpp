/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestConfig.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mle-faou <mle-faou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/22 14:41:55 by mle-faou          #+#    #+#             */
/*   Updated: 2021/09/20 12:56:09 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTCONFIG_HPP
#define REQUESTCONFIG_HPP

#include <string>
#include <iostream>
#include <map>

# include "../../utils/utils.hpp"
# include "ChunkProcess/ChunkProcess.hpp"

# define REQUEST_METHOD _config["Method"]

class RequestConfig
{
private:

	ChunkProcess						_cp;
	std::map<std::string, std::string>	_config;
	std::string							_content;

	bool								_invalidHeader;
	bool								_invalidContent;

	//									Multiple Request (with post content-lengh < current content)
	bool								_isWaitingForContent;

public:
	RequestConfig(void);
	RequestConfig(RequestConfig const &cpy);
	RequestConfig &operator=(RequestConfig const &cpy);
	RequestConfig(std::string request);
	~RequestConfig(void);

	void printRequestConfig(void);

	std::map<std::string, std::string> getRequestConfig(void) {
		return (_config);
	}
	std::string getRequestContent(void) {
		return (_content);
	}
	bool isInvalidHeader(void) {
		return (_invalidHeader);
	}
	bool isInvalidContent() {
		return (_invalidContent);
	}
	bool isWaitingForContent(void) {
		return (_isWaitingForContent);
	}

// ChunkProcess
public:
	ChunkProcess getChunkProcess(void) {
		return (_cp);
	}
	unsigned long int getCPOriginalSize(void) {
		return (_cp.getOriginalSize());
	}

// Utils
public:
	void ParseContent(std::string request);

public:
	std::string &operator[](std::string const &key);
};

#endif
