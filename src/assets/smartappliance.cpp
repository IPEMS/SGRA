/*
 * smartappliance.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: hansentm
 */

#include "smartappliance.h"
#include "../global/global_rand.h"

smart_appliance::smart_appliance(int total_index, int cust_index, double power, int duration, int start_index, int avail_start, int avail_duration, int type) :
	_customer(cust_index), _power(power), _duration(duration), _start_time(start_index), _avail_start_time(avail_start), _avail_duration(avail_duration)
{
	_resch_time = start_index;
	_totalindex = total_index;
	_type = type;
}

smart_appliance::~smart_appliance() {

}

int smart_appliance::getType(){
	return _type;
}

int smart_appliance::getIndex(){
	return this->_totalindex;
}

int smart_appliance::getCustomer(){
	return this->_customer;
}

double smart_appliance::getPower(){
	return this->_power;
}

int smart_appliance::getDuration(){
	return this->_duration;
}

int smart_appliance::getStartTime(){
	return this->_start_time;
}

int smart_appliance::getAvailabilityStartTime(){
	return this->_avail_start_time;
}

int smart_appliance::getAvailabilityDuration(){
	return this->_avail_duration;
}

int smart_appliance::getRescheduledTime(){
	return _resch_time;
}

void smart_appliance::setRescheduledTime(int index){
	_resch_time = index;
}

bool smart_appliance::isRescheduled(){
	return _start_time != _resch_time;
}


////////////////////////////////////////////////////////////////////
// COST FUNCTIONS
////////////////////////////////////////////////////////////////////
double smart_appliance::getCostOrig(time_vector * value){
	return smart_appliance::_getCost(value,this->_start_time,this->_duration,this->_power);
}

double smart_appliance::getCostResch(time_vector * value){
	return smart_appliance::_getCost(value,this->_resch_time,this->_duration,this->_power);
}


/////////////////////////////////////////////////////////////////////
// POWER FUNCTIONS
/////////////////////////////////////////////////////////////////////
void smart_appliance::addPowerOrig(time_vector * ret){
	smart_appliance::_addPower(ret,this->_start_time,this->_duration,this->_power);
}

void smart_appliance::addPowerResch(time_vector * ret){
	smart_appliance::_addPower(ret,this->_resch_time,this->_duration,this->_power);
}

ostream& operator<< (ostream &out, smart_appliance &info){
	out << info._customer << "," << info._power << "," << info._duration << "," << info._start_time << "," << info._avail_start_time << "," << info._avail_duration << "," << info._resch_time;
	return out;
}


/////////////////////////////////////////////////////////////////////////////
///// SMART APPLIANCE GENE
/////////////////////////////////////////////////////////////////////////////
smart_appliance_gene::smart_appliance_gene(smart_appliance * app){
	_app = app;
	init();
}

smart_appliance_gene::~smart_appliance_gene(){
	//DON'T DELETE POINTERS, MANAGED ELSEWHERE
}

smart_appliance_gene::smart_appliance_gene(const smart_appliance_gene & source){
	_gene = source._gene;
	_app = source._app;
}

void smart_appliance_gene::mutate(){
	init();
}

void smart_appliance_gene::init(){
	gsl_rng * r;
	r = get_global_rand();

	_gene = gsl_rng_uniform(r);
}

smart_appliance * smart_appliance_gene::getApp(){
	return _app;
}

void smart_appliance_gene::applyGene(){
	_app->setRescheduledTime(getTime());
}

void smart_appliance_gene::resetGene(){
	_app->setRescheduledTime(_app->getStartTime());
}

int smart_appliance_gene::getTime(){
	return _app->getAvailabilityStartTime() + (int)((double)_app->getAvailabilityDuration() * _gene);
}
