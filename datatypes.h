#ifndef TIME_H
#define TIME_H

#include <string>
#include <chrono>
#include <tuple>
#include <Wt/WRegExpValidator>

using std::string;

class Time {
public:
	Time();
	Time(int arg);
	Time(string arg);

	static Time	  					current_time	(void);
	static Wt::WRegExpValidator*	validator		(void);
	string 		  					to_string		(void);
	int			  					to_int			(void);

	int time;
};

class Duration {
public:
	Duration(string arg);
	Duration(int arg);

	static Wt::WRegExpValidator*	validator	(void);
	string 		    				to_string	(void);
	int								to_int		(void);

	int duration;
};

class Multiplier {
public:
	Multiplier(string arg);
	Multiplier(double arg);

	static Wt::WRegExpValidator*	validator	(void);
	string 							to_string	(void);
	double							to_double	(void);

	double multiplier;
};

std::tuple<Time,Time,bool> sunriset_today	(void);
int zone_utc_plus	(void);

using std::chrono::system_clock;
system_clock::time_point until_next_second(void);

#endif //TIME_H
