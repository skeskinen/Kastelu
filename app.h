#ifndef APP_H
#define APP_H
#include <Wt/WApplication>
#include "dbo.h"
#include "config_wdgt.h"
#include "stat_wdgt.h"
#include "line_wdgt.h"
#include "prog_wdgt.h"

class App : public Wt::WApplication {
public:
	App(const Wt::WEnvironment& env);
	~App();

	Config_wdgt	*config_wdgt;
	Stat_wdgt	*stat_wdgt;
	Line_wdgt	*line_wdgt;
	Prog_wdgt	*prog_wdgt;
	Session    session;
};

#endif //APP_H
