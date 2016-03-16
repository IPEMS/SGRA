/*
 * PsychAlpha.cpp
 *
 *  Created on: Feb 12, 2014
 *      Author: hansentm
 */

#include "PsychAlpha.h"
#include <sstream>

PsychAlpha::PsychAlpha(map<int, double> * alphas){
	_alphas = alphas;
	_id = 1;
}

bool PsychAlpha::getGamma(smart_appliance * app, time_vector * inc_price, time_vector * rtp_price, int time){
	double _costOrig, _costNew;

	app->setRescheduledTime(time);

	_costOrig = app->getCostOrig(rtp_price);
	_costNew = app->getCostResch(inc_price);

	return _costNew <= (_alphas->at(app->getIndex()) * _costOrig);
}

PsychAlpha::~PsychAlpha() {delete _alphas;}

void PsychAlpha::save(ofstream * o){
	for(map<int, double>::iterator it = _alphas->begin(); it != _alphas->end(); it++)
		(*o) << it->first << ',' << it->second << endl;
}

double PsychAlpha::getValue(smart_appliance * app){
	return _alphas->at(app->getIndex());
}

PsychAlpha * PsychAlpha::load(ifstream * i){
	map<int, double> * alphas = new map<int, double>();

	string _tempLine;
	//parse load_info line
	while(getline((*i),_tempLine)){
		stringstream ss(_tempLine);
		string item;
		vector<string> elems;
		while (getline(ss, item, ',')) {
			elems.push_back(item);
		}
		(*alphas)[atoi(elems.at(0).c_str())] = atof(elems.at(1).c_str());
	}

	return new PsychAlpha(alphas);
}

