/*
 * ga.h
 *
 *  Created on: Mar 28, 2012
 *      Author: hansentm
 */

#ifndef GA_H_
#define GA_H_

//#include "heuristic.h"
#include <vector>
#include <string>
#include <utility>

#include "../assets/smartappliance.h"
#include "../timevector.h"
#include "../customer.h"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "../xml_reader/rapidxml.hpp"

using namespace std;

class chromosome;

////////////////////////////////////////////////////////////////////////////////////////////////
/////// GA
////////////////////////////////////////////////////////////////////////////////////////////////

class ga {//: public heuristic {
public:
	ga(string id, int generations, int non_changing_generations, int population_size, double linear_bias, double p_mutation, int num_seeds);
	virtual ~ga();

	void initialize_population(vector<smart_appliance *> * appliances, time_vector * rtp, time_vector * spot, vector<customer *> * cust);

	pair<chromosome, double> * run();

	void saveEvolution(string filename);

private:
	int linearBias();
	void _popSort();
	void _printPop();

protected:
	string _id;
	int _generations;
	int _non_changing_gens;
	double _linear_bias;
	double _p_mutation;
	int _pop_size;
	int _num_seeds;

	int _iterations_taken;
	bool _initialized;

	vector<pair<chromosome,double> > _population;
	vector<customer *> * _customers; //DO NOT DELETE
	vector<pair<int, double> > _evolution;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
////// CHROMOSOME
///////////////////////////////////////////////////////////////////////////////////////////////////

class chromosome{
public:
	chromosome(vector<smart_appliance *> * appliances, vector<customer *> * customers, time_vector * rtp, time_vector * spot);
	chromosome(vector<smart_appliance *> * appliances, vector<customer *> * customers, time_vector * rtp, time_vector * spot, int seedNum, int totalSeeds);
	virtual ~chromosome();
	chromosome(const chromosome& source);
	chromosome& operator= (const chromosome& source);

	//NOTE: crossover occurs in place
	static void crossover(chromosome * c1, chromosome * c2);
	void mutate(double prob);

	void applyChromosomeToAssets();

	//chromosome accessors
	time_vector * getCIP(); //caller is responsible for this memory

	//objective function things, use given spot market price
	double getP();
	//double getN();
	//double getS();
	//double getB();
	//get values with spot market price
	double getP(time_vector * spot);
	//double getN(time_vector * spot);
	//S does not depend on the spot market
	//double getB(time_vector * spot);


	//sets the appliance vector according to the genes and customers
	//void setApplianceVectorFromChromosome();

	friend ostream& operator<<(ostream& out, chromosome& c);

protected:
	//DO NOT DELETE POINTERS, THEY ARE MANAGED ELSEWHERE (except _cip)
	vector<smart_appliance_gene> _apps;
	time_vector * _cip, * _rtp, * _spot;
	gsl_rng * r;
	vector<customer *> * _customers; //DO NOT DELETE

	//ONLY SET APPLIANCE VECTOR ONCE
	double getN(time_vector * spot, bool _setAppVector);
	double getS(bool _setAppVector);
	double getB(time_vector * spot, bool _setAppVector);

private:
	double _getRatio(double ratio, double y1, double y2);
	void _initOverlap(vector<smart_appliance *> * appliances, vector<customer *> * customers, time_vector * rtp, time_vector * spot);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////
///////// GA PARSER
////////////////////////////////////////////////////////////////////////////////////////////////////////

class ga_parser{// : public heuristic_parser{
public:
	ga_parser();
	virtual ~ga_parser();
	virtual ga * parse(rapidxml::xml_node<> * node);
};

#endif /* GA_H_ */
