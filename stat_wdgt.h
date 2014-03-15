#ifndef STAT_WDGT_H
#define STAT_WDGT_H
#include <Wt/WContainerWidget>
#include <Wt/WBreak>
#include <vector>
#include "config.h"

using std::vector;

class Stat_chunk : public Wt::WContainerWidget
{
public:
	Stat_chunk			    (int _i, Wt::WContainerWidget* parent = 0);
	void button_clicked     (void);
	void set_state		    (line_oper_state s);
	void modify_config_state(void);

	int				 i;
	Wt::WPushButton* stat_button;
	line_oper_state  state;
	Wt::WText*		 indicator;
};

class Stat_wdgt : public Wt::WContainerWidget {
public:
	Stat_wdgt		  (Wt::WContainerWidget* parent = 0);
	void update_status(int id, bool state);
	
	vector<Stat_chunk*> chunks;
};

#endif //STAT_WDGT_H
