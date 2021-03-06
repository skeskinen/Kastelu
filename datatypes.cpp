#include "datatypes.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip> 
#include <cstdlib>
#include "sunriset.h"

Time::Time(string arg)
{
	int i;
	char c;
	std::stringstream ss(arg);

	time = 0;
	ss >> i >> c;
	time += i * 60 * 60;
	ss >> i;
	time += i * 60;
}

Time::Time(int arg)
{
	time = arg;
}

Time::Time()
{
	time = 0;
}

int Time::to_int()
{
	return time;
}

string Time::to_string(void)
{
	std::stringstream ss;
	ss.fill('0');
	ss << std::setw(2) << time/60/60;
	ss << ":" << std::setw(2) << ((time/60)%60);
	return ss.str();
}

Time Time::current_time(void)
{
	Time t;
	std::time_t now;
	now = std::time(0);
	
	std::tm midnight = *localtime(&now);
	midnight.tm_hour = 0; midnight.tm_min = 0; midnight.tm_sec = 0;

	t.time = (int) std::difftime(now, std::mktime(&midnight));
	return t;
}

system_clock::time_point until_next_second(void)
{
	return system_clock::from_time_t(time(0)+2);
}

Duration::Duration(string arg)
{
	int i;
	char c;
	int sign = 0;
	std::stringstream ss(arg);

	duration = 0;
	ss >> i >> c;
	while(ss.good())
	{
		switch(c) {
			case 'h':
				duration += i * 60 * 60;
				break;
			case 'm':
				duration += i * 60;
				break;
			case 's':
				duration += i;
				break;
		}
		ss >> i >> c;
		if(sign == 0){
			if(duration > 0) sign = 1;
			else sign = -1;
			duration = abs(duration);
		}
	}
	duration = duration * sign;
}

Duration::Duration(int arg)
{
	duration = arg;
}

string Duration::to_string()
{
	if(duration == 0)
		return "0s ";
	int dur = abs(duration);
	std::stringstream ss;
	if(duration < 0)
		ss << "-";
	int hours = dur/60/60;
	int minutes = (dur/60)%60;
	int seconds = dur % 60;
	if(hours > 0)
		ss << hours << "h ";
	if(minutes > 0)
		ss << minutes << "m ";
	if(seconds > 0)
		ss << seconds << "s ";
	return ss.str();
}

int Duration::to_int()
{
	return duration;
}

Multiplier::Multiplier(string arg)
{
	std::stringstream ss(arg);

	ss >> multiplier;
}

Multiplier::Multiplier(double arg)
{
	multiplier = arg;
}

string Multiplier::to_string(void)
{
	std::stringstream ss;

	ss << std::setprecision(2) << std::fixed << multiplier;
	return ss.str();
}

double Multiplier::to_double(void)
{
	return multiplier;
}

Wt::WRegExpValidator* Time::validator(void)
{

	Wt::WRegExpValidator *time_validator =
		new Wt::WRegExpValidator("\\s*[0-2]?[0-9]:[0-6][0-9]\\s*");

	return time_validator;
}

Wt::WRegExpValidator* Duration::validator(void)
{
	Wt::WRegExpValidator *duration_validator =
		new Wt::WRegExpValidator("\\s*-?([0-9]{1,2}h\\s*)?([0-9]{1,2}m\\s*)?([0-9]{1,2}s)?\\s*");

	return duration_validator;
}

Wt::WRegExpValidator* Multiplier::validator(void)
{
	Wt::WRegExpValidator *multiplier_validator =
		new Wt::WRegExpValidator("\\s*[0-9]+\\.?[0-9]*\\s*");

	return multiplier_validator;
}

int zone_utc_plus()
{
	std::time_t now = time(0);
	std::tm tm = *localtime(&now);
	if(tm.tm_isdst) return 3;
	else return 2;
}

std::tuple<Time,Time,bool> sunriset_today(void)
{
	double lon = 25.183, lat = 61.567;
	std::time_t now = time(0);
	std::tm tm = *localtime(&now);
	int year = 1900 + tm.tm_year, mon = tm.tm_mon + 1, day = tm.tm_mday;
	double rise, set;
	bool rs;
	int zo = zone_utc_plus();

	rs = (bool) sun_rise_set(year, mon, day, lon, lat, &rise, &set);
	Time r, s;
	if(rs == 0) {
		rise += zo; set += zo;
		if(set>24) set -= 24;
		r = Time(rise*3600);
		s = Time(set*3600);
	} else { //yoton yo
		r = Time(0);
		s = Time(0);
	}
	return std::make_tuple(r,s,rs);
}
	
Multiplier current_radiation()
{
	std::ifstream fin("sensors/26.1C40B6000000/vis");
	double r;
	fin >> r;
	return Multiplier(r*1000.0);
}
