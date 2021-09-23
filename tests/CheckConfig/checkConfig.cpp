/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checkConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/19 22:56:48 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 10:16:25 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "checkConfig.hpp"

int	test(std::vector<std::string> &v) {
	for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it) {
		std::cout << RESET CYAN << "\n- Check: " << GREEN << "[" << *it << "]" RESET << '\n';
		system(std::string("./webserv " + *it + " > /dev/null").c_str());
		std::cout << "\n" << '\n';
		usleep(500000);
	}
	return (0);
}

int main(int argc, char const *argv[]) {

	system("clear");
	std::cout << "This is a simple test of all webserv config file possibilities" << '\n';
	sleep(1);
	std::cout << "Compilation..." << '\n';
	system("make -C ../../ && cp ../../webserv .");


	DIR *dir = opendir("./badConfigs");
	struct dirent *entry;
	int files = 0;
	std::vector<std::string> v;

	if (!dir) {
		std::cerr << strerror(errno) << '\n';
		return (1);
	}
	while( (entry=readdir(dir)) )
	{
		std::string filename(entry->d_name);

		if (filename == "." || filename == "..")
			continue;
		v.push_back("badConfigs/" + filename);
		files++;
		usleep(50000);
	}
	std::sort(v.begin(), v.end());
	for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it) {
		std::cout << CYAN BOLD << "- Add file: " << *it << RESET << '\n';
		usleep(50000);
	}

	closedir(dir);
	sleep(1);
	test(v);
	return (0);
}
