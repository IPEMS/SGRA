/*
 * smartappliance.h
 *
 *  Created on: Dec 3, 2013
 *      Author: hansentm
 */

#ifndef SMARTAPPLIANCE_H_
#define SMARTAPPLIANCE_H_

#include <string>
#include "../timevector.h"
#include <iostream>

using namespace std;

class smart_appliance {
public:
	smart_appliance(int total_index, int cust_index, double power, int duration, int start_index, int avail_start, int avail_duration, int type);
	virtual ~smart_appliance();

	int getCustomer();
	double getPower();
	int getDuration();
	int getStartTime();
	int getAvailabilityStartTime();
	int getAvailabilityDuration();
	int getRescheduledTime();
	int getIndex();
	void setRescheduledTime(int index);
	bool isRescheduled();
	int getType();

	double getCostOrig(time_vector * value);
	double getCostResch(time_vector * value);

	void addPowerOrig(time_vector * ret);
	void addPowerResch(time_vector * ret);

	static string getLabels(){return string("customer,power,duration,orig_start,avail_start,avail_duration,resch_start");}

	friend ostream& operator<< (ostream &out, smart_appliance &info);

protected:
	int _customer;
	double _power;
	int _duration;
	int _start_time;
	int _avail_start_time;
	int _avail_duration;
	int _resch_time;
	int _type;

	int _totalindex;

private:
	static double _getCost(time_vector * value, int start, int duration, double power){
		double ret = 0.0;

		for(int i = start; i < (start+duration); i++)
			ret += ((value->getValue(i)*power)/4.0); //TODO INDECES_PER_HOUR

		return ret/100.0; // c/kWh to $/kWh
	}

	static void _addPower(time_vector * ret, int start, int duration, double power){
		for(int i = start; i < (start + duration); i++)
			ret->setValue(i,ret->getValue(i)+power);
	}
};

class smart_appliance_gene {
public:
	smart_appliance_gene(smart_appliance * app);
	virtual ~smart_appliance_gene();
	smart_appliance_gene(const smart_appliance_gene & source);

	void mutate();
	void init();

	void applyGene(); //sets smart_appliance
	void resetGene();

	smart_appliance * getApp();

	int getTime();

protected:
	double _gene; //[0-1] - represents range between availability
	smart_appliance * _app;
};

#endif /* SMARTAPPLIANCE_H_ */
