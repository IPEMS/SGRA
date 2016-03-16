/*
 * global_rand.h
 *
 *  Created on: Feb 14, 2012
 *      Author: hansentm
 */

#ifndef GLOBAL_RAND_H_
#define GLOBAL_RAND_H_

#include <gsl/gsl_rng.h>

void init_global_rand(long seed);

gsl_rng * get_global_rand();

void del_global_rand();


#endif /* GLOBAL_RAND_H_ */
