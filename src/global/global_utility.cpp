/*
 * global_utility.cpp
 *
 *  Created on: Feb 26, 2014
 *      Author: hansentm
 */

#include "global_utility.h"


vector<time_vector> * getLoadpointMatrixFromCustomers(vector<customer *> * cust, int numLoadPoints, int numTimeIntervals, bool useBase){
	vector<time_vector> * ret;

	ret = new vector<time_vector>(numLoadPoints,time_vector(numTimeIntervals));

	for(vector<customer *>::iterator iCust = cust->begin(); iCust != cust->end(); iCust++){
		time_vector * tempCustLoad = useBase?(*iCust)->getTotalLoadBefore():(*iCust)->getTotalLoad();
		ret->at((*iCust)->getLoadpoint()) += ((*tempCustLoad)/1000.0);
		delete tempCustLoad;
	}

	return ret;
}


