#ifndef CONFIG_H
#define CONFIG_H
#include <fstream>
#include <vector>
#include <string>

using std::vector;
using std::string;

enum line_oper_state {
	AUTO_FUNC = 0,
	FORCE_OFF,
	FORCE_ON
};

std::istream& operator>>(std::istream& s, line_oper_state& l);
std::ostream& operator<<(std::ostream& s, line_oper_state& l);

struct line_oper_prog {
	int	 start_hour;
	int	 end_hour;
	int  duration;
	int	 interval;
	bool active;
};

struct cfg_chunk {
	string 				   name;
	line_oper_state 	   state;
	int 				   progs_n;
	vector<line_oper_prog> progs;
};


void			  load_config (void);
vector<cfg_chunk> clone_config(void);
int				  line_count  (void);
void			  config_state(int i, line_oper_state s);

#endif //CONFIG_H
