#include "measure.h"

int64_t time_in_ms(struct timespec *timeA_p, struct timespec *timeB_p) {
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
         ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}
