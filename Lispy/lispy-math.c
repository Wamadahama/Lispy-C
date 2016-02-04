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

long min(long x, long y){
  if (x < y) {
    return x;
  }else{
    return y;
  }
}

long max(long x, long y){
  if (x > y) {
    return x;
  } else {
    return y;
  }
}
