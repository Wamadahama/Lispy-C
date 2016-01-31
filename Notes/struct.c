/* structures are used to declare new types */
typedef struct{
  float x;
  float y;
} point;

int main(int argc, char const *argv[]) {
  point p;
  p.x = 0.1;
  p.y = 10.0;
  return 0;
}
