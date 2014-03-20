#ifndef APP_H
#define APP_H
#include <Wt/WApplication>
#include "dbo.h"
#include "stat_wdgt.h"
#include "schd_wdgt.h"

class App : public Wt::WApplication {
public:
	App(const Wt::WEnvironment& env);
	~App();

	Stat_wdgt* stat_wdgt;
	Schd_wdgt* schd_wdgt;
	Session    session;
};

#endif //APP_H
