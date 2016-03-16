/*
 * aggregator.cpp
 *
 *  Created on: Dec 3, 2013
 *      Author: hansentm
 */

#include "aggregator.h"
#include <fstream>
#include <iostream>
#include <sstream>

#include "global/global_utility.h"
#include "global/global_timer.h"

/*
 * ORDER OF CREATION
 *
 * create all customers - generate base load for each
 * for each asset type
 * 		generate asset (include pointer to customer in asset)
 * 		add to asset * list
 * 		add to customer as *
 */

/*
 * DELETION OF ASSETS
 *
 * don't delete customer from asset * list
 * don't delete asset from customer * list
 * delete from aggregator lists ONLY
 */

#ifdef INCLUDE_POWERWORLD
aggregator::aggregator(vector<customer * > * customers, vector<smart_appliance *> * assets, time_vector * spot_fore, time_vector * spot_act,
		               time_vector * rtp_fore, time_vector * rtp_act, octave_interface * powerflow, int numLoadPoints){
#else
aggregator::aggregator(vector<customer * > * customers, vector<smart_appliance *> * assets, time_vector * spot_fore, time_vector * spot_act,
					   time_vector * rtp_fore, time_vector * rtp_act, int numLoadPoints){
#endif
	_customers= customers;
	_assets = assets;
	_spot_f = spot_fore;
	_spot_a = spot_act;
	_rtp_f = rtp_fore;
	_rtp_a = rtp_act;
	_ga = NULL;
	_best_solution = NULL;
#ifdef INCLUDE_POWERWORLD
	_powerflow = powerflow;
#endif
	_numLoadPoints = numLoadPoints;
}

aggregator::~aggregator() {
	for(vector<smart_appliance *>::iterator it = _assets->begin(); it != _assets->end();){
		delete * it;
		it = _assets->erase(it);
	}
	delete _assets;

	for(vector<customer *>::iterator it = _customers->begin(); it != _customers->end();){
		delete * it;
		it = _customers->erase(it);
	}
	delete _customers;

	delete _spot_f;
	delete _spot_a;
	delete _rtp_f;
	delete _rtp_a;

	if(_ga != NULL)
		delete _ga;

	if(_best_solution != NULL)
		delete _best_solution;
#ifdef INCLUDE_POWERWORLD
	if(_powerflow != NULL)
		delete _powerflow;
#endif
}

void aggregator::setGA(ga * g){
	_ga = g;
	_ga->initialize_population(_assets, _rtp_f, _spot_f, _customers);
}

void aggregator::run_heuristics(string folder){
	if(_ga != NULL){
		/*
		 * RUN THE HEURISTICS AND GET THE SOLUTIONS
		 */
		cout << "Aggregator: running GA" << endl;
		int _gaTime = start_new_global_timer("Genetic Algorithm");
		_best_solution = _ga->run();
		stop_global_timer(_gaTime);
		_best_solution->first.applyChromosomeToAssets();
		cout << "Aggregator: Final Objective = $" << _best_solution->second << endl;
		print_global_timer(_gaTime);

		/*
		 * DATA DECOMPOSITION
		 */
		cout << "Aggregator: decomposing data" << endl;
		//CIP
		time_vector * _cip;
		_cip = _best_solution->first.getCIP();

		//out_3d_data - load, profit_f, profit_a, number, B_f, B_a, N_f, N_a, S_f, S_a
		//load_info - [cust,resch,orig,final,avail_s,avail_d,pow,dur,profit_f, profit_a]
		vector<load_info> * _loads;
		_loads = _getLoadInfo(_cip);

		//time_vectors - before_total, after_total, before_sch, after_sch
		time_vector * _bef_tot, * _aft_tot, * _bef_sch, * _aft_sch;
		_bef_tot = new time_vector(_rtp_f->getSize());
		_aft_tot = new time_vector(_rtp_f->getSize());
		_bef_sch = new time_vector(_rtp_f->getSize());
		_aft_sch = new time_vector(_rtp_f->getSize());

		//open customer savings file, "out_cust_savings.txt"
		ofstream o;
		o.open((folder + "out_cust_savings.txt").c_str());
		o << "forecast,actual" << endl; //column headers

		//if(_powerflow != NULL) - output powerflow for final chromosome and original solution
#ifdef INCLUDE_POWERWORLD
		if(_powerflow != NULL){
			vector<time_vector> * _tempLPs;

			//run powerflow on base load
			cout << "Testing solution with powerflow, base: ";
			int _pfTimeBase = start_new_global_timer("Powerflow - Base");
			_tempLPs = getLoadpointMatrixFromCustomers(_customers,_numLoadPoints,_spot_a->getSize(),true);
			cout << _powerflow->run_power_flow(_tempLPs) << endl;
			stop_global_timer(_pfTimeBase);
			cout << "Saving powerflow...";
			_powerflow->save_power_flow(_tempLPs,string("base/"));
			cout << "done." << endl;
			delete _tempLPs;

			//run powerflow on resch load
			cout << "Testing solution with powerflow, resch: ";
			int _pfTimeResch = start_new_global_timer("Powerflow - Resch");
			_tempLPs = getLoadpointMatrixFromCustomers(_customers,_numLoadPoints,_spot_a->getSize(),false);
			cout << _powerflow->run_power_flow(_tempLPs) << endl;
			stop_global_timer(_pfTimeResch);
			cout << "Saving powerflow...";
			_powerflow->save_power_flow(_tempLPs,string("resch/"));
			cout << "done." << endl;
			delete _tempLPs;

			print_global_timer(_pfTimeBase);
			print_global_timer(_pfTimeResch);
		}
		//TODO output loadpoint information here
#endif

		for(vector<customer *>::iterator it = _customers->begin(); it != _customers->end(); it++){
			time_vector * _cbt, * _cat, * _cbs, * _cas, * _cbase;

			//get customer load profiles
			_cbase = (*it)->getBaseLoad();
			_cbt = (*it)->getTotalLoadBefore();
			_cat = (*it)->getTotalLoad();
			_cbs = (*it)->getReschLoadBefore();
			_cas = (*it)->getReschLoad();

			//add customer load profiles to total
			*_bef_tot += *_cbt;
			*_aft_tot += *_cat;
			*_bef_sch += *_cbs;
			*_aft_sch += *_cas;

			//make customer directory recursively
			stringstream ss;
			ss << folder << "cust/" << it-_customers->begin() << "/";
			string cust_folder = ss.str();
			ss.str("");
			ss << "mkdir -p -m 0777 " << cust_folder;
			int temp = system(ss.str().c_str());

			//SAVE CUSTOMER LOAD PROFILES TO FILE
			_cbt->save(cust_folder + "tot_bef.txt");
			_cat->save(cust_folder + "tot_aft.txt");
			_cbs->save(cust_folder + "sch_bef.txt");
			_cas->save(cust_folder + "sch_aft.txt");
			_cbase->save(cust_folder + "base.txt");;

			//CALCULATE AND OUTPUT CUSTOMER SAVINGS (INDIVIDUAL AND TOTAL (MIN,MAX,AVERAGE,SUM))
			o << (*it)->getSavings(this->_rtp_f, _cip) << "," << (*it)->getSavings(this->_rtp_a, _cip) << endl;

			//output psychology model (add virtual save/load=0 to IPsychology interface)
			(*it)->getPsych()->save(cust_folder + "psych.txt");
#ifdef INCLUDE_POWERWORLD
			//TODO output loadpoint (easiest way is to add to load_profile
#endif

			//clean memory
			delete _cbase;
			delete _cbt;
			delete _cat;
			delete _cbs;
			delete _cas;
		}

		//close customer savings file, "out_cust_savings.txt"
		o.close();

		/*
		 * OUTPUT REST OF DATA
		 */
		cout << "Aggregator: outputting data to " << folder << endl;

		//CIP
		_cip->save(folder + "out_cip.txt");

		//GA
		_ga->saveEvolution(folder + "out_ga_evolution.txt");

		//3d_
		_save3dData(folder + "out_3d_data.txt", _loads);

		//load_info
		_saveLoadInfo(folder + "out_load_info.txt", _loads);

		//out_
		_bef_tot->save(folder + "out_before_total.txt");
		_aft_tot->save(folder + "out_after_total.txt");
		_bef_sch->save(folder + "out_before_sch.txt");
		_aft_sch->save(folder + "out_after_sch.txt");

		//objective
		_saveObjective(folder + "out_objective.txt", _best_solution->first);

		/*
		 * CLEAN MEMORY
		 */
		delete _ga;
		_ga = NULL;
		delete _cip;

		delete _bef_tot;
		delete _aft_tot;
		delete _bef_sch;
		delete _aft_sch;

		delete _loads;
	}else{
		cout << "INITIALIZE GA FIRST BEFORE RUNNING AGGREGATOR" << endl;
	}
}

vector<load_info> * aggregator::_getLoadInfo(time_vector * cip){
	vector<load_info> * ret;
	ret = new vector<load_info>();

	for(vector<smart_appliance *>::iterator it = this->_assets->begin(); it != this->_assets->end(); it++)
		ret->push_back(load_info(*it, _spot_f, _spot_a, cip, _rtp_a, (this->_customers->at((*it)->getCustomer())->isGarch()), this->_customers->at((*it)->getCustomer())->getPsych()->getValue(*it) ));

	return ret;
}

void aggregator::_saveLoadInfo(string filename, vector<load_info> * loads){
	ofstream out(filename.c_str());

	out << load_info::getLabels() << endl;

	for(vector<load_info>::iterator it = loads->begin(); it != loads->end(); it++)
		out << *it << endl;

	out.close();
}

#define NUM_BINS			96
#define NUM_BINS_SQUARED	9216 //TODO INDECES_PER_HOUR

void aggregator::_save3dData(string filename, vector<load_info> * loads){
	ofstream out(filename.c_str());

	out << "from,to,load,P_f,P_a,num,B_f,B_a,N_f,N_a,S_f,S_a" << endl;

	double load[NUM_BINS_SQUARED] = {}, p_f[NUM_BINS_SQUARED] = {},p_a[NUM_BINS_SQUARED] = {},
			b_f[NUM_BINS_SQUARED] = {},b_a[NUM_BINS_SQUARED] = {},s_f[NUM_BINS_SQUARED] = {},
			s_a[NUM_BINS_SQUARED] = {},n_f[NUM_BINS_SQUARED] = {},n_a[NUM_BINS_SQUARED] = {};
	int num[NUM_BINS_SQUARED] = {};

	for(vector<load_info>::iterator it = loads->begin(); it != loads->end(); it++){
		if((it->__index >= 0) && (it->__index < NUM_BINS_SQUARED)){
			num[it->__index]++;

			p_f[it->__index] += it->__p_f;
			p_a[it->__index] += it->__p_a;
			b_f[it->__index] += it->__B_f;
			b_a[it->__index] += it->__B_a;
			s_f[it->__index] += it->__S_f;
			s_a[it->__index] += it->__S_a;
			n_f[it->__index] += it->__N_f;
			n_a[it->__index] += it->__N_a;
		}

		for(vector<int>::iterator __it = it->__load_indeces.begin(); __it != it->__load_indeces.end(); __it++) //__it points to load indeces
			load[*__it] += it->__load;
	}

	for(int i = 0; i < NUM_BINS; i++){
		for(int j = 0; j < NUM_BINS; j++){
			int _index = i * NUM_BINS + j;
			out << ((double)i)/4.0 << "," << ((double)j)/4.0 << "," << load[_index] << "," << p_f[_index] << "," << p_a[_index] << ","
					 << num[_index] << "," << b_f[_index] << "," << b_a[_index] << "," << n_f[_index] << "," << n_a[_index] << ","
					 << s_f[_index] << "," << s_a[_index] << endl;
		}
	}

	out.close();
}

void aggregator::_saveObjective(string filename, chromosome& c){
	ofstream out(filename.c_str());

	out << "forecast,actual" << endl;
	out << c.getP(_spot_f) << "," << c.getP(_spot_a) << endl;

	out.close();
}
