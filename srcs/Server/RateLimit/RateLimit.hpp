/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RateLimit.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvaucoul <vvaucoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/22 17:03:41 by vvaucoul          #+#    #+#             */
/*   Updated: 2021/09/20 12:54:40 by vvaucoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RATE_LIMIT_HPP
# define RATE_LIMIT_HPP

# include "../../utils/utils.hpp"

# define CURRENT_TIME getCurrentTime()
# define TIME_TO_STR(time) FT::to_string(std::ctime(&time))

# define MAKE_RATE_LIMIT(attempt) (t_rate_limit){convertTimeToCustomTime(CURRENT_TIME), attempt}
# define NULL_RATE_LIMIT (t_rate_limit){NULL, 0}

# define TIME_DEFAULT_YEAR 1900

class RateLimit {
private:

	std::time_t									_currentTime;
	std::map<int, std::vector<t_rate_limit> >	_rateLimit;

	t_rate_limit								initRateLimit(int socket);
	bool										shouldResetAttempt(int socket);
	void										printTime(t_custom_time ctm);
	void										printAllRateLimit(int socket);

public:
	RateLimit ();
	virtual ~RateLimit ();
	RateLimit(const RateLimit &rlm);
	RateLimit &operator=(const RateLimit &rlm);

	void 						addRateLimit(int socket);
	std::vector<t_rate_limit>	getRateLimit(int socket);
	t_rate_limit				getLastRateLimit(int socket);
	t_rate_limit				accessRateLimit(int socket, int index);
	bool						rateLimitAsBeenExceded(int socket);
};

inline struct tm *getCurrentTime() {
	std::time_t t = std::time(0);
	struct tm *n = std::localtime(&t);
	return (n);
}

inline t_custom_time convertTimeToCustomTime(struct tm *t) {
	t_custom_time ct;

	ct.sec = t->tm_sec;
	ct.min = t->tm_min;
	ct.hour = t->tm_hour;
	ct.day = t->tm_mday;
	ct.month = t->tm_mon;
	ct.year = TIME_DEFAULT_YEAR + t->tm_year;
	return (ct);
}

#endif
