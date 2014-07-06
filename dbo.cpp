#include "dbo.h"
#include <iostream>

using std::cerr;
using std::endl;

Config_db_obj::Config_db_obj()
	: dbo::Dbo<Config_db_obj>() { }

Line_db_obj::Line_db_obj()
	: dbo::Dbo<Line_db_obj>() { } 

Prog_db_obj::Prog_db_obj()
	: dbo::Dbo<Prog_db_obj>() { }

Session::Session()
	: session()
	, sqlite3("kastelu.db")
{
	session.setConnection(sqlite3);
	session.mapClass<Config_db_obj>("config");
	session.mapClass<Line_db_obj>("line");
	session.mapClass<Prog_db_obj>("program");

	try{
		session.createTables();
	}catch(...){}

	ensure_lines_exist();
	ensure_config_exists();
}

void Session::ensure_lines_exist(void)
{
	dbo::Transaction transaction(session);
	int count;
   	count = session.query<int>("select count(*) from line");
	transaction.commit();
	if(count < 10) {
		for(int i = 0; i < 10; i++) {
			dbo::Transaction transaction(session);
			Line_db_obj *line = new Line_db_obj();
			line->name = std::string("A");
			line->name[0] += i;
			line->state = State::AUTO_FUNC;
			line->multiplier = 1.0;
			line->program = -1;
			line->total_today = 0;
			session.add(line);
			transaction.commit();
		}
	}
}

void Session::ensure_config_exists(void)
{
	dbo::Transaction transaction(session);
	int count;

	count = session.query<int>("select count(*) from config");
	transaction.commit();
	if(count < 1) {
		dbo::Transaction transaction(session);
		Config_db_obj *config = new Config_db_obj();
		config->multiplier = 1.0;
		config->start_delay = 0;
		config->end_delay = 0;
		session.add(config);
		transaction.commit();
	}
}

Config_db_obj Session::get_config()
{
	dbo::ptr<Config_db_obj> config;
	dbo::Transaction      	transaction(session);

	config = session.find<Config_db_obj>().limit(1);
	transaction.commit();
	return *config;
}


Line_db_obj Session::get_line(int i)
{
	dbo::ptr<Line_db_obj> line;
	dbo::Transaction      transaction(session);
	Line_db_obj			  ret;

	line = session.find<Line_db_obj>().limit(1).offset(i);
	transaction.commit();
	ret = *line;
	ret.m_id = line->id();
	return ret;
}

std::vector<Line_db_obj> Session::get_lines()
{
	dbo::collection<dbo::ptr<Line_db_obj> > lines;
	std::vector<Line_db_obj> ret;
	dbo::Transaction transaction(session);

	lines = session.query<dbo::ptr<Line_db_obj> >("select l from line l");
	for(auto line : lines) {
		ret.push_back(*line);
		ret.back().m_id = line->id();
	}
	transaction.commit();
	return ret;
}

std::vector<Prog_db_obj> Session::get_progs(void)
{
	dbo::collection<dbo::ptr<Prog_db_obj>> progs;
	std::vector<Prog_db_obj> ret;
	dbo::Transaction transaction(session);

	progs = session.query<dbo::ptr<Prog_db_obj>>("select p from program p");
	for(auto prog : progs) {
		ret.push_back(*prog);
		ret.back().m_id = prog->id();
	}
	transaction.commit();
	return ret;
}

void Session::store_prog(Prog_db_obj* prog)
{
	dbo::Transaction transaction(session);
	session.add(prog);
	transaction.commit();
}

void Session::remove_prog(int i)
{
	dbo::ptr<Prog_db_obj> prog;
	dbo::Transaction transaction(session);

	prog = session.find<Prog_db_obj>().where("id = ?").bind(i);
	prog.remove();
	transaction.commit();
}

void Session::set_state(int i, State s)
{
	dbo::ptr<Line_db_obj> line;
	dbo::Transaction transaction(session);

	line = session.find<Line_db_obj>().where("id = ?").bind(i);
	line.modify()->state = s;
	transaction.commit();
}

void Session::set_config(Config_db_obj cfg)
{
	dbo::ptr<Config_db_obj> config;
	dbo::Transaction transaction(session);

	config = session.find<Config_db_obj>().limit(1);
	config.modify()->start_delay = cfg.start_delay;
	config.modify()->end_delay = cfg.end_delay;
	config.modify()->multiplier = cfg.multiplier;
	transaction.commit();
}

void Session::update_line(Line_db_obj l)
{
	dbo::ptr<Line_db_obj> line;
	dbo::Transaction transaction(session);

	line = session.find<Line_db_obj>().where("id = ?").bind(l.m_id);
	line.modify()->name = l.name;
	line.modify()->multiplier = l.multiplier;
	line.modify()->program = l.program;
	transaction.commit();
}

void Session::reset_totals(void)
{
	dbo::collection<dbo::ptr<Line_db_obj> > lines;
	dbo::Transaction transaction(session);
	lines = session.query<dbo::ptr<Line_db_obj> >("select l from line l");
	for(auto line : lines) {
		line.modify()->total_today = 0;
	}
	transaction.commit();
}

void Session::increment_total(int line_id, int inc)
{
	dbo::ptr<Line_db_obj> line;
	dbo::Transaction transaction(session);

	line = session.find<Line_db_obj>().where("id = ?").bind(line_id);
	line.modify()->total_today += inc;
	transaction.commit();
}
