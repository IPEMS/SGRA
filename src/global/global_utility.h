/*
 * global_utility.h
 *
 *  Created on: Feb 26, 2014
 *      Author: hansentm
 */

#ifndef GLOBAL_UTILITY_H_
#define GLOBAL_UTILITY_H_

#include <vector>
#include "../customer.h"
#include "../timevector.h"

using namespace std;

vector<time_vector> * getLoadpointMatrixFromCustomers(vector<customer *> * cust, int numLoadpoints, int numTimeIntervals, bool useBase);



#endif /* GLOBAL_UTILITY_H_ */
