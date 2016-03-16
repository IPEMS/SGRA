/*
 * timevector.h
 *
 *  Created on: Jan 24, 2014
 *      Author: hansentm
 */

#ifndef TIMEVECTOR_H_
#define TIMEVECTOR_H_

#include <string>
#include <vector>
#include <iostream>

using namespace std;

class time_vector {
public:
	time_vector(string filename, int num_intervals);
	time_vector(int num_intervals);
	//time_vector(vector<double> * vect);
	virtual ~time_vector();

	time_vector(const time_vector& source);
	time_vector& operator= (const time_vector& source);

	time_vector& operator+= (const time_vector& right);
	const time_vector operator+ (time_vector& right);

	time_vector& operator-= (const time_vector& right);
	const time_vector operator- (time_vector& right);

	time_vector& operator/ (double right);

	double getValue(int index);
	void setValue(int index, double value);

	int getSize();
	void save(string filename);

	friend ostream& operator<< (ostream &out, time_vector &tInfo);

	//Note: SWAPS IN PLACE, CREATE NEW TIME_VECTORS
	static void swap(time_vector * t1, time_vector * t2, int i1, int i2);

protected:
	vector<double> _vect;
};

time_vector * clone_tv_p(time_vector * x);

#endif /* TIMEVECTOR_H_ */
