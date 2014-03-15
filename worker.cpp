#include "worker.h"
#include "config.h"
#include "stat_wdgt.h"
#include <thread>
#include <iostream>
#include <Wt/WSignal>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <chrono>

using std::cerr;
using std::endl;
using std::chrono::system_clock;

static std::thread t;
static vector<App*> apps;
static vector<Wt::Signal<int, bool>*> signals;
static vector<bool> line_status;

static system_clock::time_point until_next_second()
{
	return system_clock::from_time_t(time(0)+2);
}

static bool eval_program(line_oper_prog p)
{
	std::time_t now;
	std::tm now_time, start_time, end_time;
	int seconds;

	time(&now);  /* get current time; same as: now = time(NULL)  */

	now_time = *localtime(&now);
	start_time = *localtime(&now);
	end_time = *localtime(&now);

	if(now_time.tm_hour < p.start_hour || now_time.tm_hour >= p.end_hour)
		return false;

	start_time.tm_hour = p.start_hour; start_time.tm_min = 0; start_time.tm_sec = 0;
	end_time.tm_hour = p.end_hour; end_time.tm_min = 0; end_time.tm_sec = 0;

	seconds = (int)std::difftime(now, std::mktime(&start_time));
	return (seconds % p.interval) < p.duration;
}

bool calc_state(cfg_chunk c)
{
	bool ret = false;

	if(c.state == line_oper_state::FORCE_ON)
		return true;
	if(c.state == line_oper_state::FORCE_OFF)
		return false;
	for(line_oper_prog p : c.progs) 
	{
		ret = ret || eval_program(p);
	}
	return ret;
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
	vector<cfg_chunk> cfg = clone_config();
	vector<bool>	  nLs = vector<bool>(line_count());
	size_t 			  i;
	bool			  updated = false;

	for(i = 0; i < nLs.size(); i++) {
		nLs[i] = calc_state(cfg[i]);
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
	line_status = vector<bool>(line_count(), false);
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
