#include "worker.h"
#include "stat_wdgt.h"
#include <thread>
#include <iostream>
#include <Wt/WSignal>
#include <vector>
#include <algorithm>
#include <chrono>
#include "datatypes.h"
#include "dbo.h"

using std::cerr;
using std::endl;
using std::vector;

struct work_item {
	int line;
	int time;
};


static std::thread t;
static vector<App*> apps;
static vector<Wt::Signal<int, bool>*> signals;
static vector<bool> line_status;
static vector<bool> new_line_status;
static vector<double> radiation_accural;
static vector<int> time_accural;
static vector<work_item> work_queue;
static int last_tick;
static int time_d;
static Session session;
static bool new_client;

static const int max_concurrent_programs = 2;

static void emit_new_status(int i)
{
	using Wt::WApplication;
	size_t j;

	WApplication::UpdateLock l(apps[i]);
	for(j = 0; j < new_line_status.size(); j++) {
		signals[i]->emit(j, new_line_status[j]);
	}
	apps[i]->triggerUpdate();
}

static bool updated()
{
	for(size_t i = 0; i < line_status.size(); i++) {
		if(new_line_status[i] != line_status[i]) return true;
	}
	return false;
}

static void usb_write()
{

}

static void increment_time_radiation(vector<Line_db_obj> &lines)
{
	double prev_radiation = 0;
	for(size_t i = 0; i < lines.size(); i++) {
		auto line = lines[i];
		if(line.state == State::AUTO_FUNC) {
			radiation_accural[i] += prev_radiation * time_d;
			time_accural[i] += time_d;
		}
	}
}

static int find_prog_with_id(int id, vector<Prog_db_obj> &progs)
{
	for(size_t i = 0; i < progs.size(); i++) {
		if(progs[i].m_id == id) return i;
	}
	return -1;
}

static void queue_work(Config_db_obj &conf, 
		vector<Line_db_obj> &lines, 
		vector<Prog_db_obj> &progs)
{
	for(size_t i=0; i < lines.size(); i++) {
		auto line = lines[i];
		int prog_i = find_prog_with_id(line.program, progs);
		if(prog_i == -1) {
			time_accural[i] = 0;
			radiation_accural[i] = 0;
		} else {
			auto prog = progs[prog_i];
			bool proc = false;
			if((prog.radiation > 0.01) 
					&& (radiation_accural[i] > prog.radiation)) {
				proc = true;
			}
			if((prog.interval > 0) 
					&& (time_accural[i] > prog.interval)) {
				proc = true;
			}
			if(proc) {
				int time = prog.duration * (conf.multiplier * line.multiplier);
				work_queue.push_back(work_item{(int)i, time});
				time_accural[i] = 0;
				radiation_accural[i] = 0;
			}
		}
	}
}

static void manual_mode_lines(vector<Line_db_obj> &lines)
{
	for(size_t i = 0; i<lines.size(); i++) {
		auto line = lines[i];
		if(line.state == State::FORCE_ON)
			new_line_status[i] = true;
		if(line.state == State::FORCE_OFF)
			new_line_status[i] = false;
	}
}

static void increment_totals()
{
	int now = Time::current_time().to_int();
	time_d = now - last_tick;
	last_tick = now;
	if(now < 10) session.reset_totals();
	else {
		for(size_t i=0; i<line_status.size(); i++) {
			if(line_status[i]) 
				session.increment_total(i+1, time_d);
		}
	}
}

static void check_work_queue()
{
	cerr << endl << "queue :" << endl;
	for(size_t i=0; i < max_concurrent_programs; i++) {
		if(work_queue.size() > i) {
			cerr << '\t' << "line: " << work_queue[i].line << ", time: " << work_queue[i].time << endl;
		}
	}
	for(size_t i=max_concurrent_programs; i < work_queue.size(); i++) {
		cerr << '\t' << '\t' << "line: " << work_queue[i].line << ", time: " << work_queue[i].time << endl;
	}
	for(size_t i=0; i < max_concurrent_programs; i++) {
		if(work_queue.size() > i) {
			auto wi = work_queue[i];
			if(wi.time <= 0) {
				new_line_status[wi.line] = false;
				work_queue.erase(work_queue.begin()+i);
				i--;
				continue;
			}
			new_line_status[wi.line] = true;
			work_queue[i].time -= time_d;
		}

	}
}

static bool correct_time_frame(Config_db_obj conf)
{
	auto sunriset = sunriset_today();
	int now = Time::current_time().to_int();
	int rise = std::get<0>(sunriset).to_int();
	int set = std::get<1>(sunriset).to_int();

	if(std::get<2>(sunriset)) return true;
	if((now > rise) && ((now < set) || (set < rise))) return true;
	if((now < rise) && ((set < rise) && (set > now))) return true;
	return false;
}

static void work(void)
{
	vector<Line_db_obj> lines = session.get_lines();
	vector<Prog_db_obj> progs = session.get_progs();
	Config_db_obj 			 config = session.get_config();

	new_line_status.assign(lines.size(), false);

	increment_totals();
	if(correct_time_frame(config)) 
		increment_time_radiation(lines);
	queue_work(config, lines, progs);
	check_work_queue();
	manual_mode_lines(lines);

	if(updated() || new_client){
		usb_write();
		for(size_t j = 0; j < apps.size(); j++) {
			emit_new_status(j);
		}
		line_status.swap(new_line_status);
		new_client = false;
	}
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
	new_line_status = vector<bool>(lines.size(), false);
	time_accural = vector<int>(lines.size(), 0);
	radiation_accural = vector<double>(lines.size(), 0);
	work_queue = vector<work_item>(0);
	last_tick = Time::current_time().to_int();
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
	new_client = true;
} 

void worker_disconnect(App* app)
{
	int index = std::find(apps.begin(), apps.end(), app) - apps.begin();

	apps.erase(apps.begin() + index);
	delete signals[index];
	signals.erase(signals.begin() + index);
}
