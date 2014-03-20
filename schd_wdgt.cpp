#include "schd_wdgt.h"
#include <Wt/WLabel>
#include <Wt/WBreak>
#include <Wt/WTable>
#include <functional>
#include <Wt/WRegExpValidator>
#include <Wt/WText>
#include <sstream>
#include <iostream>
#include <cstdlib>

using std::cerr;
using std::endl;

Prog_form::Prog_form(int _line, Schd_group* group, Session &sess)
	: Wt::WDialog()
	, line(_line)
	, group_widget(group)
	, session(sess)
{
	typedef Wt::WLabel Label;
	typedef Wt::WLineEdit Line;
	typedef Wt::WPushButton Button;
	setWindowTitle(group->line.name);

	Wt::WRegExpValidator *time_validator =
		new Wt::WRegExpValidator("[0-2]?[0-9]:[0-6][0-9]");
	Wt::WRegExpValidator *duration_validator =
		new Wt::WRegExpValidator("([0-9]{1,2}h ?)?([0-9]{1,2}m ?)? ?([0-9]{1,2}s)?");

	Wt::WTable *table = new Wt::WTable();
	contents()->addWidget(table);

	table->elementAt(0, 0)->addWidget(new Label("Alku aika: "));
	table->elementAt(0, 1)->addWidget(start_field = new Line("08:00"));
	
	table->elementAt(1, 0)->addWidget(new Label("Loppu aika: "));
	table->elementAt(1, 1)->addWidget(end_field = new Line("21:00"));

	table->elementAt(2, 0)->addWidget(new Label("Päällä kerralla: "));
	table->elementAt(2, 1)->addWidget(duration_field = new Line("1m 30s"));

	table->elementAt(3, 0)->addWidget(new Label("Poissa päältä kerralla: "));
	table->elementAt(3, 1)->addWidget(interval_field = new Line("1h 15m"));
	
	contents()->addWidget(new Wt::WBreak());
	contents()->addWidget(ok_button = new Button("Ok"));
	contents()->addWidget(cancel_button = new Button("Peruuta"));

	start_field->setValidator(time_validator);
	end_field->setValidator(time_validator);
	duration_field->setValidator(duration_validator);
	interval_field->setValidator(duration_validator);

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

	v(start_field);
	v(end_field);
	v(duration_field);
	v(interval_field);

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

static int parse_duration(std::string str)
{
	int seconds = 0;
	int i;
	char c;
	std::stringstream ss(str);

	ss >> i >> c;
	while(ss.good())
	{
		switch(c) {
			case 'h':
				seconds += i * 60 * 60;
				break;
			case 'm':
				seconds += i * 60;
				break;
			case 's':
				seconds += i;
				break;
		}
		ss >> i >> c;
	}
	return seconds;
}

static int parse_time(std::string str)
{
	int seconds = 0;

	int i;
	char c;
	std::stringstream ss(str);

	ss >> i >> c;
	seconds += i * 60 * 60;
	ss >> i;
	seconds += i * 60;

	return seconds;
}

void Prog_form::create_prog(void)
{
	Prog_db_obj *prog = new Prog_db_obj();

	prog->line = line;
	prog->start_time = parse_time(start_field->text().narrow());
	prog->end_time = parse_time(end_field->text().narrow());
	prog->duration = parse_duration(duration_field->text().narrow());
	prog->interval = parse_duration(interval_field->text().narrow());
	session.store_prog(prog);
	group_widget->reload();
}

static std::string time_string(int seconds)
{
	std::stringstream ss;
	ss.fill('0');
	ss << std::setw(2) << seconds/60/60;
	ss << ":" << std::setw(2) << ((seconds/60)%60);
	return ss.str();
}

static std::string duration_string(int seconds)
{
	if(seconds <= 0)
		return "0s ";
	else {
		std::stringstream ss;
		int hours = seconds/60/60;
		int minutes = (seconds/60)%60;
		seconds %= 60;
		if(hours > 0)
			ss << hours << "h ";
		if(minutes > 0)
			ss << minutes << "m ";
		if(seconds > 0)
			ss << seconds << "s ";
		return ss.str();
	}
}

static Wt::WString interval_string(Prog_db_obj prog)
{	
	Wt::WString str = "Voimassa: ";
	std::stringstream ss;
	ss << time_string(prog.start_time) << "-" 
		<< time_string(prog.end_time);
	str += ss.str();
	return str;
}

static Wt::WString on_string(Prog_db_obj prog)
{	
	Wt::WString str = L"Päällä: ";
	str += duration_string(prog.duration);
	return str;
}

static Wt::WString off_string(Prog_db_obj prog)
{	
	Wt::WString str = L"Pois päältä: ";
	str += duration_string(prog.interval);
	return str;
}

static int current_time()
{
	std::time_t now;
	now = time(0);
	
	std::tm midnight = *localtime(&now);
	midnight.tm_hour = 0; midnight.tm_min = 0; midnight.tm_sec = 0;
	return (int) std::difftime(now, std::mktime(&midnight));
}

static Wt::WString next_time_string(Prog_db_obj prog)
{	 
	Wt::WString str = L"Seuraavan kerran: "; 
	int now = current_time();
	int start_time;
	if(now < prog.start_time || now >= prog.end_time) {
		start_time = prog.start_time;
	} else {
		int sum = prog.duration + prog.interval;
		int mul = ((now - prog.start_time)/sum);
		int ref_point = prog.start_time + mul * sum;
		if(ref_point + prog.duration > now) //program running
			start_time = ref_point;
		else if(ref_point + sum >= prog.end_time) //next one tomorrow
			start_time = prog.start_time;
		else								//currently waiting
			start_time = ref_point + sum;
	}
	std::stringstream ss;
	ss << time_string(start_time) << "-" 
		<< time_string(std::min(start_time + prog.duration, prog.end_time));
	str += ss.str();
	return str;
}

static Wt::WString total_string(Prog_db_obj prog)
{	
	Wt::WString str = L"Yhteensä päivässä: ";
	int delta = prog.end_time - prog.start_time;
	int sum = prog.duration + prog.interval;
	int total = (delta / sum) * prog.duration; 
	int remainder = delta - ((delta/sum)*sum);
	total += std::min(prog.duration, remainder);
	cerr << "sum " << sum << " total " << total << " remainder " << remainder << endl;
	str += duration_string(total);
	return str;
}

Schd_chunk::Schd_chunk(Prog_db_obj prog, Session& sess)
	: Wt::WContainerWidget()
	, program(prog)
	, session(sess)
{
	Wt::WPushButton *remove_button;

	addStyleClass("schd_chunk");
	addWidget(remove_button = new Wt::WPushButton());
	remove_button->addStyleClass("cross_icon");
	remove_button->clicked().connect(std::bind([&](){
				session.remove_prog(program.m_id);
				delete this;
				}));

	addWidget(new Wt::WText(interval_string(program)));
	addWidget(new Wt::WBreak());
	addWidget(new Wt::WText(on_string(program)));
	addWidget(new Wt::WBreak());
	addWidget(new Wt::WText(off_string(program)));
	addWidget(new Wt::WBreak());
	addWidget(new Wt::WText(next_time_string(program)));
	addWidget(new Wt::WBreak());
	addWidget(new Wt::WText(total_string(program)));
}

Schd_group::Schd_group(Line_db_obj _line, Session &sess)
	: Wt::WGroupBox()
	, line(_line)
	, session(sess)
{
	setTitle(line.name);
	reload();
}

void Schd_group::reload(void)
{
	clear();

	Wt::WPushButton *add = new Wt::WPushButton();
	std::vector<Prog_db_obj> progs = session.get_progs(line.m_id);
	for(auto prog : progs) {
		addWidget(new Schd_chunk(prog, session));
	}
	add->addStyleClass("plus_icon");
	add->clicked().connect(std::bind([=](){new Prog_form(line.m_id, this, session);}));
	addWidget(add);
}

Schd_wdgt::Schd_wdgt(Session& sess, Wt::WContainerWidget* parent)
	: Wt::WContainerWidget(parent)
	, session(sess)
{
	std::vector<Line_db_obj> lines = session.get_lines();

	for(auto& line : lines) {
		addWidget(new Schd_group(line, session));
		addWidget(new Wt::WBreak());
	}
}
