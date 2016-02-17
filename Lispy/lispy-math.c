#include <stdio.h>

#include "lispy-math.h"

/* Typical exponent operation */
long power(long base, unsigned long exp){
  int i, result = 1;
  for(i = 0; i < exp; i++){
    result *= base;
  }

  return result;
}

/* Returns the smaller of two values*/
long min(long x, long y){
  if (x < y) {
    return x;
  }else{
    return y;
  }
}

/* Returns the larger of two values */
long max(long x, long y){
  if (x > y) {
    return x;
  } else {
    return y;
  }
}
