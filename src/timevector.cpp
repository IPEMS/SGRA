/*
 * timevector.cpp
 *
 *  Created on: Jan 24, 2014
 *      Author: hansentm
 */

#include "timevector.h"

#include <fstream>
#include <cstdlib>
#include <algorithm>

time_vector::time_vector(string filename, int num_intervals){
	ifstream fs;
	string templine;

	fs.open(filename.c_str(), ifstream::in);

	uint temp_counter = 0;
	while(getline(fs,templine)){
		double test = atof(templine.c_str());
		_vect.push_back(atof(templine.c_str()));
		temp_counter++;
	}

	if(temp_counter != num_intervals)
		cout << "Mismatch in Intervals - " << filename << endl;

	fs.close();
}

time_vector::time_vector(int num_intervals){
	for(int i = 0; i < num_intervals; i++)
		_vect.push_back(0.0);
}

time_vector::~time_vector() {
	//no dynamic memory
	_vect.clear();
}

time_vector::time_vector(const time_vector& source){
	_vect = vector<double>(source._vect);
}

time_vector& time_vector::operator= (const time_vector& source){
	if(this == &source) //check self assignment
		return *this;

	_vect.clear();
	_vect = vector<double>(source._vect);

	return *this;
}

time_vector& time_vector::operator+= (const time_vector& right){
	for(vector<double>::iterator iter = _vect.begin(); iter < _vect.end(); iter++){
		(*iter) += right._vect.at(iter-_vect.begin());
	}
	return *this;
}

time_vector& time_vector::operator/ (double right){
	for(vector<double>::iterator iter = _vect.begin(); iter < _vect.end(); iter++){
		(*iter) /= right;
	}

	return *this;
}

const time_vector time_vector::operator+ (time_vector& right){
	time_vector ret = *this;
	ret += right;
	return ret;
}

time_vector& time_vector::operator-= (const time_vector& right){
	for(vector<double>::iterator iter = _vect.begin(); iter < _vect.end(); iter++){
		(*iter) -= right._vect.at(iter-_vect.begin());
	}
	return *this;
}

const time_vector time_vector::operator- (time_vector& right){
	time_vector ret = *this;
	ret -= right;
	return ret;
}

double time_vector::getValue(int index){
	/*if(index >= 0 && index < _vect.size())
		return _vect.at(index);
	else
		return 0.0;*/

	double retVal = 0.0;

	//if value is outside of the index, return the closest element (0 or size-1)
	if(index < 0)
		retVal = _vect.at(0);
	else if(index >= _vect.size())
		retVal = _vect.at(_vect.size()-1);
	else
		retVal = _vect.at(index);

	return retVal;
}
void time_vector::setValue(int index, double value){
	if(index >= 0 && index < _vect.size())
		_vect.at(index) = value;
}

int time_vector::getSize(){
	return _vect.size();
}

void time_vector::save(string filename){
	ofstream fout(filename.c_str());

	for(vector<double>::iterator iter = _vect.begin(); iter < _vect.end(); iter++)
		fout << (*iter) << endl;

	fout.close();
}

ostream& operator<< (ostream &out, time_vector &tInfo){
	for(vector<double>::iterator iter = tInfo._vect.begin(); iter < tInfo._vect.end(); iter++)
		out << (*iter) << endl;

	return out;
}

void time_vector::swap(time_vector * t1, time_vector * t2, int i1, int i2){
	//ensure proper parameters
	i1 = max(i1,0);
	i2 = min(i2,t1->getSize());

	//swap elements x, i1 <= x < i2
	for(int i = i1; i < i2; i++)
		iter_swap(t1->_vect.begin()+i,t2->_vect.begin()+i);
}

time_vector * clone_tv_p(time_vector * x){

	if(x == NULL)
		return NULL;

	time_vector * ret;

	ret = new time_vector(x->getSize());

	for(int i = 0; i < x->getSize(); i++)
		ret->setValue(i,x->getValue(i));

	return ret;

}

