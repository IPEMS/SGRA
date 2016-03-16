/*
 * timers.cpp
 *
 *  Created on: Nov 27, 2013
 *      Author: hansentm
 */

#include "timers.h"
#include <fstream>
#include <sstream>

using namespace std;

timers::timers() {
#ifndef PAR
	_system_start = boost::chrono::system_clock::now();
#else
	_system_start = omp_get_wtime();
#endif
}

timers::~timers() {

}

int timers::new_timer(string description){
	int retVal = _timers.size();
	_timers.push_back(time_info(description,&_system_start));
	_timer_state.push_back(true);
	return retVal;
}

void timers::stop_timer(int index){
	if(index < _timers.size() && _timer_state.at(index)){
		_timers.at(index).stop();
		_timer_state.at(index) = false;
	}
}

double timers::current_time(int index){
	double time = -1;

	if(index < _timers.size())
		time = _timers.at(index).get_current_time();

	return time;
}

string timers::print_timer(int index){
	stringstream ss;
	if(index < _timers.size())
		ss << _timers.at(index);

	return ss.str();
}

void timers::saveTimers(string filename){
	ofstream fout(filename.c_str());
	for(vector<time_info>::iterator iter = _timers.begin(); iter != _timers.end(); iter++){
		fout << *iter;
	}
	fout.close();
}

