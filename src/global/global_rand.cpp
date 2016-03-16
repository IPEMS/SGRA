/*
 * global_rand.cpp
 *
 *  Created on: Feb 14, 2012
 *      Author: hansentm
 */

#include "global_rand.h"

gsl_rng * r;

void init_global_rand(long seed){
	gsl_rng_env_setup();

	r=gsl_rng_alloc(gsl_rng_rand48);
	gsl_rng_set(r,seed);
}

gsl_rng * get_global_rand(){
	return r;
}

void del_global_rand(){
	gsl_rng_free(r);
}
