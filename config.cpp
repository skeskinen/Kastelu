#include "config.h"
#include <mutex>
#include <iostream>
#include <fstream>
#include <sstream>

using std::cerr;
using std::endl;

static std::mutex 		 mtx;
static vector<cfg_chunk> config;
static int				 n;

static cfg_chunk read_cfg_line(string line)
{
	int 			  i;
	std::stringstream ss;
	cfg_chunk		  c;
	line_oper_prog	  p;

	ss.str(line);
	ss >> c.name >> c.state >> c.progs_n;
	for(i = 0; i < c.progs_n; i++) {
		p = line_oper_prog();
		ss >> p.start_hour >> p.end_hour >> p.duration
			>> p.interval >> p.active;
		c.progs.push_back(p);
	}
	return c;
}

static void write_cfg_line(std::ostream& s, int i)
{
	cfg_chunk c = config[i];

	s << c.name << " " << c.state << " " << c.progs_n;
	for(const line_oper_prog& p : c.progs) {
		s << " " << p.start_hour << " " << p.end_hour << " "
			<< p.duration << " " << p.interval << " " << p.active;
	}
	s << endl;
}

void load_config(void)
{
	int 		  i;
	std::string   str;
	cfg_chunk 	  c;
	std::ifstream fin("kastelu.cfg");

	mtx.lock();
	config = vector<cfg_chunk>();
	fin >> n;
	fin.get();
	for(i = 0; i < n; i++) {
		getline(fin, str);
		c = read_cfg_line(str);
		config.push_back(c);
	}
	mtx.unlock();
}

vector<cfg_chunk> clone_config(void)
{
	vector<cfg_chunk> ret;

	mtx.lock();
	ret.assign(config.begin(), config.end());
	mtx.unlock();
	return ret;
}

static void config_write()
{
	std::ofstream fout("kastelu.cfg");

	fout << n << endl;
	for(int i = 0; i < n; i++) {
		write_cfg_line(fout, i);
	}
}

void config_state(int i, line_oper_state s)
{
	mtx.lock();
	config[i].state = s;
	config_write();
	mtx.unlock();
}

int line_count(void)
{
	return n;
}

std::istream& operator>>(std::istream& s, line_oper_state& l)
{
	string str;
	s >> str;
	if(str == "on") {
		l = line_oper_state::FORCE_ON;
	} else if (str == "off") {
		l = line_oper_state::FORCE_OFF;
	} else {
		l = line_oper_state::AUTO_FUNC;
	}
	return s;
}

std::ostream& operator<<(std::ostream& s, line_oper_state& l)
{
	switch (l) {
		case line_oper_state::FORCE_OFF:
			s << "off";
			break;
		case line_oper_state::FORCE_ON:
			s << "on";
			break;
		default:
			s << "auto";
			break;
	}
	return s;
}
