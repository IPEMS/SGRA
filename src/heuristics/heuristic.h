/*
 * heuristic.h
 *
 *  Created on: Mar 28, 2012
 *      Author: hansentm
 */

#ifndef HEURISTIC_H_
#define HEURISTIC_H_

#include <string>
#include "../xml_reader/rapidxml.hpp"

#ifndef PAR
#include <boost/chrono/include.hpp>
#else
#include <omp.h>
#endif

using namespace std;

class heuristic{
public:
	virtual ~heuristic() {}

protected:
	heuristic();
	string id;

	#ifndef PAR
	boost::chrono::duration<double> runtime;
	#else
	double runtime;
	#endif
};

class heuristic_parser{
public:
	virtual ~heuristic_parser();
	virtual heuristic * parse(rapidxml::xml_node<> * node) = 0;

protected:
	heuristic_parser();
};

#endif /* HEURISTIC_H_ */
