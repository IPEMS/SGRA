/*
 * global_timer.cpp
 *
 *  Created on: Nov 30, 2013
 *      Author: hansentm
 */

#include "global_timer.h"
#include "timers.h"
#include <iostream>

timers * _timer;

void init_global_timer(){
	_timer = new timers();
}

int start_new_global_timer(string description){
	return _timer->new_timer(description);
}

void stop_global_timer(int index){
	_timer->stop_timer(index);
}

double glance_global_timer(int index){
	return _timer->current_time(index);
}

void del_global_timer(){
	delete _timer;
}

void print_global_timer(int index){
	cout << _timer->print_timer(index) << endl;
}

void save_global_timer(string filename){
	_timer->saveTimers(filename);
}
