#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <stdint.h>

double random_uniform();
uint8_t random_uniform_range(uint8_t min, uint8_t max);
uint8_t random_bernoulli(double tau);

#endif
