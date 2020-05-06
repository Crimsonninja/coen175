int x, a[10];

char f(void);

int x;

int f(void);      /*line 7: conflicting types for 'f'*/


double h(int c, char g){
  int d;
  int d;            /*line 12: redeclaration of 'd'*/
  g = 5;
  eu = f;           /*line 14: 'eu' undeclared*/
  {
    int x, y, a;
  }
  {
    int x, g, a;
    nope = 9;     /*line 19: 'nope' undeclared*/
    nope = 10;    /* Even though this is an undeclared error, only the first one should be mentioned" */
  }
}

double h(int c, char t),d;
char h(void);       /*line 24: conflicting types for 'h'*/
char a[10];         /*line 25: conflicting types for 'a'*/
int a;              /*line 26: conflicting types for 'a'*/

double h(int d, char er){     /*line 28: redefinition of 'h'*/
}
double **df(void);

int d;                /*line 32: conflicting types for 'd'*/

double hi(int x, int y, ...);
double hi(int d, int y, ...){
}
double hi(int d, int y);      /*line 37: conflicting types for 'hi'*/
double hi(int d, int d, ...); /*line 39: redeclaration of 'd'*/
