#include <stdio.h>

// Function prototype
int add_together(int x, int y);

int main(int argc, char const *argv[]) {
  int added = add_together(10, 18);
  return 0;
}

int add_together(int x, int y){
  int result = x + y;
  return result;
}
