int *g(void);
int main(int x, char *y,...) {
  int z, q, w;
  double d;
  char a[10];
  int *p;

  if (y == g)
    x++;

  if (y != g)
    x++;

  if (y <= g)
    x++;

  if (y >= g)
    x++;

  if (y < g)
    x++;

  if (y > g)
    x++;

  if (z == a[0]) {
    q = z + q;
    d = d + z;
    d = d + d;
    p = p + z;
    p = z + p;
    y = y + w;
    q = d - q;
    q = p - p;
    y = a + q;

    /*wrong cases*/
    p = q - p;
    y = a + a;

  }

  z = d % q;
  z = q % d;

  z = a[0] * *y;
  z = a[0] / *y;

  z = a * y;
  z = a / y;
}
