#include "app.h"
#include "worker.h"
#include <Wt/WBreak>
#include <Wt/WBootstrapTheme>

App::App(const Wt::WEnvironment& env)
	: Wt::WApplication(env)
{
	setTheme(new Wt::WBootstrapTheme());
	useStyleSheet("style.css");
	root()->addWidget(stat_wdgt = new Stat_wdgt(session));
	root()->addWidget(new Wt::WBreak());
	root()->addWidget(new Wt::WBreak());
	root()->addWidget(schd_wdgt = new Schd_wdgt(session));
	enableUpdates(true);
	worker_connect(this);
}

App::~App()
{
	worker_disconnect(this);
}
