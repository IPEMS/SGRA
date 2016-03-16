/*
 * uniformdecision.h
 *
 *  Created on: Feb 25, 2014
 *      Author: hansentm
 */

#ifndef UNIFORMDECISION_H_
#define UNIFORMDECISION_H_

#include <vector>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

using namespace std;

class uniform_decision {
public:
	uniform_decision(vector<double> * values);
	virtual ~uniform_decision();

	int getDecision();

protected:
	vector<double> * _vals;
	gsl_rng * _r; //DO NOT DELETE
};

#endif /* UNIFORMDECISION_H_ */
