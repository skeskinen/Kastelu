#ifndef STAT_WDGT_H
#define STAT_WDGT_H
#include <Wt/WContainerWidget>
#include <Wt/WBreak>
#include <Wt/WLineEdit>
#include <vector>
#include "dbo.h"

using std::vector;

class Stat_chunk : public Wt::WContainerWidget
{
public:
	Stat_chunk			    (Line_db_obj l, Session& sess);
	void button_clicked     (void);
	void set_state		    (Line_db_obj::Line_oper_state s);

	Line_db_obj					  line;
	Wt::WLineEdit				  *name_field;
	Wt::WPushButton				  *name_button;
	Wt::WPushButton               *stat_button;
	Line_db_obj::Line_oper_state  state;
	Wt::WText		              *indicator;
	Session			              &session;
};

class Stat_wdgt : public Wt::WContainerWidget {
public:
	Stat_wdgt		  (Session& sess, Wt::WContainerWidget* parent = 0);
	void update_status(int id, bool state);
	
	vector<Stat_chunk*> chunks;
	Session             &session;
};

#endif //STAT_WDGT_H
