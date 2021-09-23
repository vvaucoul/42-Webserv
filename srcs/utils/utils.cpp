/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/21 17:40:37 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 13:29:47 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

//**************************//
//			UTILS			//
//**************************//

void *ft_memset(void *b, int c, size_t len) {
	unsigned char	*t;

	t = (unsigned char *)b;
	while (len--)
		*(t++) = (unsigned char)c;
	return (b);
}
void *ft_memcpy(void *dest, void *src, size_t n) {
	unsigned char	*ptr_dest;
	unsigned char	*ptr_src;
	if (!dest && !src)
		return (NULL);
	ptr_dest = (unsigned char*)dest;
	ptr_src = (unsigned char*)src;
	while (n--)
		*(ptr_dest++) = *(ptr_src++);
	return (dest);
}
bool ft_strcmp(std::string const &s1, std::string const &s2) {
	if (s1.size() != s2.size())
		return (false);
	else {
		for (size_t i = 0; i < s1.size(); ++i) {
			if (s1[i] != s2[i])
			 	return (false);
		}
	}
 	return (true);
}
bool contain(std::string const &str, std::string const &s) {
	if (str.find(s) != NPOS
	|| s.find(str) != NPOS)
		return (true);
	else
		return (false);
}
size_t nbChar(std::string const &str, char c) {
	if (str.empty())
		return (0);
	size_t nb = 0;
	for (size_t i = 0; i < str.length(); i++) {
		if (str[i] == c)
			++nb;
	}
	return (nb);
}
size_t nrfind(std::string const &str, std::string const &cmp, size_t n) {
	size_t i = str.length();
	size_t pos = 0;

	while (pos < n) {
		while (i > 0 && strncmp(&str.c_str()[i], cmp.c_str(), cmp.length())) {
			--i;
		}
		if (i <= 0)
			return (0);
		--i;
		++pos;
	}

	return (i + 1);
	/*

	size_t k = 0;
	size_t i = str.length();

	while (k < n) {
		while (i > 0 && strncmp(&(str.c_str())[i], cmp.c_str(), cmp.length())) {
			--i;
		}
		++k;
		--i;
		if (i <= 0)
			return (0);
	}
	return (i + 1);
	*/

}
size_t nfind(std::string const &str, std::string const &cmp, size_t n) {
	size_t pos = 0;
	size_t i = 0;

	while (i < n) {
		pos = str.find_first_of(cmp, pos) + 1;
		++i;
	}
	return (pos);
}
std::string removeChars(std::string const &str, char c) {
	std::string nstr;
	size_t j = 0;

	nstr.resize(str.length(), sizeof(char));
	for (size_t i = 0; i < str.length(); i++) {
		if (str[i] != c) {
			nstr[j] = str[i];
			++j;
		}
	}
	nstr[j] = 0;
	nstr.resize(j, sizeof(char));
	return (nstr);
}
std::string fixSlash(std::string const &str) {
	std::string ret = "";
	for (size_t i = 0; i < str.size(); i++) {
		if (str[i] == '/' && str[i + 1] == '/')
			continue;
		else
			ret += str[i];
	}
	return (ret);
}
std::string subcstr(std::string const &str, size_t const start, char const c) {
	return (str.substr(start, (str.find_first_of(c, start) - start)));
}
std::string	subsstr(std::string const &str, std::string const &start, std::string const &end, bool const startAtEndStart) {
	size_t s_start = start.empty() ? 0 : str.find(start) + (startAtEndStart ? start.length() : 0);
	size_t s_end = end.empty() ? str.length() : str.find(end);
	return (str.substr(s_start, s_end - s_start));
}
std::vector<std::string> strToTab(std::string const &str, char del) {
	std::vector<std::string> v;

	size_t pos = 0;
	size_t prev = 0;

	do {
		pos = str.find_first_of(del, prev);
		std::string current = str.substr(prev, pos - prev);
		if (!current.empty())
			v.push_back(current);
		prev = pos + 1;
	} while(pos != NPOS);

	return (v);
}
std::string cleanResize(std::string const &str) {
	std::string n;

	for (size_t i = 0; str[i] && i < str.length(); i++) {
		if (IS_GCHAR((int)str[i])) {
			n += str[i];
		}
	}

	//std::cout << "cleanResize ret: " << n << '\n';
	//std::cout << "cleanResize ret size: " << n.size() << '\n';
	//std::cout << "cleanResize ret len: " << n.length() << '\n';

	return (n);
}
std::string	getFileExtension(std::string const &file) {
	if (contain(file, ".")) {
		return (file.substr(file.rfind(".") + 1, file.length() - file.rfind(".") - 1));
	}
	else
		return ("");
}
int stoi_base(std::string const &str, size_t const base) {
	std::string refB = "0123456789abcdef";
	bool isNeg;
	int nb, i;
	std::string nbBase;

	nb = atoi(str.c_str());
	if (nb < 0)
		isNeg = true;
	else
		isNeg = false;
	i = 0;
	while (nb) {
		nbBase += refB[nb % base];
		nb /= base;
		++i;
	}
	std::reverse(nbBase.begin(), nbBase.end());
	return (atoi(nbBase.c_str()));
}
int	stoi_dec(std::string const &str, size_t const base){
	size_t ret = 0;

	if (base < 2 || base > 16)
		return (atoi(str.c_str()));

	size_t power = str.length() - 1;
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
		size_t add;
		add = ((*it) - 48) * pow(base, power);
		ret += add;
		--power;
	}
	return (ret);
}
bool sIsnum(std::string const &str) {
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
		if (!isdigit(*it))
			return (false);
	}
	return (true);
}

//**************************//
//			DISPLAY			//
//**************************//

void displayInfo(std::string const &info, int delay) {
	for (size_t i = 0; i < info.length(); i++) {
		std::cout << info.c_str()[i];
		usleep(delay);
	}
}
void formatInfo(std::string const &name, std::string const &port, std::string const &title, std::string const &info, size_t const space) {
	for (size_t i = 0; i < space; i++) {
		std::cout << "  ";
	}
	std::cout << RESET << DIM << "L-> " << RESET << BOLD << \
	(!name.empty() ? "[" : "") << name << (!name.empty() ? "]" : "") << \
	((!port.empty()) ? "[" : "") << port << ((!port.empty()) ? "]" : "") << \
	CYAN << title << RESET << info << RESET << '\n';

	usleep(5000);
}
void displayDimBar(size_t const &clamp) {
	size_t nc = clamp;
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);

	if (nc <= 0)
		nc = w.ws_col;
	else if (nc > w.ws_col)
		nc = w.ws_col;
	std::cout << RESET << DIM << "\n";
	for (size_t i = 0; i < nc; i++) {
		std::cout << "#";
	}
	std::cout << "\n\n" << RESET;
}

//**************************//
//			ERRORS			//
//**************************//

int		errnoError() {
	std::cerr << errno << '\n';
	return (0);
}
int		errnoSelect() {
	switch (errno) {
		case EBADF:
			std::cerr << "Invalid File Descriptor" << '\n';
			return (1);
		case EINTR:
			std::cerr << "Signal intercepted" << '\n';
			return (1);
		case EINVAL:
			std::cerr << "nfds < 0 or timeout invalid" << '\n';
			return (1);
		case ENOMEM:
			std::cerr << "Not enough Core memory" << '\n';
			return (1);
	}
	return (0);
}
int		errnoBind() {
	switch (errno) {
		case EACCES :
		std::cout << "L'adresse est protégée et l'utilisateur n'est pas le superutilisateur." << std::endl;
		return (1);
		case EADDRINUSE :
		std::cout << "L'adresse fournie est déjà utilisée." << std::endl;
		return (1);
		case EBADF :
		std::cout << "sockfd n'est pas un descripteur valide." << std::endl;
		return (1);
		case EINVAL :
		std::cout << "La socket est déjà liée à une adresse." << std::endl;
		return (1);
		case ENOTSOCK :
		std::cout << "sockfd est un descripteur de fichier, pas une socket." << std::endl;
		return (1);
	}
	return (0);
}

//**************************//
//			FILE			//
//**************************//

bool	isDir(std::string const &file) {
	struct stat st;


	if (file.empty())
		return (false);
	int ret = stat(file.c_str(), &st);

	if (ret < 0) {
		return (false);
	}

	return (S_ISDIR(st.st_mode));
}
bool	isFile(std::string const &file) {
	struct stat st;

	if (file.empty())
		return (false);
	int ret = stat(file.c_str(), &st);
	if (ret < 0) {
		return (false);
	}
	return (S_ISREG(st.st_mode));
}
bool	fileExist(std::string const &file) {
	struct stat st;

	if (file.empty())
		return (false);
	int ret = stat(file.c_str(), &st);

	if (ret < 0) {
		return (false);
	}
	return (true);
}

//**************************//
//			PATH			//
//**************************//

bool	searchingFile(std::string const &file, std::string const &root) {
	if (*(file.end() - 1) == '/')
		return (false);
	if (nbChar(file, '/') >= 1) {

		// recherche si le dernier element du path est un *.*
		std::string last = file.substr(nrfind(file, "/", 1), file.length());
		if (FT::contain(last, '.')) {
			return (true);
		}

		// sinon, cherche s'il existe un fichier avec le path file
		{
			if (isFile(fixSlash(root + "/" + file))) {
				return (true);
			}
			else
				return (false);
		}
	}
	else
		return (false);
	return (false);
}

//**************************//
// LOCATIONS - GLOBAL UTILS //
//**************************//

bool	locationIsValid(t_locationNode node) {
	if (!node.found)
		return (false);
	else if (!node.data || node.path.empty())
		return (false);
	else
		return (true);
}

struct tm *addTime(struct tm *tm, size_t seconds) {
	tm->tm_sec += seconds;
	if (tm->tm_sec > 60) {
		tm->tm_sec %= 60;
		tm->tm_min += 1;
	}
	if (tm->tm_min > 60) {
		tm->tm_min %= 60;
		tm->tm_hour += 1;
	}
	if (tm->tm_hour > 24) {
		tm->tm_hour %= 24;
		tm->tm_mday += 1;
	}
	if (tm->tm_mday > 31) {
		tm->tm_mday %= 31;
		tm->tm_mon += 1;
	}
	if (tm->tm_year > 2050) {
		tm->tm_year = 1900;
	}
	return (tm);
}

//**************************//
// 		     DISK 			//
//**************************//

unsigned long long int getFreeDiskBytes() {
	/*
	struct statvfs fiData;
	struct statvfs *fpData;
	char fnPath[128];

	strcpy(fnPath, "/dev/disk2");
	if((statvfs(fnPath,&fiData)) < 0 ) {
		printf("Failed to stat %s:\n", fnPath);
	} else {
		printf("Disk %s: \n", fnPath);
		printf("\tblock size: %lu\n", fiData.f_bsize);
		printf("\ttotal no blocks: %i\n", fiData.f_blocks);
		printf("\tfree blocks: %i\n", fiData.f_bfree);
	}
	*/
	return (0);
}

//**************************//
// 		     SERVER			//
//**************************//

static bool			ipIsValid(std::string const &ip) {
	if (nbChar(ip, '.') != 3)
		return (false);
	for (std::string::const_iterator it = ip.begin(); it != ip.end(); ++it) {
		if (*it == '.')
			continue;
		else if (isdigit(*it))
			continue;
		else
			return (false);
	}
	return (true);
}

unsigned int	strToIP(std::string const &ip) {
	std::string		nip = ip;
	size_t			separator = 0;
	unsigned int	n;
	unsigned char	m[4];
	size_t			start = 0;
	bool 			invalidIP = ipIsValid(ip);

	if (nip == "localhost")
		nip = "127.0.0.1";
	else if (!invalidIP)
		nip = "0.0.0.0";
	for (unsigned int i = IP_LEN; i != std::numeric_limits<uint32_t>::max(); i--) {
		separator = nip.find_first_of('.', separator);
		std::string curr = nip.substr(start, separator);
		n = atoi(curr.c_str());
		m[i] = static_cast<unsigned char>(n);
		++separator;
		start = separator;
	}
	unsigned unsignedIP = *(reinterpret_cast<unsigned int *>(m));
	return (unsignedIP);
}

static unsigned int	ft_bswap32(unsigned int x)
{
	x = (x & BYTE_0) >> 24 | (x & BYTE_3) << 24 |
		(x & BYTE_1) >> 8  | (x & BYTE_2) << 8;
	return (x);
}


int ft_stohl(std::string const &sip) {
	unsigned int ip;

	ip = strToIP(sip);
	if (BYTE_ORDER == BIG_ENDIAN)
		return (ip);
	else if (BYTE_ORDER == LITTLE_ENDIAN)
		return (ft_bswap32(ip));
	return (0);
}

//**************************//
// 		     CGI 			//
//**************************//

char **convertMapToCGIEnv(std::map<std::string, std::string> &env) {
	char **nenv = NULL;
	size_t i = 0;
	size_t maxLen = 0;

	for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); ++it) {
		size_t len = it->second.length();
		if (len > maxLen)
			maxLen = len;
	}

	if (!(nenv = (char **)malloc(sizeof(char *) * (env.size() * maxLen) + 1)))
		return (NULL);
	for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); ++it) {
		std::string curr = (*it).first + "=" + (*it).second;
		nenv[i] = (strdup(curr.c_str()));
		++i;
	}
	nenv[i] = NULL;
	return (nenv);
}
void	free_tab(char **tab) {
	for (size_t i = 0; tab[i]; i++) {
		free (tab[i]);
	}
}
