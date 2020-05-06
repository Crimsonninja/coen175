int printf(char *s, ...);

int main(void) {
  int i;
  int n;
  int j;
  n = 10;
  for (i = 1; i < n; i++) {
    for (j = 1; j < n; j++) {
      printf("Hello\t%d\t%d\n",i,j);
      if (j == 4)
        break;
    }

    if (i == 5)
      break;
  }
}
