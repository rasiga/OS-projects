#include "mymem.h"
#include<stdio.h>
#include<pthread.h>
void *thread(void *arg)
{
	char *s=arg;
	printf("Function %s\n",s);
	void *p =Mem_Alloc(100);
	printf(" MAlloc return %p\n",p);
	Mem_Dump();
	return NULL;
}

int main()
{
	
	pthread_t pth[2];
	Mem_Init(4096,128);
	pthread_create(&pth[0],NULL,thread,"A");
	pthread_create(&pth[1],NULL,thread,"B");
	printf("Main\n");
	pthread_join(pth[1],NULL);
	pthread_join(pth[0],NULL);
	
	/*
	printf("Meminit 4096 %p\n",Mem_Init(4096,128));
	Mem_Dump(); // prints info on address of nextfit and slab head
	void *p=Mem_Alloc(100); // should be in next fit
	printf("Malloc address %p\n",p);
        Mem_Dump();
	printf("Free\n");
	Mem_Free(p);
	Mem_Dump();
	p=Mem_Alloc(100);
	printf("Malloc address %p\n",p);
        Mem_Dump();
     
	printf("Allocate slab\n");
        void *p=Mem_Alloc(128); // should be in slab
	printf("Mem_Alloc %p\n",p);
	 printf("Allocate slab\n");
	p=Mem_Alloc(128); // should be in slab
        printf("Mem_Alloc %p\n",p);
        Mem_Dump();
	 printf("Free slab\n");
	Mem_Free(p);
	Mem_Dump();
	 printf("Allocate slab\n");

	printf("Mem_Alloc %p\n",Mem_Alloc(128)); // should be in slab
        Mem_Dump();
	*/

	return 1;
}
