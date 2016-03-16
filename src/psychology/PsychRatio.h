/*
 * PsychRatio.h
 *
 *  Created on: Feb 12, 2014
 *      Author: hansentm
 */

#ifndef PSYCHRATIO_H_
#define PSYCHRATIO_H_

#include "IPsychology.h"

class PsychRatio : public IPsychology {
public:
	PsychRatio(time_vector * limit);
	bool getGamma(smart_appliance * app, time_vector * inc_price, time_vector * rtp_price, int time);
	virtual double getValue(smart_appliance * app);
	virtual ~PsychRatio();

	static PsychRatio * load(ifstream * i);

protected:
	virtual void save(ofstream * o);
	time_vector * _limit;
};

#endif /* PSYCHRATIO_H_ */
