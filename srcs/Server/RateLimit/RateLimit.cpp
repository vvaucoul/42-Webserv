/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RateLimit.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/22 17:05:18 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 12:54:29 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "RateLimit.hpp"

//**************************//
//			DEFAULT 		//
//**************************//

RateLimit::RateLimit() {
	_currentTime = std::time(NULL);
}
RateLimit::~RateLimit() {
	_rateLimit.clear();
}
RateLimit::RateLimit(const RateLimit &rlm) {
	*this = rlm;
}
RateLimit &RateLimit::operator=(const RateLimit &rlm) {
	if (this == &rlm)
		return (*this);
	_rateLimit = rlm._rateLimit;
	_currentTime = rlm._currentTime;
	return (*this);
}

//**************************//
//		  RATE LIMIT 		//
//**************************//

void 						RateLimit::addRateLimit(int socket) {
	(void)socket;
	_rateLimit[socket].push_back(MAKE_RATE_LIMIT(_rateLimit.empty() ? 1 : _rateLimit.size() + 1));
	bool sra = shouldResetAttempt(socket);
	if (sra) {
		_rateLimit.clear();
		_rateLimit[socket].push_back(MAKE_RATE_LIMIT(1));
	}
}
std::vector<t_rate_limit>	RateLimit::getRateLimit(int socket) {
	return (_rateLimit[socket]);
}
t_rate_limit				RateLimit::accessRateLimit(int socket, int index) {
	return (_rateLimit[socket][index]);
}
t_rate_limit				RateLimit::getLastRateLimit(int socket) {
	return (_rateLimit[socket].back());
}
bool						RateLimit::rateLimitAsBeenExceded(int socket) {
	(void)socket;
	struct tm *ctm;
	t_custom_time ltm;
	t_rate_limit current = getLastRateLimit(socket);

	// current Time
	{
		ctm = CURRENT_TIME;
	}
	// Last Request Time
	{
		ltm = current.time;
	}

	unsigned long long int cr_ctm = (ctm->tm_min * 60) + (ctm->tm_hour * 3600) + (ctm->tm_mday * 86400) + ctm->tm_sec;
	unsigned long long int cr_ltm = (ltm.min * 60) + (ltm.hour * 3600) + (ltm.day * 86400) + ltm.sec;

	if (cr_ctm - cr_ltm < RATE_LIMIT_TIME) {
		//exceded time
		if (current.attempt >= RATE_LIMIT) {
			// exceded time & attempt
			return (true);
		}
	}
	else {
		return (false);
	}
	return (false);
}

//**************************//
//	 RATE LIMIT - PRIVATE	//
//**************************//

bool												RateLimit::shouldResetAttempt(int socket) {
	struct tm		*ctm;
	t_custom_time	ltm;

	// get current time
	{
		ctm = CURRENT_TIME;
	}

	// get last t_rate_limit time
	{
		if (_rateLimit.size() <= 1)
			return (false);
		ltm = _rateLimit[socket][_rateLimit.size() - 2].time;
	}

	unsigned long long int cr_ctm = (ctm->tm_min * 60) + (ctm->tm_hour * 3600) + (ctm->tm_mday * 86400) + ctm->tm_sec;
	unsigned long long int cr_ltm = (ltm.min * 60) + (ltm.hour * 3600) + (ltm.day * 86400) + ltm.sec;

	if ((size_t)ctm->tm_hour != ltm.hour
		|| (size_t)ctm->tm_mday != ltm.day
		|| (size_t)ctm->tm_mon != ltm.month) {
			return (false);
		}
	else {
		if (cr_ctm - cr_ltm > RATE_LIMIT_TIME)
			return (true);
	}
	return (false);
}
t_rate_limit										RateLimit::initRateLimit(int socket) {
	t_rate_limit rlt;

	(void)socket;
	rlt = (t_rate_limit){convertTimeToCustomTime(CURRENT_TIME), 0};
	return (rlt);
}
void												RateLimit::printTime(t_custom_time ctm) {
	std::cout << "\t- Time: " << '\n';
	std::cout << "\t  - Day: " << ctm.day << '\n';
	std::cout << "\t  - Hour: " << ctm.hour << '\n';
	std::cout << "\t  - Min: " << ctm.min << '\n';
	std::cout << "\t  - Sec: " << ctm.sec << '\n';
}
void												RateLimit::printAllRateLimit(int socket) {
	size_t i = 0;
	for (std::vector<t_rate_limit>::iterator it = _rateLimit[socket].begin(); it != _rateLimit[socket].end(); ++it) {
		std::cout << DIM;
		std::cout << "\t- Index: " << i << '\n';
		std::cout << "\t- Attempt: " << (*it).attempt << '\n';
		printTime((*it).time);
		++i;
		std::cout << "\n";
	}
	std::cout << RESET;
}
