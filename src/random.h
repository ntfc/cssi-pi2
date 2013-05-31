#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <stdint.h>

typedef uint32_t *Challenge;

double random_uniform();
uint8_t random_uniform_range(uint8_t min, uint8_t max);
uint8_t random_bernoulli(double tau);
uint32_t random_uniform_uint32(void);
uint32_t random_bernoulli_uint32(double tau);
Challenge random_gen_c(uint8_t n);

#endif
