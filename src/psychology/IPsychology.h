/*
 * IPsychology.h
 *
 *  Created on: Dec 3, 2013
 *      Author: hansentm
 */

#ifndef IPSYCHOLOGY_H_
#define IPSYCHOLOGY_H_

#include "../timevector.h"
#include "../assets/smartappliance.h"
#include <fstream>
#include <cstdlib>

/*
 * given all information, will the customer allow rescheduling
 */
class IPsychology {
public:
	virtual ~IPsychology();
	virtual bool getGamma(smart_appliance * app, time_vector * inc_price, time_vector * rtp_price, int time)=0;
	virtual double getValue(smart_appliance * app)=0;
	virtual void save(string filename);
	static IPsychology * load(string filename);

protected:
	virtual void save(ofstream * o)=0;

	IPsychology();
	int _id;
};

#endif /* IPSYCHOLOGY_H_ */
