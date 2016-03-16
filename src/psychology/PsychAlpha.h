/*
 * PsychAlpha.h
 *
 *  Created on: Feb 12, 2014
 *      Author: hansentm
 */

#ifndef PSYCHALPHA_H_
#define PSYCHALPHA_H_

#include "IPsychology.h"
#include <map>

class PsychAlpha: public IPsychology {
public:
	PsychAlpha(map<int, double> * alphas);
	bool getGamma(smart_appliance * app, time_vector * inc_price, time_vector * rtp_price, int time);
	virtual ~PsychAlpha();
	virtual double getValue(smart_appliance * app);


	static PsychAlpha * load(ifstream * i);

protected:
	map<int, double> * _alphas;
	virtual void save(ofstream * o);
};

#endif /* PSYCHALPHA_H_ */
