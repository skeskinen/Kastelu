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

class Line_db_obj : public dbo::Dbo<Line_db_obj> {
public:
	enum Line_oper_state {
		AUTO_FUNC = 0,
		FORCE_OFF,
		FORCE_ON
	};

	Line_db_obj();

	long long	    m_id;
	std::string     name;
	Line_oper_state state;

	template<typename Action>
	void persist(Action& a)
	{
		dbo::field(a, name, "name");
		dbo::field(a, state, "state");
	}
};

class Prog_db_obj : public dbo::Dbo<Prog_db_obj> {
public:
	Prog_db_obj();

	long long m_id;
	int       line;
	int       start_time;
	int       end_time;
	int       duration;
	int       interval;

	template<typename Action>
	void persist(Action& a)
	{
		dbo::field(a, line, "line");
		dbo::field(a, start_time, "start_time");
		dbo::field(a, end_time, "end_time");
		dbo::field(a, duration, "duration");
		dbo::field(a, interval, "interval");
	}
};

typedef Line_db_obj::Line_oper_state State;

class Session {
public:
	Session();

	Line_db_obj 	         get_line   (int i);
	std::vector<Line_db_obj> get_lines  (void);
	std::vector<Prog_db_obj> get_progs  (int line);
	void                     store_prog (Prog_db_obj* prog);
	void					 remove_prog(int i);
	void					 set_state  (int i, State s);
	void					 set_name   (int i, std::string str);

	dbo::Session          session;
	dbo::backend::Sqlite3 sqlite3;
};

#endif //DBO_H
