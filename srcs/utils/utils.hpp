/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 15:08:47 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 13:29:55 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <sstream>
#include <map>
#include <ctime>
#include <vector>

#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <sys/statvfs.h>
#include <sys/ioctl.h>
#include <math.h>
#include <stdint.h>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <limits>

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

# define NPOS std::string::npos
# define DIM_BAR_SIZE 60

// Rate-Limit
// 10 requetes par 10 secondes, si dépassé, afficher retry-after
# define RATE_LIMIT	10			// nombre de requetes maximum
# define RATE_LIMIT_TIME 10		// temps pour les requetes max

# define IP_LEN 3
// to remove (tmp emplacement post)

# define POST_EMPLACEMENT "webSite/post/result/"

# define BYTE_0 0xff000000
# define BYTE_1 0x00ff0000
# define BYTE_2 0x0000ff00
# define BYTE_3 0x000000ff

//**************************//
//			MACROS			//
//**************************//

# define LOCATIONS(srv, root, code) ::getLocation(srv, root, code)
# define NEW_LOCATIONS(srv) ::getLocations(srv)
# define EXTENSIONS(srv, root) ::getExtensions(srv, root)
# define IS_GCHAR(c) c >= 32 && c <=176

# define POST_HT_TO_STR(type) \
type == 0 ? "" : \
type == 1 ? "form_data" : \
type == 2 ? "application/x-www-form-urlencoded" : \
type == 3 ? "raw" : \
type == 4 ? "binary" \
: "graphql"

//**************************//
//		  TEMPLATES			//
//**************************//

namespace FT {

	template <class Key, class T>
	void 		emplace(std::map<Key, T> &map, std::pair<Key, T> value) {
		map.insert(map.end(), value);
	}
	template <typename T>
	std::string	to_string(T value) {
		std::string result;
		std::ostringstream convert;
		convert << value;
		result = convert.str();
		return (result);
	}
	template <class Key, class Value>
	std::pair<Key, Value>getPair(Key k, Value v) {
		return (std::pair<Key, Value>(k, v));
	}
	template <typename T>
	void LOG(T value) {
		std::ofstream file;
  		file.open ("log.txt");
  		file << value << "\n";
  		file.close();
	}
	template <typename T, typename V>
	bool	contain(T value, V m) {
		if ((std::find(value.begin(), value.end(), m)) != value.end())
			return (true);
		else
			return (false);
	}
	template <typename T>
	size_t	vectorLength(std::vector<T> v) {
		return (v.size());
	}
};

class Server;

//**************************//
//		 	 ENUMS			//
//**************************//

enum E_LocationsSearch {
	LS_LOCATION = 0,
	LS_PAGE = 1,
	LS_ERRORPAGE = 2,
	LS_CGI = 3
};

enum E_PostHeaderType {
	NONE = 0,
	FORM_DATA = 1,
	X_WWW_FORM_URLENCODED = 2,
	RAW = 3,
	BINARY = 4,
	GRAPHQL = 5
};

enum E_SEARCH_PAGE {
	SP_FILE = 0,
	SP_DIRECTORY = 1,
	SP_INVALID = 2
};

//**************************//
//		  STRUCTURES		//
//**************************//

typedef struct	s_custom_time {
	size_t		sec;
	size_t		min;
	size_t		hour;
	size_t		day;
	size_t		month;
	size_t		year;
}				t_custom_time;

typedef struct	s_rate_limit {
	t_custom_time	time;
	size_t			attempt;
}				t_rate_limit;

/*
typedef struct	s_srv_request {
	std::vector<std::string>	requests;
	size_t						fd;
}				t_srv_request;
*/

typedef struct	s_locationNode {
	Server 						*data;
	std::vector<s_locationNode>	next;
	std::vector<s_locationNode> prev; // 1 seul element de vecteur, mais relou de faire avec un pointeur
	std::string					path;
	size_t						depth;
	bool						found;
}				t_locationNode;

typedef struct	s_extensionNode {
	Server		*rootLocation;
	Server		*data;
	bool		found;
}				t_extensionNode;

typedef struct	s_methodResponse {
	size_t		code;
	std::string	content;
	bool		valid;
}				t_methodResponse;

typedef struct	s_Post_Boundary {
	std::string							boundary;
	std::map<std::string, std::string>	boundaryValues;
}				t_Post_Boundary;

typedef struct	s_Post {
	bool							isChunked;
	E_PostHeaderType				contentType;
	std::vector<t_Post_Boundary>	boundary;
	std::string						ressourceLocation;
	std::string						content;

	std::map<std::string, std::string>	formUrlencodedMap;

	bool				isValid;
}				t_Post;

typedef struct	s_Redirect {
	bool		isRedirect;
	std::string redirectPath;
}				t_Redirect;

//**************************//
//			DEBUG			//
//**************************//

namespace DEBUG {
	template <typename T>
	void 	printValue(std::string const &prefix, T value, std::string const &sufix, std::string const &color = NULL) {
		std::cerr << color << prefix << " " << value << " " << sufix << RESET << '\n';
	}
};

// C Functions
void			*ft_memset(void *b, int c, size_t len);
void			*ft_memcpy(void * dst, void * src, size_t n);

// Errors
int				errnoError();
int				errnoSelect();
int				errnoBind();

// Display
void 			displayInfo(std::string const &info, int delay);
void 			displayDimBar(size_t const &clamp);
void			formatInfo(std::string const &name, std::string const &port, std::string const &title, std::string const &info, size_t const space);

// Files / Dir
bool			isDir(std::string const &file);
bool			isFile(std::string const &file);
bool			fileExist(std::string const &file);

// Path Utils
bool			searchingFile(std::string const &file, std::string const &root);

// C++ Extensions
bool			ft_strcmp(std::string const &s1, std::string const &s2);
bool			contain(std::string const &str, std::string const &s);
size_t			nbChar(std::string const &str, char c);
size_t 			nrfind(std::string const &str, std::string const &cmp, size_t n);
size_t			nfind(std::string const &str, std::string const &cmp, size_t n);
std::string 	removeChars(std::string const &str, char c);
std::string 	fixSlash(std::string const &str);
std::string		subcstr(std::string const &str, size_t const start, char const c);
std::string		subsstr(std::string const &str, std::string const &start, std::string const &end, bool const startAtEndStart);
std::vector<std::string>	strToTab(std::string const &str, char del);
std::string		cleanResize(std::string const &str);
std::string		getFileExtension(std::string const &file);
int				stoi_dec(std::string const &str, size_t const base);
int 			stoi_base(std::string const &str, size_t const base);
bool			sIsnum(std::string const &str);

// Locations global Utils
bool			locationIsValid(t_locationNode node);
struct tm 		*addTime(struct tm *tm, size_t seconds);

// Disk util
unsigned long long int getFreeDiskBytes();

// Server
unsigned int	strToIP(std::string const &ip);
int 			ft_stohl(std::string const &sip);

// CGI Util
char **convertMapToCGIEnv(std::map<std::string, std::string> &env);
void	free_tab(char **tab);

#endif
