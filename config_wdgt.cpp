#include "config_wdgt.h"
#include "datatypes.h"
#include <Wt/WBreak>
#include <Wt/WTable>

Config_wdgt::Config_wdgt(Session &sess)
	: Wt::WContainerWidget()
	, session(sess)
{
	setStyleClass("config_wdgt");
	auto sun_riset = sunriset_today();
	Config_db_obj config = session.get_config();

	Wt::WTable *t = new Wt::WTable();
	addWidget(t);
	t->elementAt(0,0)->addWidget(new Wt::WLabel("Aurinko nousee:"));
	t->elementAt(0,1)->addWidget(new Wt::WLabel(std::get<0>(sun_riset).to_string()));

	t->elementAt(1,0)->addWidget(new Wt::WLabel("Aurinko laskee:"));
	t->elementAt(1,1)->addWidget(new Wt::WLabel(std::get<1>(sun_riset).to_string()));

	Duration start_delay = Duration(config.start_delay);
	Duration end_delay = Duration(config.end_delay);
	t->elementAt(2,0)->addWidget(new Wt::WLabel("Aloita kastelu"));
	t->elementAt(2,1)->addWidget(start_field = new Wt::WLineEdit(start_delay.to_string()));
	t->elementAt(2,2)->addWidget(new Wt::WLabel(" auringonnousun jälkeen "));
	t->elementAt(3,0)->addWidget(new Wt::WLabel("Lopeta kastelu"));
	t->elementAt(3,1)->addWidget(end_field = new Wt::WLineEdit(end_delay.to_string()));
	t->elementAt(3,2)->addWidget(new Wt::WLabel(" auringonlaskun jälkeen "));

	Multiplier mul = Multiplier(config.multiplier);
	t->elementAt(4,0)->addWidget(new Wt::WLabel("Kastelun pituus kerroin:"));
	t->elementAt(4,1)->addWidget(multiplier_field = new Wt::WLineEdit(mul.to_string()));
	t->elementAt(5,0)->addWidget(update_button = new Wt::WPushButton("päivitä"));

	update_button->clicked().connect(this, &Config_wdgt::update_db);

	start_field->setValidator(Duration::validator());
	end_field->setValidator(Duration::validator());
	multiplier_field->setValidator(Multiplier::validator());
}

void Config_wdgt::update_db(void)
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

	v(start_field);
	v(end_field);
	v(multiplier_field);

	if(ok) {
		Config_db_obj config;
		config.start_delay = Duration(start_field->text().narrow()).to_int();
		config.end_delay = Duration(end_field->text().narrow()).to_int();
		config.multiplier = Multiplier(multiplier_field->text().narrow()).to_double();
		session.set_config(config);
	}
}
