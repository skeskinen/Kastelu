#include "app.h"
#include "worker.h"
#include <Wt/WBreak>

App::App(const Wt::WEnvironment& env)
	: Wt::WApplication(env)
{
	useStyleSheet("style.css");
	root()->addWidget(stat_wdgt = new Stat_wdgt());
	root()->addWidget(new Wt::WBreak());
	root()->addWidget(new Wt::WBreak());
	root()->addWidget(schd_wdgt = new Schd_wdgt());
	enableUpdates(true);
	worker_connect(this);
}

App::~App()
{
	worker_disconnect(this);
}
