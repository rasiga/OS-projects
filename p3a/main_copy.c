#include "mymem.h"
#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
int main()
{
	char *ptr = (char *)Mem_Init(4096, 64);
	assert(ptr != NULL);
   	Mem_Dump();
//	int i = 0;
   	/*char *nfPtr = NULL, *nfPtr1 = NULL, *nfPtr2 = NULL;
   	for(i=0; i<64; i++)
   	{
		if(i == 13)
		{
			nfPtr1 = (char *)Mem_Alloc(32);
			printf("nfPtr for 13:%p\n",nfPtr1);
			Mem_Dump();
			assert(nfPtr1 != NULL);
		}
		else if(i == 14)
		{
			nfPtr2 = (char *)Mem_Alloc(32);
			printf("nfPtr for 14:%p\n",nfPtr2);
			Mem_Dump();
			assert(nfPtr2 != NULL);
		}
		else
		{
			nfPtr = (char *)Mem_Alloc(32);
			Mem_Dump();
			if(nfPtr == NULL)
				printf("\n%d failed\n",i);
		}
		printf("%d\n",i);
   	}
   	if(Mem_Alloc(32) == NULL)
	{
		printf("Assert failed\n");

	}
	printf("Freeing 13\n");
  	assert(Mem_Free(nfPtr1) == 0);
	Mem_Dump();
	printf("Freeing 14\n");
   	assert(Mem_Free(nfPtr2) == 0);
   	Mem_Dump();
	printf("allocating 55\n");
   	nfPtr = (char *)Mem_Alloc(55);
	printf("55 allocated at %p",nfPtr);
   	assert(nfPtr != NULL);
	Mem_Dump();
   	assert(nfPtr == nfPtr1);*/
	printf("Allocating 32");
	void* p1 = Mem_Alloc(32);
	Mem_Dump();
	printf("Allocating 55");
	void *p3 = Mem_Alloc(55);
	Mem_Dump();
	assert(p3!=NULL);
	printf("Freeing the 32");
	Mem_Free(p1);
	Mem_Dump();
	 printf("Freeing the 55");
        Mem_Free(p3);
        Mem_Dump();

   	exit(0);



/*
	
	char *ptr = (char *)Mem_Init(4096, 64);
   	assert(ptr != NULL);
   	int i = 0;
   	//char *nfPtr;
   	for(i=0; i<64; i++)
   	{
   		//Mem_Alloc(32);
		printf("\n%d - %p",i,Mem_Alloc(32));
		//assert(nfPtr != NULL);
		//assert(nfPtr-ptr-sizeof(struct AllocatedHeader) >= 1024);
   	}
	printf("\n%p",Mem_Alloc(32));
   	//assert(Mem_Alloc(32) == NULL);
   
	
	
	printf("Meminit 4096 %p\n",Mem_Init(4096,128));
	void *p=Mem_Alloc(100); // should be in next fit
	printf("Malloc address %p\n",p);
        //struct AllocatedHeader *pp = p - sizeof(struct AllocatedHeader);
        p=Mem_Alloc(128); // should be in next fit
        printf("Malloc address %p\n",p);
        //struct AllocatedHeader *pp = p - sizeof(struct AllocatedHeader);
        //        int i=Mem_Free(p);
        //                printf("%d\n",i);
        //
        
	int i=Mem_Free(p);
	printf("%d\n",i);


	assert(Mem_Init(4096,64) != NULL);
   	void* ptr = Mem_Alloc(32);
   	assert(ptr != NULL);
   	printf("%d",Mem_Free(ptr));*/
   	exit(0);
}
