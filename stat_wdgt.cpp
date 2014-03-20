#include "stat_wdgt.h"
#include <iostream>
#include <Wt/WText>
#include <Wt/WPushButton>
#include <functional>

using std::cerr;
using std::endl;

Stat_chunk::Stat_chunk(Line_db_obj l, Session& sess)
	: Wt::WContainerWidget()
	, line(l)
	, session(sess)
{
	setStyleClass("stat_chunk");
	addWidget(name_field = new Wt::WLineEdit(line.name));
	addWidget(name_button = new Wt::WPushButton(L"Päivitä"));
	name_field->addStyleClass("stat_name_field");
	addWidget(new Wt::WBreak());
	addWidget(indicator = new Wt::WText()); 
	indicator->addStyleClass("indicator");
	addWidget(stat_button = new Wt::WPushButton());
	name_button->clicked().connect(std::bind([&](){
				session.set_name(line.m_id, name_field->text().toUTF8());
				}));
	stat_button->clicked().
		connect(this, &Stat_chunk::button_clicked);
	set_state(line.state);
};

void Stat_chunk::set_state(Line_db_obj::Line_oper_state s)
{
	typedef Line_db_obj::Line_oper_state State;
	state = s;
	switch(s) {
		case State::AUTO_FUNC:
			stat_button->setStyleClass("stat_button stat_button_auto");
			stat_button->setText("auto");
			break;
		case State::FORCE_ON:
			stat_button->setStyleClass("stat_button stat_button_on");
			stat_button->setText(L"päällä");
			break;
		case State::FORCE_OFF:
			stat_button->setStyleClass("stat_button stat_button_off");
			stat_button->setText("pois");
			break;
	}
}


void Stat_chunk::button_clicked()
{
	switch(state) {
		case State::AUTO_FUNC:
			set_state(State::FORCE_ON);
			break;
		case State::FORCE_ON:
			set_state(State::FORCE_OFF);
			break;
		case State::FORCE_OFF:
			set_state(State::AUTO_FUNC);
			break;
	}
	session.set_state(line.m_id, state);
}

Stat_wdgt::Stat_wdgt(Session& sess, Wt::WContainerWidget* parent)
	: Wt::WContainerWidget(parent)
	, chunks(vector<Stat_chunk*>())
	, session(sess)
{
	Stat_chunk* w;
	std::vector<Line_db_obj> lines = session.get_lines();

	for(auto line : lines) {
		w = new Stat_chunk(line, session);
		addWidget(w);
		chunks.push_back(w);
	}
}

void Stat_wdgt::update_status(int id, bool status)
{
	if(status == true) {
		chunks[id]->indicator->addStyleClass("indicator_active");
	} else {
		chunks[id]->indicator->removeStyleClass("indicator_active");
	}
}
