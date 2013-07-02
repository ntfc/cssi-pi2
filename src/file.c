#include <math.h>
#include "file.h"
#include "binary.h"
#include "poly.h"


int8_t file_save_challenge(FILE *fp, const Challenge c) {
  if(!fp) {
    fprintf(stderr, "ERROR: null file\n");
    return -1;
  }
  if(!c) {
    fprintf(stderr, "ERROR: null challenge\n");
    return -1;
  }
  
  // write the sec param
  int fpr = fprintf(fp, "%u %u\n", W, SEC_PARAM);
  
  uint8_t i = 0;
  uint8_t t = ceil((double)SEC_PARAM / (double)W);
  for(i = 1; i <= t; i++) {
    if(i == t) {
      fpr = fprintf(fp, "%.8x\n", c[i-1]);
    }
    else {
      if(i % 8) {
        fpr = fprintf(fp, "%.8x ", c[i-1]);
      }
      else {
        fpr = fprintf(fp, "%.8x\n", c[i-1]);
      }
    }
  }
  return fpr;
}

int8_t file_save_poly(FILE *fp, const Poly *p) {
  if(!fp) {
    fprintf(stderr, "ERROR: null file\n");
    return -1;
  }
  if(!p) {
    fprintf(stderr, "ERROR null poly\n");
    return -1;
  }
  
  int fpr = fprintf(fp, "%u %u %u\n", W, p->m, p->t);
  
  uint16_t i = 0;
  
  for(i = 1; i <= p->t; i++) {
    if(i == p->t) {
      fpr = fprintf(fp, "%.8x\n", p->vec[i-1]);
    }
    else {
      if(i % 8) {
        fpr = fprintf(fp, "%.8x ", p->vec[i-1]);
      }
      else {
        fpr = fprintf(fp, "%.8x\n", p->vec[i-1]);
      }
    }
  }
  
  return fpr;
}

int8_t file_save_crt(FILE *fp, const PolyCRT *c) {
  if(!fp) {
    fprintf(stderr, "ERROR: null file\n");
    return -1;
  }
  if(!c) {
    fprintf(stderr, "ERROR null crt\n");
    return -1;
  }
  
  int fpr = fprintf(fp, "%u\n", c->m);
  uint8_t i = 0;
  
  for(i = 0; i < c->m; i++) {
    fpr = file_save_poly(fp, c->crt[i]);
  }
  
  return fpr;
  
}
