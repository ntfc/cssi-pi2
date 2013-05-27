#include <stdlib.h>
#include "random.h"

// TODO
//#ifdef URANDOM

//#elifdef SRAND

// generates a double between 0 and 1
double random_uniform() {
  double ret = ((double)rand()/(double)RAND_MAX);
  return ret;
}

// http://stackoverflow.com/a/288869/1975046
uint8_t random_uniform_range(uint8_t min, uint8_t max) {
  double ret = ((double) rand() / (((double)RAND_MAX) + 1.0)) * (max-min+1) + min;
  return (uint8_t)ret;
}

// returns a random bit
uint8_t random_bernoulli(double tau) {
  return (uint8_t)(random_uniform() < tau);
}

//#endif
