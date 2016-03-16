/*
 * ga.cpp
 *
 *  Created on: Mar 28, 2012
 *      Author: hansentm
 */

#include "ga.h"

#include <cmath>
#include <algorithm>
#include <fstream>

#include "../global/global_rand.h"
#include "../global/global_timer.h"
#include "../input.h"

//ranked list, best is always element zero
#define ELITE_SOLUTION	(int)0

//LOCAL FUNCTIONS
bool _obj_comparator(const pair<chromosome,double>& _obj1, const pair<chromosome,double>& _obj2){
	return _obj1.second > _obj2.second;
}

///////////////////////////////////////////////////////////////////////////////
///// GA
///////////////////////////////////////////////////////////////////////////////
ga::ga(string id, int generations, int non_changing_generations, int population_size, double linear_bias, double p_mutation, int num_seeds){
	_id = id;
	_generations = generations;
	_non_changing_gens = non_changing_generations;
	_pop_size = population_size;
	_linear_bias = linear_bias;
	_p_mutation = p_mutation;
	_num_seeds = min(num_seeds,population_size);

	_iterations_taken = 0;
	_initialized = false;
	_customers=NULL;
}

ga::~ga(){

}

int ga::linearBias(){
	gsl_rng * r;
	r = get_global_rand();

	return _pop_size * ((_linear_bias - sqrt(_linear_bias * _linear_bias - (4.0 * (_linear_bias - 1.0)) * gsl_rng_uniform(r))) / 2.0 / (_linear_bias - 1.0));
}

void ga::initialize_population(vector<smart_appliance *> * appliances, time_vector * rtp, time_vector * spot, vector<customer *> * cust){
	_customers = cust;
	_population.clear();

	for(int i = 0; i < _num_seeds; i++){
		//init seeds
		chromosome tempc(appliances, cust, rtp, spot, i, _num_seeds);
		double tempv;
		tempv = tempc.getP();

		_population.push_back(pair<chromosome,double>(tempc,tempv));
		//cout << i << ", $" << tempv << endl;
	}

	for(int i = _num_seeds; i < _pop_size; i++){
		//random init rest of pop
		chromosome tempc(appliances, cust, rtp, spot);
		double tempv;
		tempv = tempc.getP();

		_population.push_back(pair<chromosome,double>(tempc,tempv));
		//cout << i << ", $" << tempv << endl;
	}
	_popSort();

	_initialized = true;
}

pair<chromosome, double> * ga::run(){
	//ga::run()
	if(!_initialized){
		cout << "ERROR: Initialize GA population before running." << endl;
		return NULL;
	}

	//initialized, run logic here
	int gen = 0;
	int gen_no_improve = 0;
	double _eliteObjective;

	_eliteObjective = _population.at(ELITE_SOLUTION).second;

	_evolution.clear();
	_evolution.push_back(pair<int,double>(0,_eliteObjective));

	int tempGlobalTimer = start_new_global_timer("GA iterations");
	while((gen < this->_generations) && (gen_no_improve < this->_non_changing_gens)){
		/*
		 * SELECTION
		 */
		int _s1, _s2;
		_s1 = linearBias();

		do{ //ensure that we get two distinct chromosomes to crossover
			_s2 = linearBias();
		}while(_s1 == _s2);

		/*
		 * CROSSOVER
		 */
		//create copies of new elements, crossover occurs in-place
		chromosome _c1(_population.at(_s1).first);
		chromosome _c2(_population.at(_s1).first);

		chromosome::crossover(&_c1, &_c2);

		/*
		 * MUTATION
		 */
		_c1.mutate(this->_p_mutation);
		_c2.mutate(this->_p_mutation);

		/*
		 * EVALUATE
		 */
		double _v1, _v2;
		_v1 = _c1.getP();
		_v2 = _c2.getP();

		/*
		 * INSERT AND SORT
		 */
		/*_population.push_back(pair<chromosome,double>(_c1,_v1));
		_population.push_back(pair<chromosome,double>(_c2,_v2));
		_popSort();*/
		int inserted = 0;
		bool _firstInserted = false;
		if(_v1 > _v2){
			for(int p = 0; p < _population.size(); p++){
				if(!_firstInserted){
					if(_v1 > _population.at(p).second){
						_population.insert(_population.begin() + p, pair<chromosome,double>(_c1,_v1));
						inserted++;
						_firstInserted = true;
					}
				}else if(_v2 > _population.at(p).second){
					_population.insert(_population.begin() + p, pair<chromosome,double>(_c2,_v2));
					inserted++;
					break;
				}
			}
		}else{ //_v2 >= _v1
			for(int p = 0; p < _population.size(); p++){
				if(!_firstInserted){
					if(_v2 > _population.at(p).second){
						_population.insert(_population.begin() + p, pair<chromosome,double>(_c2,_v2));
						inserted++;
						_firstInserted = true;
					}
				}else if(_v1 > _population.at(p).second){
					_population.insert(_population.begin() + p, pair<chromosome,double>(_c1,_v1));
					inserted++;
					break;
				}
			}
		}

		/*
		 * TRIM POPULATION
		 */
		/*_population.pop_back();
		_population.pop_back();*/
		for(int _ins = 0; _ins < inserted; _ins++)
			_population.pop_back();

		/*
		 * INCREMENT INDECES
		 */
		gen++;
		if(_population.at(ELITE_SOLUTION).second == _eliteObjective){
			gen_no_improve++;
		}else{
			_eliteObjective = _population.at(ELITE_SOLUTION).second;
			gen_no_improve = 0;
			cout << "Generation " << gen << ":";
			cout << "Best Fitness = $" << _population.at(ELITE_SOLUTION).second <<endl;
			_evolution.push_back(pair<int,double>(gen,_eliteObjective));
		}

		if(((gen+1)%2500) == 0){
			cout << "Generation " << gen+1 << ", Runtime " << glance_global_timer(tempGlobalTimer) << ", Time-per-iter " << glance_global_timer(tempGlobalTimer)/(double)gen << endl;

		}
	} //end generation loop
	stop_global_timer(tempGlobalTimer);
	_evolution.push_back(pair<int,double>(gen,_eliteObjective));

	return new pair<chromosome,double>(_population.at(ELITE_SOLUTION).first, _population.at(ELITE_SOLUTION).second);
}

void ga::saveEvolution(string filename){
	ofstream fout(filename.c_str());

	for(vector<pair<int,double> >::iterator it = _evolution.begin(); it != _evolution.end(); it++){
		fout << it->first << "," << it->second << endl;
	}

	fout.close();
}

void ga::_popSort(){
	std::sort(_population.begin(),_population.end(),_obj_comparator);
}

void ga::_printPop(){
	for(vector<pair<chromosome,double> >::iterator it = _population.begin(); it != _population.end(); it++){
		cout << "$" << it->second << endl;
	}
}

/////////////////////////////////////////////////////////////////////////
//// CHROMOSOME
/////////////////////////////////////////////////////////////////////////
chromosome::chromosome(vector<smart_appliance *> * appliances, vector<customer *> * customers, time_vector * rtp, time_vector * spot){
	_initOverlap(appliances, customers, rtp, spot);

	for(int i = 0; i < _cip->getSize(); i++)
		//_cip->setValue(i,_getRatio(gsl_rng_uniform(r),rtp->getValue(i),spot->getValue(i))); //OLD, BETWEEN SPOT AND RTP
		_cip->setValue(i,_getRatio(gsl_rng_uniform(r),0.0,max(rtp->getValue(i),spot->getValue(i)))); //NEW, BETWEEN 0 and max(rtp,spot)
}

chromosome::chromosome(vector<smart_appliance *> * appliances, vector<customer *> * customers, time_vector * rtp, time_vector * spot, int seedNum, int totalSeeds){
	_initOverlap(appliances, customers, rtp, spot);

	double ratio = ((double)seedNum)/((double)totalSeeds-1.0);

	for(int i = 0; i < _cip->getSize(); i++)
		//_cip->setValue(i,_getRatio(ratio,rtp->getValue(i),spot->getValue(i))); //OLD, BETWEEN SPOT AND RTP
		_cip->setValue(i,_getRatio(ratio,0.0,max(rtp->getValue(i),spot->getValue(i)))); //NEW, BETWEEN 0 and max(rtp,spot)
}

chromosome::~chromosome(){
	//DO NOT DELETE POINTERS, THEY ARE MANAGED ELSEWHERE
	delete _cip; //only local dynamic memory
}

chromosome::chromosome(const chromosome& source){
	//deep copy of vector<smart_appliance_gene> _apps;
	_apps = source._apps;
	_cip = new time_vector(*(source._cip));
	_rtp = source._rtp;
	_spot = source._spot;
	_customers = source._customers;

	r = source.r;
}

chromosome& chromosome::operator= (const chromosome& source){
	if(this == &source) //check self assignment
		return *this;

	delete _cip;

	_apps = source._apps;
	_cip = new time_vector(*(source._cip));
	_rtp = source._rtp;
	_spot = source._spot;
	_customers = source._customers;

	r = source.r;

	return *this;
}

time_vector * chromosome::getCIP(){
	return new time_vector(*_cip);
}

//sets i2 >= i1
void __order(int * i1, int * i2){
	int _temp;

	if(*i2 < *i1){
		_temp = *i2;
		*i2 = *i1;
		*i1 = _temp;
	}
}

void chromosome::crossover(chromosome * c1, chromosome * c2){
	int _cross1, _cross2;

	//CIP crossover
	_cross1 = (int)(gsl_rng_uniform(c1->r) * (double)c1->_cip->getSize());
	_cross2 = (int)(gsl_rng_uniform(c1->r) * (double)c1->_cip->getSize());
	__order(&_cross1, &_cross2);
	time_vector::swap(c1->_cip, c2->_cip, _cross1,_cross2);

	//smart apps crossover
	_cross1 = (int)(gsl_rng_uniform(c1->r) * (double)c1->_apps.size());
	_cross2 = (int)(gsl_rng_uniform(c1->r) * (double)c1->_apps.size());
	__order(&_cross1, &_cross2);
	for(int i = _cross1; i < _cross2; i++)
		iter_swap(c1->_apps.begin()+i, c2->_apps.begin()+i);
}

void chromosome::mutate(double prob){
	//mutate CIP
	for(int i = 0; i < _cip->getSize(); i++){
		if(gsl_rng_uniform(r) < prob)
			_cip->setValue(i, _getRatio(gsl_rng_uniform(r),0.0,max(_rtp->getValue(i),_spot->getValue(i))));
	}

	//mutate smart apps
	for(int i = 0; i < _apps.size(); i++){
		if(gsl_rng_uniform(r) < prob)
			_apps.at(i).mutate();
	}
}

double chromosome::_getRatio(double ratio, double y1, double y2){
	double _max = max(y1, y2);
	double _min = min(y1, y2);

	return _min + (ratio)*(_max-_min);
}

void chromosome::_initOverlap(vector<smart_appliance *> * appliances, vector<customer *> * customers, time_vector * rtp, time_vector * spot){
	_rtp = rtp;
	_spot = spot;
	r = get_global_rand();
	_cip = new time_vector(_rtp->getSize());
	_customers = customers;

	for(vector<smart_appliance *>::iterator it = appliances->begin(); it != appliances->end(); it++){
		_apps.push_back(smart_appliance_gene(*it));
	}
}

void chromosome::applyChromosomeToAssets(){
	for(vector<smart_appliance_gene>::iterator it = _apps.begin(); it != _apps.end(); it++){
		smart_appliance * t_app;
		t_app = it->getApp();

		//gamma = 1
		//TODO: add _rtp to CUSTOMER
		if(_customers->at(t_app->getCustomer())->allowResch(t_app,_cip, _rtp, it->getTime()))
			it->applyGene();
		else //else gamma=0
			it->resetGene();
	}
}

/*void chromosome::setApplianceVectorFromChromosome(){
	//setApplianceVectorFromChromosome()
}*/

/*
 * OBJECTIVE FUNCTION ITEMS
 *
 * P = Total profit (N + S - B)
 * N - negative load,      to spot market, from original schedule
 * S - sell electricity,   to customer (CIP), from final schedule
 * B - buy electricity,    from spot market, from final schedule
 */
double chromosome::getP(time_vector * spot){ //////P LOGIC -------------------------------
	double P = 0.0;

	for(vector<smart_appliance_gene>::iterator it = _apps.begin(); it != _apps.end(); it++){
		smart_appliance * t_app;
		t_app = it->getApp();

		//gamma = 1
		//TODO: add _rtp to CUSTOMER
		if(_customers->at(t_app->getCustomer())->allowResch(t_app,_cip, _rtp, it->getTime())){
			it->applyGene();

			if(t_app->isRescheduled()){
				//P = N + S - B
				double N, S, B;
				N = t_app->getCostOrig(spot); //spot market, original time
				S = t_app->getCostResch(_cip);//CIP		  , final time
				B = t_app->getCostResch(spot);//spot market, final time
				P += (N + S - B);
			}
		}else{//else gamma=0
			it->resetGene();
		}
	}

	return P;
}

double chromosome::getP(){return getP(_spot);}

ostream& operator<<(ostream& out, chromosome& c){
	out << "CIP\n----------------\n" << *(c._cip) << endl << "Smart Apps\n---------------\n";

	for(vector<smart_appliance_gene>::iterator it = c._apps.begin(); it != c._apps.end(); it++)
		out << it-c._apps.begin() << "," << it->getTime() << endl;

	out << endl;

	return out;
}

/////////////////////////////////////////////////////////////////////////
////////// PARSER
/////////////////////////////////////////////////////////////////////////
ga_parser::ga_parser(){

}

ga * ga_parser::parse(rapidxml::xml_node<> * node){
	//create new GA and assign to retVal after all values determined
	string id;
	int generations;
	int non_changing_generations;
	int population_size;
	double linear_bias;
	double p_mutation;
	int num_seeds;

	id = string(node->first_attribute("id")->value());
	generations = getIntFromNode(node->first_node("generations"));
	non_changing_generations = getIntFromNode(node->first_node("same_gens"));
	population_size = getIntFromNode(node->first_node("population"));
	linear_bias = getDoubleFromNode(node->first_node("linear_bias"));
	p_mutation = getDoubleFromNode(node->first_node("prob_mut"));
	num_seeds = getIntFromNode(node->first_node("seeds"));

	/*cout << id << "\n------------" << endl;
	cout << "Generations:\t" << generations <<endl;
	cout << "Generations-N:\t" << non_changing_generations <<endl;
	cout << "Pop Size:\t" << population_size <<endl;
	cout << "Lin Bias:\t" << linear_bias <<endl;
	cout << "P Mutation:\t" << p_mutation <<endl;
	cout << "Num Seeds:\t" << num_seeds <<endl;*/

	return new ga(id, generations, non_changing_generations, population_size, linear_bias, p_mutation, num_seeds);
}

ga_parser::~ga_parser(){

}
