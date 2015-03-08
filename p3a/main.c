#include "mymem.h"
#include<stdio.h>
int main()
{
	printf("Meminit 4096 %p\n",Mem_Init(4096,128));
	Mem_Dump(); // prints info on address of nextfit and slab head
	printf("Mem_Alloc %p\n",Mem_Alloc(100)); // should be in next fit
        Mem_Dump();
        printf("Mem_Alloc %p\n",Mem_Alloc(128)); // should be in slab
        Mem_Dump();
	printf("Mem_Alloc %p\n",Mem_Alloc(128)); // should be in slab
        Mem_Dump();


	return 1;
}
