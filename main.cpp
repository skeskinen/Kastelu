#include "app.h"
#include "worker.h"
#include <iostream>
#include <Wt/WEnvironment>

Wt::WApplication* spawn_app(const Wt::WEnvironment& env)
{
	return new App(env);
}

int main(int argc, char* argv[])
{
	Wt::WString::setDefaultEncoding(Wt::CharEncoding::UTF8);
	worker_start();
	return Wt::WRun(argc, argv, &spawn_app);
}

