#include "worker.h"
#include "stat_wdgt.h"
#include <thread>
#include <iostream>
#include <Wt/WSignal>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <chrono>
#include "dbo.h"

using std::cerr;
using std::endl;
using std::chrono::system_clock;

static std::thread t;
static vector<App*> apps;
static vector<Wt::Signal<int, bool>*> signals;
static vector<bool> line_status;
static Session session;

static system_clock::time_point until_next_second()
{
	return system_clock::from_time_t(time(0)+2);
}

static int current_time()
{
	std::time_t now;
	now = time(0);
	
	std::tm midnight = *localtime(&now);
	midnight.tm_hour = 0; midnight.tm_min = 0; midnight.tm_sec = 0;
	return (int) std::difftime(now, std::mktime(&midnight));
}

static bool eval_program(Prog_db_obj prog)
{
	int now = current_time();

	if(now < prog.start_time || now >= prog.end_time)
		return false;
	int sum = prog.duration + prog.interval;
	int mul = ((now - prog.start_time)/sum);
	int ref_point = prog.start_time + mul * sum;
	return ref_point + prog.duration > now;
}

bool calc_state(Line_db_obj line)
{
	if(line.state == State::FORCE_ON)
		return true;
	else if(line.state == State::FORCE_OFF)
		return false;
	else {
		bool ret = false;
		std::vector<Prog_db_obj> progs = session.get_progs(line.m_id);
		for(auto prog : progs) {
			ret = ret || eval_program(prog);
		}
		return ret;
	}
}

static void emit(int i)
{
	using Wt::WApplication;
	size_t j;

	WApplication::UpdateLock l(apps[i]);
	for(j = 0; j < line_status.size(); j++) {
		signals[i]->emit(j, line_status[j]);
	}
	apps[i]->triggerUpdate();
}

static void usb_write()
{

}

static void work(void)
{
	std::vector<Line_db_obj> lines = session.get_lines();
	vector<bool>	  nLs = vector<bool>(lines.size());
	size_t 			  i;
	bool			  updated = false;

	for(i = 0; i < nLs.size(); i++) {
		nLs[i] = calc_state(lines[i]);
		if(nLs[i] != line_status[i])
			updated = true;
	}
	line_status.swap(nLs);
	if(updated)
		for(i = 0; i < apps.size(); i++)
			emit(i);
	usb_write();
}

static void schedule_work(void)
{
	std::this_thread::sleep_until(until_next_second());
	work();
	schedule_work();
}

void worker_start()
{
	std::vector<Line_db_obj> lines = session.get_lines();
	line_status = vector<bool>(lines.size(), false);
	t = std::thread(schedule_work);
}

void worker_connect(App* app)
{
	Stat_wdgt* stat_wdgt;
	Wt::Signal<int,bool>* sig = new Wt::Signal<int,bool>();

	apps.push_back(app);
	signals.push_back(sig);
	stat_wdgt = app->stat_wdgt;
	sig->connect(stat_wdgt, &Stat_wdgt::update_status); 
	emit(apps.size()-1);
} 

void worker_disconnect(App* app)
{
	int index = std::find(apps.begin(), apps.end(), app) - apps.begin();

	apps.erase(apps.begin() + index);
	delete signals[index];
	signals.erase(signals.begin() + index);
}
