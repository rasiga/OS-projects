#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include "mymem.h"

int slabvalue;
int done=0;
int nextfitsize,slabsize;
void *slabfreehead;
struct FreeHeader * head,*slabhead,*nextfithead;
struct AllocatedHeader * process;
void * Mem_Init(int sizeOfRegion, int slabSize)
{
	if(done==0)
	{
		nextfitsize=sizeOfRegion * 0.75;
		slabsize=sizeOfRegion * 0.25;
		void *ptr = mmap(NULL, sizeOfRegion, PROT_READ|PROT_WRITE,MAP_ANON|MAP_PRIVATE, -1, 0);
		head=ptr;
		head->next=NULL;
		head->length=sizeOfRegion-sizeof(head);
		slabvalue=slabSize;
		if(ptr==MAP_FAILED)
		{
			//perror("mmap");
			return NULL;
		}
		done=1;
		//printf("\nGot memory at %p\n",ptr);
		return ptr;
	}
	else
	{
		return NULL;
	}

}
void * Mem_Alloc(int size)
{
	/*int assigned=0;
	if(size<slabvalue)
	{

	}
	if(assigned==0)
	{*/
		int magicvalue=MAGIC;
		int currsize=head->length;
		struct FreeHeader* newfree;
		newfree=head;
		process=(struct AllocatedHeader*)head;		
		process->length=size;
		process->magic=&magicvalue;
		head=head+sizeof(process)+size;
		head->next=NULL;
                head->length=currsize-size-sizeof(head);
              
		return (process+sizeof(process));		
	//}


}
int Mem_Free(void *ptr)
{
	return 0;
}
void Mem_Dump()
{
	printf("Head length is %d\n",head->length);
	return;
}

