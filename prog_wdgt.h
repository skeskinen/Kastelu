#ifndef PROG_WDGT_H
#define PROG_WDGT_H

#include <Wt/WContainerWidget>
#include <Wt/WGroupBox>
#include "dbo.h"
#include <Wt/WPushButton>
#include <Wt/WLineEdit>
#include <Wt/WDialog>

class Prog_wdgt : public Wt::WContainerWidget {
public:
	Prog_wdgt(Session& sess);

	void reload(void);

	Session &session;
};

class Prog_chunk : public Wt::WContainerWidget {
public:
	Prog_chunk(Prog_db_obj prog, Session& sess);


	Prog_db_obj program;
	Session		&session;
};

class Prog_form : public Wt::WDialog {
public:
	Prog_form(Prog_wdgt* progs, Session &sess);

	void validate	(void);
	void destroy	(void);
	void create_prog(void);

	Wt::WLineEdit   *name_field;
	Wt::WLineEdit   *duration_field;
	Wt::WLineEdit   *interval_field;
	Wt::WLineEdit   *radiation_field;;
	
	Wt::WPushButton *ok_button;
	Wt::WPushButton *cancel_button;
	int				line;
	Prog_wdgt		*programs;
	Session			&session;
};

#endif //PROG_WDGT_H
