/*
 * timers.h
 *
 *  Created on: Nov 27, 2013
 *      Author: hansentm
 *      Tested: Works as of 11/30/13
 */

#ifndef TIMERS_H_
#define TIMERS_H_

#include <vector>
#include <string>
#include <iostream>

#ifndef PAR
#include <boost/chrono/include.hpp>
#else
#include <omp.h>
#endif

using namespace std;

#ifndef PAR
static double _get_duration(boost::chrono::system_clock::time_point first,boost::chrono::system_clock::time_point second){
	boost::chrono::duration<double> duration;

	duration = second-first;

	return duration.count();
}
#else
static double _get_duration(double first, double second){
	return second - first;
}
#endif

struct time_info;

class timers {
public:
	timers();
	virtual ~timers();

	int new_timer(string description);
	void stop_timer(int index);
	string print_timer(int index);
	double current_time(int index);

	void saveTimers(string filename);

protected:
	vector<time_info> _timers;
	vector<bool> _timer_state;
#ifndef PAR
	boost::chrono::system_clock::time_point _system_start;
#else
	double _system_start;
#endif
};

struct time_info{
public:
#ifndef PAR
	time_info(string info,boost::chrono::system_clock::time_point * global_start):_info(info){
		_chrono_start = boost::chrono::system_clock::now();
#else
	time_info(string info,double * global_start):_info(info){
		_chrono_start = omp_get_wtime();
#endif
		_duration = 0;
		_start_time = _get_duration(*global_start,_chrono_start);
	}

	void stop(){
		_duration = this->get_current_time();
	}

	double get_current_time(){
#ifndef PAR
		return _get_duration(_chrono_start,boost::chrono::system_clock::now());
#else
		return _get_duration(_chrono_start,omp_get_wtime());
#endif
	}

	double get_duration(){
		return _duration;
	}

	//overload stream out (<<) operator
	friend ostream& operator<< (ostream &out, time_info &tInfo){
		out << "\"" << tInfo._info << "\"," << tInfo._start_time << "," << tInfo._duration << endl;
		return out;
	}

protected:
	string _info;
	double _duration;
	double _start_time;
private:
#ifndef PAR
	boost::chrono::system_clock::time_point _chrono_start;
#else
	double _chrono_start;
#endif

};

#endif /* TIMERS_H_ */
