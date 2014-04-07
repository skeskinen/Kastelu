#include "app.h"
#include "worker.h"
#include <Wt/WBreak>
#include <Wt/WBootstrapTheme>

App::App(const Wt::WEnvironment& env)
	: Wt::WApplication(env)
{
	setTheme(new Wt::WBootstrapTheme());
	useStyleSheet("style.css");
	root()->addWidget(config_wdgt = new Config_wdgt(session));
	root()->addWidget(new Wt::WBreak());
	root()->addWidget(stat_wdgt = new Stat_wdgt(session));
	root()->addWidget(new Wt::WBreak());
	root()->addWidget(line_wdgt = new Line_wdgt(session));
	root()->addWidget(new Wt::WBreak());
	root()->addWidget(prog_wdgt = new Prog_wdgt(session));
	enableUpdates(true);
	worker_connect(this);
}

App::~App()
{
	worker_disconnect(this);
}
