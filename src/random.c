#include <stdio.h> // TODO: remove
#include <stdlib.h>
#ifdef URANDOM
  #include "urandom.h"
#endif
#include "binary.h"

// generates a double between 0 and 1
double random_uniform() {
  double ret;
  //#ifdef URANDOM
  // TODO: how to generate random doubles between 0 and 1?
  //randombytes(&ret, sizeof(double));
  //#else
  ret = ((double)rand()/(double)RAND_MAX);
  //#endif
  return ret;
}

// http://stackoverflow.com/a/288869/1975046
uint8_t random_uniform_range(uint8_t min, uint8_t max) {
  double ret;
  //#ifdef URANDOM
  // TODO: how to generate random doubles between 0 and 1?
  //randombytes(&ret, sizeof(ret));
  //#else
  ret = ((double) rand() / (((double)RAND_MAX) + 1.0)) * (max-min+1) + min;
  //#endif
  return (uint8_t)ret;
}

// returns a random bit
uint8_t random_bernoulli(double tau) {
  return (uint8_t)(random_uniform() < tau);
}

// Generate a uniform random uint32
uint32_t random_uniform_uint32(void) {
  uint32_t n = 0;
  #ifdef URANDOM
  randombytes(&n, sizeof(n));
  #else
  uint8_t i = 0;
  for(i = 0; i < 32; i++) {
    n <<= 1;
    n ^= (uint32_t)random_uniform_range(0, 1);
  }
  #endif
  return n;
}

// generate a random uint32, according to bernoulli distribution
uint32_t random_bernoulli_uint32(double tau) {
  uint8_t i = 0;
  uint32_t n = 0;
  for(; i < 32; i++) {
    n <<= 1;
    n ^= (uint32_t)random_bernoulli(tau);
  }
  return n;
}


