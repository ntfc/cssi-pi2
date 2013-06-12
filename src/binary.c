#include <stdlib.h>
#include <stdio.h> //TODO: remove
#include "binary.h"

// to count the bits
// Used by: http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
static const int S[] = {1, 2, 4, 8, 16}; // Magic Binary Numbers
static const int B[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF, 0x0000FFFF};

// http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
uint8_t binary_hamming_weight(uint32_t n) {
  uint32_t wt = 0;
  wt = n - ((n >> 1) & B[0]);
  wt = ((wt >> S[1]) & B[1]) + (wt & B[1]);
  wt = ((wt >> S[2]) + wt) & B[2];
  wt = ((wt >> S[3]) + wt) & B[3];
  wt = ((wt >> S[4]) + wt) & B[4];
  return (uint8_t)wt;
}


// NOTE: does not return the polynomial degree!! Use poly_degree() for that!!
// p: 1st word in the polynomial // ATTENTION: by 1st, we mean the MSB
// http://graphics.stanford.edu/~seander/bithacks.html
//uint16_t binary_degree(const uint32_t* p, uint32_t t) {
uint16_t binary_degree(uint32_t p) {
  uint8_t deg = 0;
  while(p >>= 1)
    deg++;
  // TODO: define the word size. And this muliplication can be more efficient
  return deg; // + (W * (t-1));
}

// reverses all bits from a number: reverse_number(0b01001) = 0b10010
// http://graphics.stanford.edu/~seander/bithacks.html
uint32_t binary_reverse_number(uint32_t n) {
  uint32_t s = sizeof(n) * CHAR_BIT_SIZE; // bit size; must be power of 2 
  uint32_t mask = ~0;         
  while ((s >>= 1) > 0) {
    mask ^= (mask << s);
    n = ((n >> s) & mask) | ((n << s) & ~mask);
  }
  return n;
}

// convert a binary string to uint32_t
// bits from position 32 forward are discarded
uint32_t binary_char_to_uint(const unsigned char *c) {
  uint32_t out = 0;
  const unsigned char *s = c;
  while(*s && s-c < W) {
    out = (out << 1) | (*s == '0' ? 0 : 1);
    s++;
  }
  return out;
}

// convert a uint32_t to its binary representation
// saves the result in dst and returns it
// http://stackoverflow.com/a/16313354/1975046
unsigned char* binary_uint32_to_char(uint32_t w, unsigned char *dst) {
  uint32_t n = w;
  int8_t i = 0, c = 0, w_size = sizeof(w) * 8;
  
  
  for(i = w_size - 1; i >= 0; i--) {
    n = w >> i;
    *(dst + c) = (n & 0x1) ? '1' : '0';
    c++;
  }
  *(dst + c) = 0;
  
  return dst;
}

// convert a uint16_t to its binary representation
// saves the result in dst and returns it
// http://stackoverflow.com/a/16313354/1975046
unsigned char* binary_uint16_to_char(uint16_t w, unsigned char *dst) {
  uint16_t n = w;
  int8_t i = 0, c = 0, w_size = sizeof(w)*8;
  
  for(i = w_size - 1; i >= 0; i--) {
    n = w >> i;
    *(dst + c) = (n & 0x1) ? '1' : '0';
    c++;
  }
  *(dst + c) = 0;
  return dst;
}

// convert a uint8_t to its binary representation
// saves the result in dst and returns it
// http://stackoverflow.com/a/16313354/1975046
unsigned char* binary_uint8_to_char(uint8_t w, unsigned char *dst) {
  uint8_t n = w;
  int8_t i = 0, c = 0, w_size = sizeof(w)*8;
  
  for(i = w_size - 1; i >= 0; i--) {
    n = w >> i;
    *(dst + c) = (n & 0x1) ? '1' : '0';
    c++;
  }
  *(dst + c) = 0;
  return dst;
}

// get bit i from word w
// http://stackoverflow.com/a/4854257/1975046
uint8_t binary_get_bit(uint32_t w, uint8_t i) {
  // TODO: i must be greater or equal than 0
  uint8_t set = (w & (1 << i)) != 0;
  return set;
}

// t: array size
void binary_array_shift_left(uint32_t *a, uint16_t t) {
  int16_t i = 0;
  for(i = 0; i < (t - 1); i++)
    a[i] = (a[i] << 1) | (a[i+1] >> (W - 1));
  a[i] <<= 1;
}

// b = (a << 1)
void binary_array_shift_left2(const uint32_t *a, uint16_t t, uint32_t* b) {
  int16_t i = 0;
  if(b == NULL) {
    fprintf(stderr, "ERROR: b is NULL\n");
    return;
  }
  for(i = 0; i < t; i++) {
    b[i] = a[i];
  }
  binary_array_shift_left(b, t);
}

// t: array size
void binary_array_shift_right(uint32_t *a, uint16_t t) {
  int16_t i = 0;
  for(i = t-1; i > 0; i--)
    a[i] = (a[i] >> 1) | (a[i-1] << (W - 1));
  a[i] >>= 1;
}
