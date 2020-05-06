/* params.c */

int foo(int x, int y), bar(double *p, int x, ...);

int foo(int x, int y)
{
    return -x + !y * 10 != 3;
}
