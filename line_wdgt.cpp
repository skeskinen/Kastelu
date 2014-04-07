#include "line_wdgt.h"
#include <Wt/WLabel>
#include <Wt/WBreak>
#include <Wt/WTable>
#include <functional>
#include <Wt/WText>
#include <sstream>
#include <cstdlib>
#include "datatypes.h"

using std::cerr;
using std::endl;


Line_wdgt::Line_wdgt(Session& sess)
	: Wt::WContainerWidget()
	, session(sess)
{
	setStyleClass("line_wdgt");
	refresh();
}

void Line_wdgt::refresh(void)
{
	std::vector<Line_db_obj> lines = session.get_lines();
	clear();
	for(auto& line : lines) {
		addWidget(new Line_chunk(line, session));
	}

}

Line_chunk::Line_chunk(Line_db_obj li, Session& sess)
	: Wt::WGroupBox()
	, line(li)
	, session(sess)
{
	std::vector<Prog_db_obj> programs = session.get_progs();

	setStyleClass("line_chunk");
	setTitle(line.name);
	
	Wt::WTable *t = new Wt::WTable();
	addWidget(t);

	t->elementAt(0,0)->addWidget(new Wt::WLabel("Nimi:"));
	t->elementAt(0,1)->addWidget(name_field = new Wt::WLineEdit(line.name));

	t->elementAt(1,0)->addWidget(new Wt::WLabel("Kastelu kerroin:"));
	t->elementAt(1,1)->addWidget(multiplier_field 
			= new Wt::WLineEdit(Multiplier(line.multiplier).to_string()));

	t->elementAt(2,0)->addWidget(new Wt::WLabel("Kastelu ohjelma:"));
	t->elementAt(2,1)->addWidget(program_box = new Wt::WComboBox());

	program_box->addItem("");
	for(size_t i = 0; i < programs.size(); i++) {
		auto prog = programs[i];
		program_box->addItem(std::to_string(prog.m_id) + " - " + prog.name);
		if(prog.m_id == line.program) {
			program_box->setCurrentIndex(i+1);
		}
	}
	
	t->elementAt(3,0)->addWidget(new Wt::WLabel(L"Kasteltu tänään:"));
	t->elementAt(3,1)->addWidget(new Wt::WLabel(Duration(line.total_today).to_string()));

	t->elementAt(4,0)->addWidget(update_button = new Wt::WPushButton(L"päivitä"));

	multiplier_field->setValidator(Multiplier::validator());

	update_button->clicked().connect(this, &Line_chunk::update);
}

void Line_chunk::update(void)
{
	bool ok = true;
	auto v = [&](Wt::WLineEdit* arg) {
		if(arg->validate() != Wt::WValidator::Valid) {
			ok = false;
			arg->setStyleClass("invalid");
		} else {
			arg->setStyleClass("");
		}
	};

	v(multiplier_field);

	if(ok) {
		line.name = name_field->text().toUTF8();
		line.multiplier = Multiplier(multiplier_field->text().narrow()).to_double();
		std::string prog_text = program_box->currentText().narrow();
		int program_i;
		if(prog_text.size() < 1) program_i = -1;
		else {
			std::stringstream ss(prog_text);
			ss >> program_i;
		}
		line.program = program_i;
		session.update_line(line);
		((Line_wdgt*)parent())->refresh();
	}
}
