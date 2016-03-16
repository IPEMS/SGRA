/*
 * customer.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: hansentm
 */

#include "customer.h"

customer::customer(int id, vector<smart_appliance *> * appliances, time_vector * base_load, IPsychology * psych, int load_point, time_vector * _rtp_fore){
	_id=id;
	_appliances=appliances;
	_base=base_load;
	_psych=psych;
	_loadpoint = load_point;
	this->_rtp_self_fore = clone_tv_p(_rtp_fore);
}

customer::~customer() {
	if(_base != NULL)
		delete _base;

	if(_psych != NULL)
		delete _psych;

	try{
		if(_rtp_self_fore != NULL)
			delete _rtp_self_fore;
	}catch(int e){}

	delete _appliances; //don't delete pointers, aggregator owns these
}

bool customer::isGarch(){
	return this->_rtp_self_fore != NULL;
}

time_vector * customer::getBaseLoad(){
	return new time_vector(*_base);
}

int customer::getLoadpoint(){
	return _loadpoint;
}

IPsychology * customer::getPsych(){
	return this->_psych;
}

//test customer::getTotalLoad()
time_vector * customer::getTotalLoad(){
	time_vector * ret = new time_vector(*_base);

	time_vector * _resch = getReschLoad();

	(*ret) += (*_resch);

	delete _resch;

	return ret;
}

time_vector * customer::getTotalLoadBefore(){
	time_vector * ret = new time_vector(*_base);

	time_vector * _resch = getReschLoadBefore();

	(*ret) += (*_resch);

	delete _resch;

	return ret;
}

//test customer::getReschLoad()
time_vector * customer::getReschLoad(){
	time_vector * ret = new time_vector(_base->getSize());

	for(vector<smart_appliance *>::iterator iter = _appliances->begin(); iter < _appliances->end(); iter++){
		if((*iter)->isRescheduled()){
			(*iter)->addPowerResch(ret);
		}else{
			(*iter)->addPowerOrig(ret);
		}
	}

	return ret;
}

time_vector * customer::getReschLoadBefore(){
	time_vector * ret = new time_vector(_base->getSize());

	for(vector<smart_appliance *>::iterator iter = _appliances->begin(); iter < _appliances->end(); iter++){
		(*iter)->addPowerOrig(ret);
	}

	return ret;
}

double customer::getSavings(time_vector * rtp, time_vector * cip){
	double retVal = 0.0;

	for(vector<smart_appliance *>::iterator iter = _appliances->begin(); iter < _appliances->end(); iter++){
		if((*iter)->isRescheduled())
			retVal += ((*iter)->getCostOrig(rtp) - (*iter)->getCostResch(cip));
	}

	return retVal;
}

int customer::getID(){
	return _id;
}

bool customer::allowResch(smart_appliance * app, time_vector * inc_price, time_vector * rtp_price, int time){
	bool retVal = false;

	if(app->getCustomer() == _id){ //sanity check
		retVal = (this->_rtp_self_fore != NULL) ? _psych->getGamma(app, inc_price, this->_rtp_self_fore, time) : _psych->getGamma(app, inc_price, rtp_price, time);
	}else{
		cout << "ERROR: customer ID mismatch" << endl;
	}

	return retVal;
}


