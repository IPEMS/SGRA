/*
 * input.cpp
 *
 *  Created on: Apr 11, 2012
 *      Author: hansentm
 */


#include "xml_reader/rapidxml.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include "input.h"
#include "global/global_rand.h"
#include "timevector.h"
#include "heuristics/ga.h"
#include "assets/smartappliance.h"
#include "customer.h"
#include "uniformdecision.h"

using namespace std;
using namespace rapidxml;

double parse_number(xml_node<> * node);
double parse_uniform(xml_node<> * node);
double parse_gaussian(xml_node<> * node);
vector<char> *  local_parseXml(ifstream * f, xml_document<> * ret);

///////////////////////////
// TeMP
///////////////////////////
aggregator * agg_parse(rapidxml::xml_node<> * node, int ss_int, int time_int);

aggregator * parseXmlFile(ifstream * f){
	xml_document<> doc; //contains parsed xml file

	vector<char> * xml_vec = local_parseXml(f, &doc);

	//PARSE XML FILE
	xml_node<> * currentNode = doc.first_node("environment");
	int ss_intervals = getIntFromNode(currentNode->first_node("ss_intervals"));
	int time_intervals = getIntFromNode(currentNode->first_node("time_intervals"));

	//GET AGGREGATOR
	aggregator * agg = agg_parse(currentNode->first_node("aggregators")->first_node("agg"), ss_intervals, time_intervals);

	//GET HEURISTICS
	ga_parser _ga_p;
	agg->setGA(_ga_p.parse(doc.first_node("heuristics")->first_node("ga")));

	delete xml_vec;

	return agg;
}

vector<char> * local_parseXml(ifstream * f, xml_document<> * ret){
	vector<char> * xfile;
	xfile = new vector<char>((istreambuf_iterator<char>(*f)),istreambuf_iterator<char>());
	xfile->push_back('\0');
	ret->parse<0>(&(*xfile)[0]);
	return xfile;
}

double parse_number(xml_node<> * node){
	return atof(node->first_attribute("number")->value());
}

double parse_uniform(xml_node<> * node){
	double mini = atof(node->first_attribute("min")->value());
	double maxi = atof(node->first_attribute("max")->value());
	double range = maxi-mini;
	gsl_rng * r;
	r = get_global_rand();

	return (gsl_rng_uniform(r)*range)+mini;
}

double parse_gaussian(xml_node<> * node){
	double std = atof(node->first_attribute("std")->value());
	double mean = atof(node->first_attribute("mean")->value());

	gsl_rng * r;
	r = get_global_rand();

	return gsl_ran_gaussian(r,std) + mean;
}

double getDoubleFromNode(rapidxml::xml_node<> * node){
	double retVal = 0.0;

	switch(node->first_attribute("id")->value()[0]){
	case '0':
		retVal = parse_number(node);
		break;
	case 'n':
		retVal = parse_gaussian(node);
		break;
	case 'u':
		retVal = parse_uniform(node);
		break;
	}

	return retVal;
}
int getIntFromNode(rapidxml::xml_node<> * node){
	int retVal = 0;

	switch(node->first_attribute("id")->value()[0]){
	case '0':
		retVal = (int)parse_number(node);
		break;
	case 'n':
		retVal = (int)parse_gaussian(node);
		break;
	case 'u':
		retVal = (int)parse_uniform(node);
		break;
	}

	return retVal;
}

string getStringFromNode(rapidxml::xml_node<> * node){
	return string(node->first_attribute("name")->value());
}

///////////////////////////////////////////////////////////////////////////////
///// TEMPORARY
///////////////////////////////////////////////////////////////////////////////
#include "psychology/IPsychology.h"
#include "psychology/PsychRatio.h"
#include "psychology/PsychAlpha.h"
#include <map>
#include "CoV.h"

//HELPER FUNCTIONS FOR EASE OF CODE
void _getBaseLoad(time_vector * base, xml_node<> * start_node, uint cust, int time_int, gsl_rng * r){
		while(start_node != NULL){
			double penetration = getDoubleFromNode(start_node->first_node("penetration"));

			if(gsl_rng_uniform(r) < penetration){
				double power;
				int start;
				int duration;

				smart_appliance * _temp_app;

				switch(getIntFromNode(start_node)){
				case 0:
					power = getDoubleFromNode(start_node->first_node("power"));
					duration = getIntFromNode(start_node->first_node("duration"));
					start = (int)(getDoubleFromNode(start_node->first_node("start"))*4.0); //TODO  INDECES_PER_HOUR
					break;

				case 1:
					power = getDoubleFromNode(start_node->first_node("power"));
					start = 0;
					duration = time_int;
					break;
				}

				_temp_app = new smart_appliance(-1, cust, power, duration, start, -1, -1, -1);
				_temp_app->addPowerOrig(base);
				delete _temp_app;
			}
			start_node = start_node->next_sibling("app");
		}

		//TODO BASE LOAD - HVAC
}

int _getCustAppliances(vector<smart_appliance *> * appliances, vector<smart_appliance *> * _cust_appliances, xml_node<> * assetNode, int app_index, int cust, int time_int, int ss_int, gsl_rng * r){
	int _type = 0;
	while(assetNode != NULL){
		double penetration = getDoubleFromNode(assetNode->first_node("penetration"));

		if(gsl_rng_uniform(r) < penetration){
			double power = getDoubleFromNode(assetNode->first_node("power"));
			int start = (int)(getDoubleFromNode(assetNode->first_node("start")) * 4.0); ////TODO INDECES_PER_HOUR
			int duration = getIntFromNode(assetNode->first_node("duration"));
			int avail_start, avail_duration;

			avail_duration = (int)(gsl_rng_uniform(r) * (double)time_int);
			avail_start = start - (avail_duration/2);
			if((avail_start + avail_duration) >= (time_int + ss_int)) //set duration to fit in constraints
				avail_duration = (time_int + ss_int) - avail_start;
			if(avail_start < -ss_int) //set start to fit in constraints
				avail_start = -ss_int;

			smart_appliance * temp_appliance = new smart_appliance(app_index++, cust, power, duration, start, avail_start, avail_duration,_type);
			appliances->push_back(temp_appliance);
			_cust_appliances->push_back(temp_appliance);
		}
		assetNode = assetNode->next_sibling("app");
		_type++;
	}
	return app_index;
}

IPsychology * _getPsychology(int psychType, int cust, vector<smart_appliance *> * _cust_appliances,  xml_node<> * currentNode, int time_int, time_vector * rtp_fore, time_vector * spot_fore, gsl_rng * r, CoV * cov){
	IPsychology * _ptr;
	switch(psychType){
		case 0:{
			time_vector * limit = new time_vector(time_int);
			double _temp_psych = gsl_rng_uniform(r);
			for(int i = 0; i < time_int; i++){
				double _rtp = rtp_fore->getValue(i);
				double _spot = spot_fore->getValue(i);
				if(_rtp < _spot)
					limit->setValue(i,_rtp);
				else
					limit->setValue(i,_spot+ _temp_psych * (_rtp-_spot));
			}
			_ptr = new PsychRatio(limit);
			break;
		}case 1:{
			map<int, double> * alphas = new map<int, double>();
			for(int a = 0; a < _cust_appliances->size(); a++){
				double alpha;

				alpha = getDoubleFromNode(currentNode->first_node("psych")->first_node("alpha"));
				alpha = min(1.0,alpha); alpha = max(0.0,alpha);  //make sure 0 < alpha < 1

				(*alphas)[_cust_appliances->at(a)->getIndex()] = alpha;
			}
			_ptr = new PsychAlpha(alphas);

			break;
		}case 2:{
			//if CoV not filled, generate data.
			if(!cov->isGenerated()){
				cov->generateAlphaETC(getDoubleFromNode(currentNode->first_node("psych")->first_node("M_app")),
									  getDoubleFromNode(currentNode->first_node("psych")->first_node("V_app")),
									  getDoubleFromNode(currentNode->first_node("psych")->first_node("V_cust")));
			}

			//sample from CoV
			map<int, double> * alphas = new map<int, double>();
			for(int a = 0; a < _cust_appliances->size(); a++){
				(*alphas)[_cust_appliances->at(a)->getIndex()] = cov->getAlphaIJ(_cust_appliances->at(a)->getType(),cust);
			}

			_ptr = new PsychAlpha(alphas);

			break;
		}
	}
	return _ptr;
}

smart_appliance * _getApplianceFromLoadInfoLine(string * _str, int app_index, bool gen_avail){
	//NEED ELEMENTS: (0) customer, (2) start time, (4) availability start time, (5) availability duration, (6) power, (7) duration
	int _cust, _start, _av_start, _av_dur, _dur;
	double _power;

	//parse load_info line
	stringstream ss(*_str);
	string item;
	vector<string> elems;
	while (getline(ss, item, ',')) {
		elems.push_back(item);
	}

	_cust = atoi(elems.at(0).c_str());
	_start = int(atof(elems.at(2).c_str()) * 4);
	if(gen_avail){
		_av_start = int(atof(elems.at(4).c_str()) * 4);
		_av_dur = int(atof(elems.at(5).c_str()) * 4);
	}else{
		//can schedule anywhere
		_av_start = -8;
		_av_dur = 112;
	}
	_power = atof(elems.at(6).c_str());
	_dur = int(atof(elems.at(7).c_str()) * 4);

	//cout << "App " << app_index << ": cust=" << _cust << ", _start=" << _start << ", _av_tart=" << _av_start << ", _av_durt=" << _av_dur << ", _dur=" << _dur << ", _power=" << _power << endl;
	//TODO get asset type for _getApplianceFromLoadInfoLine(
	return new smart_appliance(app_index, _cust, _power, _dur, _start, _av_start, _av_dur,-1);
}

customer * _getCustomerFromFile(string _loadFolder, int prevCustomer, int time_int, xml_node<> * currentNode, vector<smart_appliance *> * _cust_appliances, time_vector * rtp_fore, time_vector * spot_fore, gsl_rng * r, CoV * cov, time_vector * rtp_garch, double garch_prob){
	IPsychology * _psych;
	time_vector * _temp_baseload;
	stringstream _temp_fname;

	//get baseload
	_temp_fname << _loadFolder << "/cust/" << prevCustomer << "/";
	_temp_baseload = new time_vector((_temp_fname.str() + "base.txt").c_str(), time_int);

	//get psych
	int _psych_type = getIntFromNode(currentNode->first_node("psych"));

	switch(_psych_type){
	case -1: //-1 -> load alpha from file
		_psych = IPsychology::load(_temp_fname.str() + "psych.txt");
		break;
	default: //else, generate alpha
		_psych = _getPsychology(_psych_type, prevCustomer, _cust_appliances, currentNode, time_int, rtp_fore, spot_fore,r,cov);
	}

	//TODO load customer loadpoint
	int _loadPoint = 0;

	if(gsl_rng_uniform(r) < garch_prob){
		return new customer(prevCustomer, _cust_appliances, _temp_baseload, _psych, _loadPoint, rtp_garch);
	}else{
		return new customer(prevCustomer, _cust_appliances, _temp_baseload, _psych, _loadPoint, NULL);
	}
}

//PARSE XML FILE
aggregator * agg_parse(rapidxml::xml_node<> * node, int ss_int, int time_int){
	xml_node<> * currentNode;

	//filestreams for xml files
	ifstream _base_if;
	ifstream _asset_if;

	//persistant string for xml reader
	vector<char> * temp_vecs[2];

	//rand
	gsl_rng * r;
	r = get_global_rand();

	//smart appliances
	vector<smart_appliance *> * appliances;
	appliances = new vector<smart_appliance *>();

	//customers
	vector<customer *> * customers = new vector<customer *>();

	/*
	 * PRICING
	 */
	currentNode = node->first_node("pricing");

	//PRICING VECTORS
	time_vector * rtp_fore, * rtp_act, * spot_fore, * spot_act, * rtp_garch;
	double garch_prob;

	//default to no GARCH
	rapidxml::xml_node<> * _garch_node;
	_garch_node = currentNode->first_node("estimate");

	if(_garch_node == 0){
		rtp_garch = NULL;
		garch_prob = 0.0;
	}else{
		rtp_garch = new time_vector(getStringFromNode(_garch_node->first_node("file")),time_int);
		garch_prob = getDoubleFromNode(_garch_node->first_node("penetration"));
		cout << "USE GARCH: " << garch_prob << endl;
	}

	rtp_fore = new time_vector(getStringFromNode(currentNode->first_node("forecast")->first_node("dist")),time_int);
	rtp_act = new time_vector(getStringFromNode(currentNode->first_node("actual")->first_node("dist")),time_int);
	spot_fore = new time_vector(getStringFromNode(currentNode->first_node("forecast")->first_node("spot")),time_int);
	spot_act = new time_vector(getStringFromNode(currentNode->first_node("actual")->first_node("spot")),time_int);



	/*
	 * CUSTOMERS
	 */
	//type = 0 -> generate new customer and load data.  type = 1 -> load customers and loads from file.  DEFAULT TO GENERATION
	int CUST_XML_TYPE = 0;
	bool GENERATE_AVAIL;
	currentNode = node->first_node("customers");
	CUST_XML_TYPE = getIntFromNode(currentNode);
	GENERATE_AVAIL = bool(getIntFromNode(currentNode->first_node("avail")));

	//TEMPERATURE VECTOR
	time_vector * temperature;
	temperature = new time_vector(getStringFromNode(currentNode->first_node("base_hvac")),time_int);

	int _numLoadpoints = getIntFromNode(node->first_node("loadpoints"));

	xml_document<> _assets;
	_asset_if.open(getStringFromNode(currentNode->first_node("assets")).c_str(),ifstream::in);
	temp_vecs[1] = local_parseXml(&_asset_if, &_assets);


	if(CUST_XML_TYPE == 0){ //GENERATE NEW CUSTOMER DATA
		//load XML documents
		xml_document<> _base;

		//get values for loadpoints
		vector<double> * _loadpointProbs = new vector<double>(_numLoadpoints);
		currentNode = node->first_node("loadpoints")->first_node("lp");

		for(int i = 0; i < _numLoadpoints; i++){
			double ___t = getDoubleFromNode(currentNode);
			_loadpointProbs->at(i) = ___t;
			currentNode = currentNode->next_sibling("lp");
		}


		uniform_decision _loadpointDist(_loadpointProbs);

		currentNode = node->first_node("customers");

		_base_if.open(getStringFromNode(currentNode->first_node("base_load")).c_str(),ifstream::in);
		temp_vecs[0] = local_parseXml(&_base_if, &_base);

		int num_customers = getIntFromNode(currentNode->first_node("total"));
		int app_index = 0;

		int num_poss_assets = getIntFromNode(_assets.first_node("assets"));
		//CoV with 0 matrix.  numassets x numcusts
		CoV _assetETC(num_poss_assets,num_customers);

		//generate customer loads
		//TODO GENERATE_AVAIL, generating customers
		for(uint cust = 0; cust < num_customers; cust++){
			xml_node<> * assetNode;

			//BASE LOAD - APPLIANCES
			time_vector * _temp_baseload = new time_vector(time_int);
			assetNode = _base.first_node("base")->first_node("appliances")->first_node("app");
			_getBaseLoad(_temp_baseload, assetNode, cust, time_int, r);


			//SCHEDULABLE LOADS
			//smart appliances - customer specific
			vector<smart_appliance *> * _cust_appliances;
			_cust_appliances = new vector<smart_appliance *>();
			assetNode = _assets.first_node("assets")->first_node("appliances")->first_node("app");
			app_index = _getCustAppliances(appliances, _cust_appliances, assetNode, app_index, cust, time_int, ss_int, r);

			//CUSTOMER PSYCHOLOGY
			IPsychology * _psych = NULL;
			_psych = _getPsychology(getIntFromNode(currentNode->first_node("psych")), cust, _cust_appliances, currentNode, time_int, rtp_fore, spot_fore,r,&_assetETC);

			//add customer with all pieces to list
			if(gsl_rng_uniform(r) < garch_prob){
				customers->push_back(new customer(cust, _cust_appliances, _temp_baseload, _psych, _loadpointDist.getDecision(),rtp_garch));
			}else{
				customers->push_back(new customer(cust, _cust_appliances, _temp_baseload, _psych, _loadpointDist.getDecision(),NULL));
			}
		} //end generate customer loads



		//clean memory
		_base_if.close();
		delete temp_vecs[0];
	}else if(CUST_XML_TYPE == 1){ //LOAD CUSTOMER DATA FROM FILE
		string _loadFolder;

		//out_load_info variables
		ifstream __load_data;
		string __load_line;

		_loadFolder = getStringFromNode(currentNode->first_node("folder"));

		__load_data.open((_loadFolder + "out_load_info.txt").c_str());
		getline(__load_data, __load_line); // first line is column headings


		//need these for each customer
		vector<smart_appliance *> * _cust_appliances;
		int prevCustomer = -1; //previous customer to know if we have a new customer
		int app_index = 0;
		//CoV with 0 matrix.  numassets x numcusts
		CoV _assetETC(getIntFromNode(_assets.first_node("assets")),getIntFromNode(node->first_node("customers")->first_node("total")));

		while(getline(__load_data, __load_line)){
			smart_appliance * _temp_app;
			//parse __load_line
			_temp_app = _getApplianceFromLoadInfoLine(&__load_line, app_index++, GENERATE_AVAIL);

			if(_temp_app->getCustomer() != prevCustomer){
				if(prevCustomer != -1){ //make sure it isn't the first iteration
					//ADD NEW CUSTOMER TO LIST
					customers->push_back(_getCustomerFromFile(_loadFolder, prevCustomer, time_int, currentNode, _cust_appliances, rtp_fore, spot_fore, r,&_assetETC, rtp_garch, garch_prob));
				}

				_cust_appliances = new vector<smart_appliance *>();
				prevCustomer = _temp_app->getCustomer();
			}

			_cust_appliances->push_back(_temp_app);
			appliances->push_back(_temp_app);
		}

		//add last customer
		customers->push_back(_getCustomerFromFile(_loadFolder, prevCustomer, time_int, currentNode, _cust_appliances, rtp_fore, spot_fore, r,&_assetETC, rtp_garch, garch_prob));
	} //end if CUST_XML_TYPE == 1
	_asset_if.close();
	delete temp_vecs[1];
	delete temperature;

	if(rtp_garch != NULL)
		delete rtp_garch;

#ifdef INCLUDE_POWERWORLD
	return new aggregator(customers, appliances, spot_fore, spot_act, rtp_fore, rtp_act, new octave_interface(getStringFromNode(node->first_node("octave")->first_node("in")),getStringFromNode(node->first_node("octave")->first_node("out"))), _numLoadpoints);
#else
	return new aggregator(customers, appliances, spot_fore, spot_act, rtp_fore, rtp_act, _numLoadpoints);
#endif
}
