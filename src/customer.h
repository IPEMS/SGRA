/*
 * customer.h
 *
 *  Created on: Dec 3, 2013
 *      Author: hansentm
 */

#ifndef CUSTOMER_H_
#define CUSTOMER_H_

#include "assets/smartappliance.h"
#include "timevector.h"
#include "psychology/IPsychology.h"
#include <vector>

using namespace std;

class customer {
public:
	customer(int id, vector<smart_appliance *> * appliances, time_vector * base_load, IPsychology * psych, int load_point, time_vector * _rtp_fore);
	time_vector * getBaseLoad();
	time_vector * getTotalLoad();
	time_vector * getTotalLoadBefore();
	time_vector * getReschLoad();
	time_vector * getReschLoadBefore();
	IPsychology * getPsych();
	double getSavings(time_vector * rtp, time_vector * cip);
	int getID();
	int getLoadpoint();
	bool allowResch(smart_appliance * app, time_vector * inc_price, time_vector * rtp_price, int time);
	bool isGarch();
	virtual ~customer();

protected:
	time_vector * _base;
	vector<smart_appliance *> * _appliances;
	IPsychology * _psych;
	int _id;
	int _loadpoint;
	time_vector * _rtp_self_fore;
};


#endif /* CUSTOMER_H_ */
