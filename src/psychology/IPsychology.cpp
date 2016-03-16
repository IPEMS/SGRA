/*
 * IPsychology.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: hansentm
 */

#include "IPsychology.h"
#include "PsychAlpha.h"
#include "PsychRatio.h"

IPsychology::IPsychology() {

}

IPsychology::~IPsychology() {

}

void IPsychology::save(string filename){
	ofstream o(filename.c_str());

	o << this->_id << endl;

	this->save(&o);

	o.close();
}

IPsychology * IPsychology::load(string filename){
	IPsychology * ret;

	ifstream in(filename.c_str());
	string _s;
	getline(in,_s);

	switch(atoi(_s.c_str())){
	case 0:
		ret = PsychRatio::load(&in);
		break;
	case 1:
		ret = PsychAlpha::load(&in);
		break;
	}

	return ret;
}
