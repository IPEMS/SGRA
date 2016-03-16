/*
 * PsychRatio.cpp
 *
 *  Created on: Feb 12, 2014
 *      Author: hansentm
 */

#include "PsychRatio.h"
#include <iostream>
#include <sstream>
#include <vector>

PsychRatio::PsychRatio(time_vector * limit){
	//cout << "WARNING: PsychRatio is no longer fit for use." << endl;
	_limit = limit;
}

bool PsychRatio::getGamma(smart_appliance * app, time_vector * inc_price, time_vector * rtp_price, int time){
	bool retVal = true;

	for(int i = time; i < (time + app->getDuration()); i++){
		if(inc_price->getValue(i) > _limit->getValue(i)){
			retVal = false;
			break;
		}
	}

	return retVal;
}

double PsychRatio::getValue(smart_appliance * app){
	//cout << "PsychRatio does not implement getValue correctly" << endl;
	return _limit->getValue(0);
}

PsychRatio::~PsychRatio() {
	delete _limit;
}

void PsychRatio::save(ofstream * o){
	for(int index = 0; index < _limit->getSize(); index++)
		(*o) << _limit->getValue(index) << endl;
}

PsychRatio * PsychRatio::load(ifstream * i){
	//janky way to do it. better would be to just save the psychology as a time_vector
	vector<double> _vect;

	string _tempLine;
	//parse load_info line
	while(getline((*i),_tempLine)){
		_vect.push_back(atof(_tempLine.c_str()));
	}

	time_vector * _limits = new time_vector(_vect.size());

	for(vector<double>::iterator it = _vect.begin(); it < _vect.end(); it++)
		_limits->setValue(it-_vect.begin(),*it);

	return new PsychRatio(_limits);
}

