#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <stdint.h>

double uniform_rand();
uint8_t uniform_rand_range(uint8_t min, uint8_t max);
uint8_t bernoulli(double tau);

#endif
