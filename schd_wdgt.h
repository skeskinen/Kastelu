#ifndef SCHD_WDGT_H
#define SCHD_WDGT_H
#include <Wt/WContainerWidget>
#include <Wt/WDialog>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WGroupBox>
#include "dbo.h"

class Schd_group : public Wt::WGroupBox {
public:
	Schd_group(Line_db_obj _line, Session& sess);

	void reload(void);

	Line_db_obj line;
	Session &session;
};

class Prog_form : public Wt::WDialog {
public:
	Prog_form(int _line, Schd_group* group, Session &sess);

	void validate	(void);
	void destroy	(void);
	void create_prog(void);

	Wt::WLineEdit   *start_field;
	Wt::WLineEdit   *end_field;
	Wt::WLineEdit   *duration_field;
	Wt::WLineEdit   *interval_field;
	Wt::WPushButton *ok_button;
	Wt::WPushButton *cancel_button;
	int				line;
	Schd_group      *group_widget;
	Session			&session;
};

class Schd_chunk : public Wt::WContainerWidget {
public:
	Schd_chunk(Prog_db_obj prog, Session& sess);

	Prog_db_obj program;
	Session &session;
};

class Schd_wdgt : public Wt::WContainerWidget {
public:
	Schd_wdgt(Session &sess, Wt::WContainerWidget* parent = 0);

	Session         &session;
};

#endif //SCHD_WDGT_H
