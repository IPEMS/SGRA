/*
 * global_timer.h
 *
 *  Created on: Nov 30, 2013
 *      Author: hansentm
 */

#ifndef GLOBAL_TIMER_H_
#define GLOBAL_TIMER_H_

#include <string>

using namespace std;

void init_global_timer();

int start_new_global_timer(string description);

void stop_global_timer(int index);

void del_global_timer();

void print_global_timer(int index);

double glance_global_timer(int index);

void save_global_timer(string filename);

#endif /* GLOBAL_TIMER_H_ */
