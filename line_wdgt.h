#ifndef LINE_WDGT_H
#define LINE_WDGT_H
#include <Wt/WContainerWidget>
#include <Wt/WDialog>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WGroupBox>
#include <Wt/WComboBox>
#include "dbo.h"
#include "time.h"

class Line_wdgt : public Wt::WContainerWidget {
public:
	Line_wdgt(Session &sess);

	void refresh(void);

	Session &session;
};

class Line_chunk : public Wt::WGroupBox {
public:
	Line_chunk(Line_db_obj li, Session& sess);

	void update(void);

	Wt::WLineEdit 	*name_field;
	Wt::WLineEdit 	*multiplier_field;
	Wt::WComboBox	*program_box;
	Wt::WPushButton *update_button;

	Line_db_obj line;
	Session &session;
};

#endif //LINE_WDGT_H
