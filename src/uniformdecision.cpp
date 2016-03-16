/*
 * uniformdecision.cpp
 *
 *  Created on: Feb 25, 2014
 *      Author: hansentm
 */

#include "uniformdecision.h"

#include "global/global_rand.h"


uniform_decision::uniform_decision(vector<double> * vals) {
	_vals = vals;
	_r = get_global_rand();

	//normalize _vals
	double _sum = 0.0;
	for(vector<double>::iterator it = _vals->begin(); it != vals->end(); it++)
		_sum += *it;

	double running_total = 0.0;
	for(vector<double>::iterator it = _vals->begin(); it != vals->end(); it++){
		running_total += (*it)/_sum;
		(*it) = running_total;
	}
}

uniform_decision::~uniform_decision() {
	delete _vals;
}

int uniform_decision::getDecision(){
	double rand = gsl_rng_uniform(_r);

	int i = 0;

	do{
		if(rand <= _vals->at(i) )
			break;
		i++;
	}while(i < _vals->size());

	return i;
}

