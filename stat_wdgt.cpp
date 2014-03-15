#include "stat_wdgt.h"
#include <iostream>
#include <Wt/WText>
#include <Wt/WPushButton>

using std::cerr;
using std::endl;

vector<cfg_chunk> cfg;

Stat_chunk::Stat_chunk(int _i, Wt::WContainerWidget* parent)
	: Wt::WContainerWidget(parent)
	, i(_i)
{
	setStyleClass("stat_chunk stat_chunk_off");
	addWidget(new Wt::WText(cfg[i].name));
	addWidget(new Wt::WBreak());
	addWidget(indicator = new Wt::WText()); 
	indicator->addStyleClass("indicator");
	addWidget(stat_button = new Wt::WPushButton());
	stat_button->clicked().
		connect(this, &Stat_chunk::button_clicked);
};

void Stat_chunk::modify_config_state(void)
{
	config_state(i, state);
}

void Stat_chunk::set_state(line_oper_state s)
{
	state = s;
	switch(s) {
		case line_oper_state::AUTO_FUNC:
			stat_button->setStyleClass("stat_button stat_button_auto");
			stat_button->setText("auto");
			break;
		case line_oper_state::FORCE_ON:
			stat_button->setStyleClass("stat_button stat_button_on");
			stat_button->setText(L"päällä");
			break;
		case line_oper_state::FORCE_OFF:
			stat_button->setStyleClass("stat_button stat_button_off");
			stat_button->setText("pois");
			break;
	}
}


void Stat_chunk::button_clicked()
{
	switch(state) {
		case line_oper_state::AUTO_FUNC:
			set_state(line_oper_state::FORCE_ON);
			break;
		case line_oper_state::FORCE_ON:
			set_state(line_oper_state::FORCE_OFF);
			break;
		case line_oper_state::FORCE_OFF:
			set_state(line_oper_state::AUTO_FUNC);
			break;
	}
	modify_config_state();
}

Stat_wdgt::Stat_wdgt(Wt::WContainerWidget* parent)
	: Wt::WContainerWidget(parent)
	, chunks(vector<Stat_chunk*>())
{
	int i;
	Stat_chunk* w;

	cfg = clone_config();
	setStyleClass("stat_wdgt");
	for(i = 0; i < line_count(); i++) {
		w = new Stat_chunk(i, this);
		chunks.push_back(w);
		w->set_state(cfg[i].state);
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
