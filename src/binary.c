#include "binary.h"

// to count the bits
// Used by: http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
static const int S[] = {1, 2, 4, 8, 16}; // Magic Binary Numbers
static const int B[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF, 0x0000FFFF};

// http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
uint8_t hamming_weight(uint32_t n) {
  uint32_t wt = 0;
  wt = n - ((n >> 1) & B[0]);
  wt = ((wt >> S[1]) & B[1]) + (wt & B[1]);
  wt = ((wt >> S[2]) + wt) & B[2];
  wt = ((wt >> S[3]) + wt) & B[3];
  wt = ((wt >> S[4]) + wt) & B[4];
  return (uint8_t)wt;
}

// p: 1st word in the polynomial // ATTENTION: by 1st, we mean the MSB
// t: number of 32bit words in the polynomial
uint32_t degree(uint32_t* p, uint32_t t) {
  uint8_t deg = 0;
  while(*p >>= 1)
    deg++;
  // TODO: define the word size. And this muliplication can be more efficient
  return deg + (W * (t-1));
}

// reverses all bits from a number: reverse_number(0b01001) = 0b10010
uint32_t reverse_number(uint32_t n) {
  uint32_t s = sizeof(n) * CHAR_BIT_SIZE; // bit size; must be power of 2 
  uint32_t mask = ~0;         
  while ((s >>= 1) > 0) {
    mask ^= (mask << s);
    n = ((n >> s) & mask) | ((n << s) & ~mask);
  }
  return n;
}

/*uint32_t uint_array_to_uint(uint32_t *t) {
  // TODO: receive uint32_t* or char*?
  uint32_t n = 0;
  while(*t++) {
    printf("->%d\n", *t);
    / *n <<= 1;
    if(*t++ == 1)
      n ^= 1;* /
  }
  return n;
}*/

// convert a binary string to uint32_t
// bits from position 32 forward are discarded
uint32_t char_to_uint(const unsigned char *c) {
  uint32_t out = 0;
  const unsigned char *s = c;
  while(*s && s-c < W) {
    out = (out << 1) | (*s == '0' ? 0 : 1);
    s++;
  }
  return out;
}
