/*
 * CoV.h
 *
 *  Created on: Apr 5, 2013
 *      Author: hansentm
 */

#ifndef COV_H_
#define COV_H_

#include <vector>
#include <cmath>
#include <ostream>

using namespace std;

class CoV {
public:
	/*
	 * nApps - number of applications
	 * nProcTypes - number of processor types
	 * M_app - mean alpha for assets
	 * V_app - CoV parameter for asset alpha heterogeneity
	 * V_cust - CoV parameter for customer heterogeneity
	 */
	CoV(int nApps, int nCusts);
	virtual ~CoV();

	friend ostream& operator<<(ostream& out, CoV& c);

	void generateAlphaETC(double M_app, double V_app, double V_cust);
	double getAlphaIJ(int asset, int customer);
	bool isGenerated();
	void save(string filename);

private:
	vector<vector<double> > _alpha_etc_matrix;
	int _numAssets, _numCusts;
	bool _isG;

	double CoV_gamma(double L, double mean);
};

#endif /* COV_H_ */
