#ifndef CONFIG_WDGT_H
#define CONFIG_WDGT_H

#include <Wt/WContainerWidget>
#include <Wt/WLabel>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include "dbo.h"

class Config_wdgt : public Wt::WContainerWidget {
public:
	Config_wdgt(Session &sess);

	void update_db(void);

	Wt::WLineEdit	*multiplier_field;
	Wt::WLineEdit	*start_field;
	Wt::WLineEdit	*end_field;
	Wt::WPushButton	*update_button;
	Session 		&session;
};

#endif //CONFIG_WDGT_H
