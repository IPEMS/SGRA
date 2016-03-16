/*
 * input.h
 *
 *  Created on: Apr 11, 2012
 *      Author: hansentm
 */

#ifndef INPUT_H_
#define INPUT_H_

#include <fstream>
#include "xml_reader/rapidxml.hpp"
#include "aggregator.h"

using namespace std;

aggregator * parseXmlFile(ifstream * f);

double getDoubleFromNode(rapidxml::xml_node<> * node);
int getIntFromNode(rapidxml::xml_node<> * node);
string getStringFromNode(rapidxml::xml_node<> * node);

#endif /* INPUT_H_ */
