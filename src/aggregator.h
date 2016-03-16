/*
 * aggregator.h
 *
 *  Created on: Dec 3, 2013
 *      Author: hansentm
 */

#ifndef AGGREGATOR_H_
#define AGGREGATOR_H_

#include <vector>
#include <utility>

#include "customer.h"
#include "assets/smartappliance.h"
#include "timevector.h"
#include "heuristics/ga.h"
#ifdef INCLUDE_POWERWORLD
#include "octaveinterface.h"
#endif

using namespace std;

struct load_info;

class aggregator {
public:
#ifdef INCLUDE_POWERWORLD
	aggregator(vector<customer * > * customers, vector<smart_appliance *> * assets, time_vector * spot_fore, time_vector * spot_act, time_vector * rtp_fore, time_vector * rtp_act, octave_interface * powerflow, int numLoadPoints);
#else
	aggregator(vector<customer * > * customers, vector<smart_appliance *> * assets, time_vector * spot_fore, time_vector * spot_act, time_vector * rtp_fore, time_vector * rtp_act, int numLoadPoints);
#endif
	virtual ~aggregator();

	void setGA(ga * g);

	void run_heuristics(string folder);

protected:
	vector<customer * > * _customers;
	vector<smart_appliance *> * _assets;
	time_vector * _spot_f, * _spot_a, * _rtp_f, * _rtp_a;
	ga * _ga;
	pair<chromosome,double> * _best_solution;
#ifdef INCLUDE_POWERWORLD
	octave_interface * _powerflow;
#endif
	int _numLoadPoints;

	vector<load_info> * _getLoadInfo(time_vector * cip);
	void _saveLoadInfo(string filename, vector<load_info> * loads);
	void _save3dData(string filename, vector<load_info> * loads);
	void _saveObjective(string filename, chromosome& c);

};

struct load_info{
public:
	int customer;
	bool rescheduled;
	bool garch;
	double orig;
	double final;
	double av_start;
	double av_dur;
	double pow_kw;
	double dur_hr;
	double profit_fore;
	double profit_act;
	double psych;
	double savings;

	//for my use only
	double __load, __p_f, __p_a, __B_f, __B_a, __N_f, __N_a, __S_f, __S_a;
	int __index;
	vector<int> __load_indeces;

	load_info(smart_appliance * asset, time_vector * spot_fore, time_vector * spot_act, time_vector * cip, time_vector * rtp_act, bool garch, double psych){
		customer = asset->getCustomer();
		this->garch = garch;
		rescheduled = asset->isRescheduled();
		orig = ((double)asset->getStartTime())/4.0; //TODO INDECES_PER_HOUR
		final = ((double)asset->getRescheduledTime())/4.0;
		av_start = ((double)asset->getAvailabilityStartTime())/4.0;
		av_dur = ((double)asset->getAvailabilityDuration())/4.0;
		pow_kw = asset->getPower();
		dur_hr = ((double)asset->getDuration())/4.0;
		this->psych = psych;

		if(rescheduled)
			savings = asset->getCostOrig(rtp_act) - asset->getCostResch(cip);
		else
			savings = 0.0;

		if(asset->isRescheduled()){
			__N_f = asset->getCostOrig(spot_fore);
			__S_f = asset->getCostResch(cip);
			__B_f = asset->getCostResch(spot_fore);
			__p_f = profit_fore = __N_f + __S_f - __B_f;

			__N_a = asset->getCostOrig(spot_act);
			__S_a = asset->getCostResch(cip);
			__B_a = asset->getCostResch(spot_act);
			__p_a = profit_act = __N_a + __S_a - __B_a;
		}else{
			__N_f = 0;
			__S_f = 0;
			__B_f = 0;
			__p_f = profit_fore = 0;

			__N_a = 0;
			__S_a = 0;
			__B_a = 0;
			__p_a = profit_act = 0;
		}

		__load = pow_kw/1000.0;
		__index = asset->getStartTime()*96 + asset->getRescheduledTime(); //TODO INDECES_PER_HOUR

		if((__index < 0) || (asset->getStartTime() >= 96) || (asset->getRescheduledTime() >= 96) || (asset->getStartTime() < 0) || (asset->getRescheduledTime() < 0) || (__index >= 9216))
			__index = -1; //invalid index

		__load_indeces.clear();
		for(int t = 0; t < asset->getDuration(); t++){
			int _tmp = ((asset->getStartTime()+t)*96) + (asset->getRescheduledTime()+t); //TODO INDECES_PER_HOUR
			if((_tmp >= 0) && ((asset->getStartTime()+t) < 96) && ((asset->getRescheduledTime()+t) < 96) && (_tmp < 9216))
				__load_indeces.push_back(_tmp);
		}
	}

	static string getLabels(){
		return string("customer,resch,orig,final,av_start,av_dur,pow(kW),dur(hr),profit_f,profit_a,garch,psych,cust_savings");
	}

	friend ostream& operator<<(ostream & out, load_info & load){
		out << load.customer << "," << (int)load.rescheduled << "," << load.orig << "," << load.final
				<< "," << load.av_start << "," << load.av_dur << "," << load.pow_kw << "," << load.dur_hr
				<< "," << load.profit_fore << "," << load.profit_act << "," << load.garch << "," << load.psych << "," << load.savings;
		return out;
	}
};

#endif /* AGGREGATOR_H_ */
