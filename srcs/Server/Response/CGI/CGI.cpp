/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/29 19:33:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 13:00:31 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

//**************************//
//			DEFAULT			//
//**************************//

CGI::CGI(Request *request, Server *srv, Server *rootSrv, std::string const &filePath, t_Post *postRequest) {
	_request = request;
	_srv = srv;
	_rootSrv = rootSrv;
	_filePath = filePath;
	_postRequest = postRequest;

	_code = 200;
	_checker = new HtmlChecker(_filePath, "/");

	init();
	process();
	parse();
	delete _checker;
}
CGI::~CGI() {
}
CGI::CGI(CGI const &cpy) {
	*this = cpy;
}
CGI &CGI::operator=(CGI const &cpy) {
	if (this == &cpy)
		return (*this);
	else {
		_request = cpy._request;
		_srv = cpy._srv;
		_rootSrv = cpy._rootSrv;
		_filePath = cpy._filePath;
		_postRequest = cpy._postRequest;
		_env = cpy._env;
		_result = cpy._result;
		_code = cpy._code;
		_header = cpy._header;
		_content = cpy._content;
	}
	return (*this);
}

//**************************//
//			DEFAULT			//
//**************************//

void CGI::init() {

	// env Creation
	{
		std::string rootedFile = "";
		std::string file = "";
		std::string contentType = "";
		std::string body = "";
		std::string hostname = "";

		{
 			rootedFile = !isFile(_request->getConfig()["Page"]) ? fixSlash(_rootSrv->getRoot() + "/" + _request->getConfig()["Page"] + "/" +
			NEW_LOCATIONS(_srv).searchLocation(_request->getConfig()["Page"]).data->getHTMLPath())
			: fixSlash(_rootSrv->getRoot() + "/" + _request->getConfig()["Page"]);

			if (rootedFile[rootedFile.size() - 1] == '/')
				rootedFile.erase(rootedFile.size() - 1);

			file = !isFile(_request->getConfig()["Page"]) ? _request->getConfig()["Page"] + "/" +
			NEW_LOCATIONS(_srv).searchLocation(_request->getConfig()["Page"]).data->getHTMLPath()
			 : _request->getConfig()["Page"];

			 if (file[file.size() - 1] == '/')
			 	file.erase(file.size() - 1);
		}

		if (_postRequest) {
			contentType = POST_HT_TO_STR(_postRequest->contentType);
			body = _postRequest->content;
		} else {
			contentType = POST_HT_TO_STR((E_PostHeaderType)NONE);
			body = _checker->getContentFile();
		}
		if (!_request->getConfig()["Hostname"].empty()) {
			hostname = _request->getConfig()["Hostname"];
		} else {
			hostname = "0";
		}
		_env.insert(_env.end(), FT::getPair("REDIRECT_STATUS", "200"));
		_env.insert(_env.end(), FT::getPair("GATEWAY_INTERFACE", "CGI/1.1"));
		_env.insert(_env.end(), FT::getPair("SCRIPT_NAME", rootedFile));
		_env.insert(_env.end(), FT::getPair("SCRIPT_FILENAME", rootedFile));
		_env.insert(_env.end(), FT::getPair("REQUEST_METHOD", _request->getConfig()["Method"]));
		_env.insert(_env.end(), FT::getPair("CONTENT_LENGTH", FT::to_string(body.length())));
		_env.insert(_env.end(), FT::getPair("CONTENT_TYPE", contentType));
		_env.insert(_env.end(), FT::getPair("PATH_INFO", file));
		_env.insert(_env.end(), FT::getPair("PATH_TRANSLATED", file));
		_env.insert(_env.end(), FT::getPair("QUERY_STRING", ""));
		_env.insert(_env.end(), FT::getPair("REMOTEaddr", "0"));
		_env.insert(_env.end(), FT::getPair("REMOTE_IDENT", _request->getConfig()["Authorization"]));
		_env.insert(_env.end(), FT::getPair("REMOTE_USER", ""));
		_env.insert(_env.end(), FT::getPair("REQUEST_URI", file));
		_env.insert(_env.end(), FT::getPair("SERVER_NAME", hostname));
		_env.insert(_env.end(), FT::getPair("SERVER_PORT", FT::to_string(_rootSrv->getPort())));
		_env.insert(_env.end(), FT::getPair("SERVER_PROTOCOL", "HTTP/1.1"));
		_env.insert(_env.end(), FT::getPair("SERVER_SOFTWARE", _rootSrv->getName()));
	}
	printEnv();
}
void CGI::process() {
	pid_t	pid;
	int		fd_in, fd_out, inBck, outBck;
	FILE	*tf1, *tf2;
	std::string body = _postRequest ? _postRequest->content : _checker->getContentFile();
	std::string scriptName = _srv->getCGIPath();

	if (body.empty() || scriptName.empty())
		throw CGIError500();

	fd_in = fileno(tf1 = tmpfile());
	fd_out = fileno(tf2 = tmpfile());

	inBck = dup(STDIN_FILENO);
	outBck = dup(STDOUT_FILENO);

	dup2(fd_in, STDIN_FILENO);
	dup2(fd_out, STDOUT_FILENO);

	write(fd_in, body.c_str(), body.size());
	lseek(fd_in, 0, SEEK_SET);

	pid = fork();
	if (pid < 0) {
		std::cerr << RED << '\n';
		perror("CGI, fork");
		std::cerr << RESET << '\n';
		return ;
	}
	else if (!pid) {
		char **cenv = convertMapToCGIEnv(_env);
		char * const * nll = NULL;

		dup2(fd_in, STDIN_FILENO);
		dup2(fd_out, STDOUT_FILENO);
		if (scriptName.find("python") != std::string::npos)
		{
			std::string path = fixSlash("./" + _rootSrv->getRoot() + "/" + _env["PATH_INFO"]);
			std::string b = "QUERY_STRING=" + body;
			char *py[3] = {
				(char *)(path.c_str()),
				(char *)(b.c_str()),
				NULL};
			execve(scriptName.c_str(), nll, py);
			std::cerr << RED << "Execve crashed." << RESET << std::endl;
			write(STDOUT_FILENO, "Status: 500\r\n\r\n", 15);
		}
		else
		{
			execve(scriptName.c_str(), nll, cenv);
			std::cerr << RED << "Execve crashed." << RESET << std::endl;
			write(STDOUT_FILENO, "Status: 500\r\n\r\n", 15);
		}
		free_tab(cenv);
		exit (0);
	} else {
		int ret;

		waitpid(pid, &ret, 0);

		if (WEXITSTATUS(ret) == -1 || WEXITSTATUS(ret) == 255) {
			perror("CGI, Execve");
			return ;
		} else {
			char buffer[42] = {0};

			waitpid(-1, NULL, 0);
			lseek(fd_out, 0, SEEK_SET);

			ret = 1;
			while (ret > 0)
			{
				memset(buffer, 0, 42);
				ret = read(fd_out, buffer, 42 - 1);
				_result += buffer;
			}
		}
	}
	dup2(inBck, STDIN_FILENO);
	dup2(outBck, STDOUT_FILENO);

	close(fd_in);
	close(fd_out);
	close(inBck);
	close(outBck);

	fclose(tf1);
	fclose(tf2);

}
void CGI::parse() {
	if (_result.empty()) {
		std::cerr << "CGI Content empty" << '\n';
		_code = 500;
		delete _checker;
		throw CGIContentException();
	} else if (_result == "Status: 500\r\n\r\n") {
		std::cerr << "Error CGI, code 500" << '\n';
		_code = 500;
		delete _checker;
		throw CGIError500();
	} else {
		_result = removeChars(_result, '\r');
		std::size_t found = _result.find("\n\n");

		if (found == NPOS) {
			return ;
		}
		std::string header(_result, 0, found + 1);
		_result = std::string(_result, found + 2);

		while ((found = header.find(": ")) != std::string::npos)
		{
			std::string key = std::string(header, 0, found);
			std::string value = std::string(header, found + 1, header.find("\n") - (found + 1));
			_header.insert(std::pair<std::string, std::string>(key, value));
			header = std::string(header, header.find("\n") + 1);
		}
		if (atoi(_header["Status"].c_str()))
			_code = atoi(_header["Status"].c_str());
	}
}

void CGI::printEnv() {

	std::cout << YELLOW << "### CGI ENV ###" << RESET << '\n';

	std::cout << CYAN DIM << "-----------------------------" << RESET << '\n';
	for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); ++it) {
		std::cout << GREEN << "<" << (*it).first << ">" << CYAN << "\r\t\t\t\t" << (*it).second << '\n';
	}
	std::cout << CYAN DIM << "-----------------------------" << RESET << '\n';
	std::cout << "\n\n";
}
