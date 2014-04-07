#include "prog_wdgt.h"
#include "datatypes.h"
#include <Wt/WLabel>
#include <Wt/WTable>

Prog_wdgt::Prog_wdgt(Session &sess)
	: Wt::WContainerWidget()
	, session(sess)
{
	setStyleClass("prog_wdgt");

	reload();
}

void Prog_wdgt::reload(void)
{
	clear();

	Wt::WPushButton *add = new Wt::WPushButton();
	std::vector<Prog_db_obj> progs = session.get_progs();
	for(auto prog : progs) {
		addWidget(new Prog_chunk(prog, session));
	}
	add->addStyleClass("plus_icon");
	add->clicked().connect(std::bind([=](){new Prog_form(this, session);}));
	addWidget(add);
}

Prog_chunk::Prog_chunk(Prog_db_obj prog, Session &sess)
	: Wt::WContainerWidget()
	, program(prog)
	, session(sess)
{
	setStyleClass("prog_chunk");

	Wt::WPushButton *remove_button;

	addWidget(remove_button = new Wt::WPushButton());
	remove_button->addStyleClass("cross_icon");
	remove_button->clicked().connect(std::bind([&](){
				session.remove_prog(program.m_id);
				delete this;
				}));

	Wt::WTable *t = new Wt::WTable();
	addWidget(t);

	t->elementAt(0,0)->addWidget(new Wt::WLabel(std::to_string(program.m_id) 
				+ " - " + program.name));

	t->elementAt(1,0)->addWidget(new Wt::WLabel("Kastelu aika:"));
	t->elementAt(1,1)->addWidget(new Wt::WLabel(Duration(program.duration).to_string()));

	t->elementAt(2,0)->addWidget(new Wt::WLabel(L"Säteily määrä:"));
	t->elementAt(2,1)->addWidget(new Wt::WLabel(Multiplier(program.radiation).to_string()));

	t->elementAt(3,0)->addWidget(new Wt::WLabel("tai"));

	t->elementAt(4,0)->addWidget(new Wt::WLabel(L"Kastelu väli:"));
	t->elementAt(4,1)->addWidget(new Wt::WLabel(Duration(program.interval).to_string()));

}

Prog_form::Prog_form(Prog_wdgt* progs, Session &sess)
	: Wt::WDialog()
	, programs(progs)
	, session(sess)
{
	typedef Wt::WLabel Label;
	typedef Wt::WLineEdit Line;
	typedef Wt::WPushButton Button;
	setWindowTitle("Uusi ohjelma");


	Wt::WTable *t = new Wt::WTable();
	contents()->addWidget(t);

	t->elementAt(0, 0)->addWidget(new Label("Nimi: "));
	t->elementAt(0, 2)->addWidget(name_field = new Line("Ohjelma"));

	t->elementAt(1, 0)->addWidget(new Label("Kastelu aika: "));
	t->elementAt(1, 2)->addWidget(duration_field = new Line("5m 30s"));

	t->elementAt(2, 0)->addWidget(new Label("Säteilyn määrä: "));
	t->elementAt(2, 2)->addWidget(radiation_field = new Line("0.00"));

	t->elementAt(3, 1)->addWidget(new Label("tai"));

	t->elementAt(4, 0)->addWidget(new Label("Kastelu väli: "));
	t->elementAt(4, 2)->addWidget(interval_field = new Line("1h 15m"));
	
	t->elementAt(5, 0)->addWidget(ok_button = new Button("Ok"));
	t->elementAt(5, 1)->addWidget(cancel_button = new Button("Peruuta"));

	duration_field->setValidator(Duration::validator());
	interval_field->setValidator(Duration::validator());
	radiation_field->setValidator(Multiplier::validator());

	ok_button->clicked().connect(this, &Prog_form::validate);
	cancel_button->clicked().connect(this, &Wt::WDialog::reject);

	finished().connect(this, &Prog_form::destroy);

	rejectWhenEscapePressed();
	show();
}

void Prog_form::validate(void)
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

	v(duration_field);
	v(interval_field);
	v(radiation_field);

	if(ok)
		accept();
}

void Prog_form::destroy(void)
{
	if (result() == Wt::WDialog::Accepted) {
		create_prog();
	}
	delete this;
}

void Prog_form::create_prog(void)
{
	Prog_db_obj *prog = new Prog_db_obj();

	prog->name = name_field->text().toUTF8();
	prog->duration = Duration(duration_field->text().narrow()).to_int();
	prog->interval = Duration(interval_field->text().narrow()).to_int();
	prog->radiation = Multiplier(radiation_field->text().narrow()).to_double();
	session.store_prog(prog);
	programs->reload();
}
