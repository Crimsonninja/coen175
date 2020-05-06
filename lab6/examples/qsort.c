/* qsort.c */

int rand(void), printf(char *s, ...);

int exchange(int *a, int *b)
{
    int t;

    t = *a;
    *a = *b;
    *b = t;
}


int partition(int *a, int lo, int hi)
{
    int i, j, x, temp;


    x = a[lo];
    i = lo - 1;
    j = hi + 1;

    while (i < j) {
	j --;

	while (a[j] > x)
	    j --;

	i ++;

	while (a[i] < x)
	    i ++;

	if (i < j)
	    exchange(&a[i], &a[j]);
    }

    return j;
}


int quickSort(int *a, int lo, int hi)
{
    int i;


    if (hi > lo) {
	i = partition(a, lo, hi);
	quickSort(a, lo, i);
	quickSort(a, i + 1, hi);
    }
}


int main(void)
{
    int a[20], i;


    for (i = 0; i < 20; i ++)
	a[i] = rand() % 20;

    quickSort(a, 0, 20 - 1);

    for (i = 0; i < 20; i ++)
	printf("%d\n", a[i]);
}
