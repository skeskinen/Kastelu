#ifndef DBO_H
#define DBO_H

#include <vector>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/Session>
#include <Wt/Dbo/backend/Sqlite3>
#include <Wt/Dbo/WtSqlTraits>
#include <Wt/Dbo/SqlTraits>
#include <Wt/Dbo/SqlTraits_impl.h>

namespace dbo = Wt::Dbo;

class Config_db_obj : public dbo::Dbo<Config_db_obj> {
public:
	double multiplier;
	int start_delay;
	int end_delay;

	Config_db_obj();

	template<typename Action>
	void persist(Action& a)
	{
		dbo::field(a, multiplier, "multiplier");
		dbo::field(a, start_delay, "start_delay");
		dbo::field(a, end_delay, "end_delay");
	}
};

class Line_db_obj : public dbo::Dbo<Line_db_obj> {
public:
	enum Line_oper_state {
		AUTO_FUNC = 0,
		FORCE_OFF,
		FORCE_ON
	};

	Line_db_obj();

	long long		m_id;
	std::string     name;
	Line_oper_state state;
	double 			multiplier;
	int				program;
	int				total_today;

	template<typename Action>
	void persist(Action& a)
	{
		dbo::field(a, name, "name");
		dbo::field(a, state, "state");
		dbo::field(a, multiplier, "multiplier");
		dbo::field(a, program, "program");
		dbo::field(a, total_today, "total_today");
	}
	
};

class Prog_db_obj : public dbo::Dbo<Prog_db_obj> {
public:
	Prog_db_obj();

	long long 	m_id;
	std::string name;
	int       	duration;
	int       	interval;
	double		radiation;

	template<typename Action>
	void persist(Action& a)
	{
		dbo::field(a, name, "name");
		dbo::field(a, duration, "duration");
		dbo::field(a, interval, "interval");
		dbo::field(a, radiation, "radiation");
	}
};

typedef Line_db_obj::Line_oper_state State;

class Session {
public:
	Session();

	Config_db_obj			 get_config();
	Line_db_obj 	         get_line   (int i);
	std::vector<Line_db_obj> get_lines  (void);
	std::vector<Prog_db_obj> get_progs  (void);
	void					 set_config(Config_db_obj);
	void                     store_prog (Prog_db_obj* prog);
	void					 remove_prog(int i);
	void					 set_state  (int i, State s);
	void					 update_line(Line_db_obj l);
	void					 ensure_lines_exist(void);
	void					 ensure_config_exists(void);
	void					 reset_totals(void);
	void					 increment_total(int line_id, int inc);


	dbo::Session          session;
	dbo::backend::Sqlite3 sqlite3;
};

#endif //DBO_H
