#include<stdio.h>
#include "mymem.h"
int main()
{
	int a=5;
	int *x=&a;
	int **y=x;
	*y=52;
	printf("%d",a);
	return 1;
}
