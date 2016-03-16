/*
 * CoV.cpp
 *
 *  Created on: Apr 5, 2013
 *      Author: hansentm
 */

#include "CoV.h"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include "global/global_rand.h"
#include <iostream>
#include <cmath>
#include <fstream>

/*
 * nApps - number of applications
 * nProcTypes - number of processor types
 * M_app - mean alpha for assets
 * V_app - CoV parameter for asset alpha heterogeneity
 * V_cust - CoV parameter for customer heterogeneity
 */
CoV::CoV(int nApps, int nCusts){
	this->_numAssets = nApps;
	this->_numCusts = nCusts;
	this->_isG = false;

	for(uint app = 0; app < nApps; app++){
		vector<double> _row(nCusts,0.0);
		_alpha_etc_matrix.push_back(_row);
	}
}

CoV::~CoV() {

}

void CoV::generateAlphaETC(double M_app, double V_app, double V_cust){
	//turn stdev into variance
	double V_a, V_c;
	V_a = pow(V_app,2.0);
	V_c = pow(V_cust,2.0);

	//generate values

	//iterate through assets
	for(int _a = 0; _a < _numAssets; _a++){
		//generate mean alpha of that asset
		double _assetMean = CoV_gamma(V_a,M_app);

		//iterate through customers for this asset
		for(int _c = 0; _c < _numCusts; _c++){
			//generate alpha value using customer variance and asset mean
			double _temp_alph = CoV_gamma(V_c, _assetMean);

			//bound alpha to [0,1]
			_temp_alph = max(0.0,_temp_alph);
			_temp_alph = min(1.0,_temp_alph);

			_alpha_etc_matrix.at(_a).at(_c) = _temp_alph;
		}
	}

	_isG = true;
}

bool CoV::isGenerated(){
	return _isG;
}

double CoV::getAlphaIJ(int asset, int customer){
	double ret = 0.0;

	//check it fits in matrix
	if((asset < this->_alpha_etc_matrix.size()) && (customer < this->_alpha_etc_matrix.at(0).size())){
		ret = this->_alpha_etc_matrix.at(asset).at(customer);
	}

	return ret;
}

double CoV::CoV_gamma(double L, double mean){
	return gsl_ran_gamma(get_global_rand(), 1.0/L, mean*L);
}

ostream& operator<<(ostream& out, CoV& c){
	out << endl;

	for(int i = 0; i < c._numAssets; i++){
		out << "[";
		for(int j = 0; j < c._numCusts; j++){
			out << c._alpha_etc_matrix.at(i).at(j) << ", ";
		}
		out << "]" << endl;
	}

	return out;
}

void CoV::save(string filename){
	ofstream fout(filename.c_str());

	for(int i = 0; i < _numAssets; i++){
		for(int j = 0; j < _numCusts; j++){
			fout << _alpha_etc_matrix.at(i).at(j) << ",";
		}
		fout << endl;
	}

	fout.close();
}
