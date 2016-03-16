/*
 * octaveinterface.cpp
 *
 *  Created on: Feb 24, 2014
 *      Author: hansentm
 */

#include "octaveinterface.h"

#include <octave-3.8.1/octave/oct.h>
#include <octave-3.8.1/octave/octave.h>
#include <octave-3.8.1/octave/parse.h>
#include <octave-3.8.1/octave/toplev.h>

#include <sstream>


#define _MP_INIT		"matpower_init"
#define _MP_RUN			"run_power_flow"
#define _MP_SAVE		"save_power_flow"

#define _NUM_LOADPOINTS	26

//LOCAL FUNCTION
Matrix * _vector_to_matrix(vector<time_vector> * loads);

//initializes Octave and adds matpower interface folder to the Octave path
octave_interface::octave_interface(string path, string outfolder) {
	string_vector argv (2);
	argv(0) = "embedded";
	argv(1) = "-q";

	octave_main (2, argv.c_str_vec (), 1);

	//add the path to the .m files
	octave_value_list in;
	in(0) = path.c_str();
	feval("addpath",in,1);

	_outfolder = outfolder;

	//make customer directory recursively
	stringstream ss;
	ss << "mkdir -p -m 0777 " << _outfolder;
	int temp = system(ss.str().c_str());
}

octave_interface::~octave_interface() {

}

//checks powerflow on the system given &loads
bool octave_interface::run_power_flow(vector<time_vector> * loads){
	octave_value_list in;

	Matrix * _l = _vector_to_matrix(loads);

	in(0) = octave_value(*_l);

	octave_value _success = feval(_MP_RUN,in,1)(0);

	delete _l;

	return (_success.int_value() == 1);
}

void octave_interface::save_power_flow(vector<time_vector> * loads, string output_folder){
	octave_value_list in;

	//make customer directory recursively
	stringstream ss;
	ss << "mkdir -p -m 0777 " << _outfolder << output_folder;
	int temp = system(ss.str().c_str());

	Matrix * _l = _vector_to_matrix(loads);

	ss.str("");
	ss << _outfolder << output_folder;

	in(0) = octave_value(*_l);
	in(1) = ss.str().c_str();

	feval(_MP_SAVE,in,2)(0);

	delete _l;
}


///////////////////////////////////////////////////////////////////////
/// LOCAL FUNCTIONS
///////////////////////////////////////////////////////////////////////

//NOTE: assumes each vector<double> is the same size, or at least no smaller than element 0
Matrix * _vector_to_matrix(vector<time_vector> * loads){
	Matrix * ret = new Matrix(loads->at(0).getSize(),loads->size()); //transpose the matrix

	for(int j = 0; j < loads->size(); j++)
		for(int i = 0; i < loads->at(0).getSize(); i++)
			(*ret)(i,j) = loads->at(j).getValue(i);

	return ret;
}
