#include "dbo.h"
#include <iostream>

using std::cerr;
using std::endl;

Line_db_obj::Line_db_obj()
	: dbo::Dbo<Line_db_obj>()
{

}

Prog_db_obj::Prog_db_obj()
	: dbo::Dbo<Prog_db_obj>()
{

}

Session::Session()
	: session()
	, sqlite3("kastelu.db")
{
	int count;

	session.setConnection(sqlite3);
	session.mapClass<Line_db_obj>("line");
	session.mapClass<Prog_db_obj>("program");

	dbo::Transaction transaction(session);
	count = session.query<int>("select count(*) from line");
	transaction.commit();
	if(count < 6) {
		for(int i = 0; i < 6; i++) {
			dbo::Transaction transaction(session);
			Line_db_obj *line = new Line_db_obj();
			line->name = std::string("0");
			line->name[0] += i;
			line->state = Line_db_obj::Line_oper_state::AUTO_FUNC;
			session.add(line);
			transaction.commit();
		}
	}
}

Line_db_obj Session::get_line(int i)
{
	dbo::ptr<Line_db_obj> line;
	Line_db_obj			  ret;
	dbo::Transaction      transaction(session);

	line = session.find<Line_db_obj>().limit(1).offset(i);
	ret = *line;
	ret.m_id = line->id();
	transaction.commit();

	return *line;
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

std::vector<Prog_db_obj> Session::get_progs(int line)
{
	dbo::collection<dbo::ptr<Prog_db_obj>> progs;
	std::vector<Prog_db_obj> ret;
	dbo::Transaction transaction(session);

	progs = session.query<dbo::ptr<Prog_db_obj>>("select p from program p").where("line = ?").bind(line);
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

void Session::set_name  (int i, std::string str)
{
	dbo::ptr<Line_db_obj> line;
	dbo::Transaction transaction(session);

	line = session.find<Line_db_obj>().where("id = ?").bind(i);
	line.modify()->name = str;
	transaction.commit();
}
