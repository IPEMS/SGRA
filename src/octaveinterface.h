/*
 * octaveinterface.h
 *
 *  Created on: Feb 24, 2014
 *      Author: hansentm
 */

#ifndef OCTAVEINTERFACE_H_
#define OCTAVEINTERFACE_H_

#include <string>
#include <vector>
#include "timevector.h"

using namespace std;

class octave_interface {
public:
	octave_interface(string path, string outfolder);
	virtual ~octave_interface();

	//takes a vector (size NUM_LOADPOINTS) of time_vector (size NUM_TIME_INTERVALS)
	bool run_power_flow(vector<time_vector> * loads);
	void save_power_flow(vector<time_vector> * loads, string output_folder);

protected:
	string _outfolder;
};

#endif /* OCTAVEINTERFACE_H_ */
