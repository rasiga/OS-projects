#include "mymem.h"
#include<stdio.h>
int main()
{
	printf("Meminit 4096 %p\n",Mem_Init(4096,16));
	Mem_Dump();
	printf("Mem_Alloc %p\n",Mem_Alloc(100));
        Mem_Dump();
        printf("Mem_Alloc %p\n",Mem_Alloc(16));
        Mem_Dump();
	return 1;
}
