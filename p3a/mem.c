#include<fcntl.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include "mymem.h"

int slabvalue;
int done=0;
int nextfitsize,slabmemory;
struct FreeHeader * nextfithead, *slabhead;
struct AllocatedHeader * process;
void * Mem_Init(int sizeOfRegion, int slabSize)
{
	if(done==0) // make sure me_init called only once
	{
		int i;
		nextfitsize=sizeOfRegion * 0.75; // allocate size
		slabmemory=sizeOfRegion * 0.25;
		void *ptr = mmap(NULL, sizeOfRegion, PROT_READ|PROT_WRITE,MAP_ANON|MAP_PRIVATE, -1, 0);
		nextfithead=ptr+slabmemory;
		nextfithead->next=NULL;
		nextfithead->length=sizeOfRegion-sizeof(struct FreeHeader)-slabmemory;
		slabvalue=slabSize;
		slabhead=ptr;
		if(ptr==MAP_FAILED)
		{
			//perror("mmap");
			return NULL;
		}
		int numofslabs=slabmemory/slabvalue;
		void *temp=slabhead;
		for(i=0;i<numofslabs-1;i++)
		{
			void ** nextpart=temp+slabvalue-8;
			*nextpart=temp+slabvalue;
			/*void * x1=temp+slabvalue;
			void **x2=temp+slabvalue-8;
			printf("Value at nextpart %p | %p\n",x1,*x2);*/
			//void *print=temp+slabvalue-8;;
			temp=temp+slabvalue;
		}
		void ** nextpart=temp+slabvalue-8;
                *nextpart=NULL;
                        
		done=1;
		return ptr;
	}
	else
	{
		return NULL;
	}

}
void * Mem_Alloc(int size)
{
	int assigned=0;
	if(size==slabvalue)
	{	
		// incomplete here
		void *temp=slabhead;
		void *new=temp;      // conversion to void required because head+slabvalue does head+(slabvalue*sizeof(head)) with void sizeof(void) is 1
                temp=temp+slabvalue;
		slabhead=temp;
		assigned=1;
		void *temp2=slabhead;
                temp2=temp2+slabvalue-8;
		struct FreeHeader **x;
		x=temp;
		//slabhead=*x;
		printf("Content at slabhead %p\n",(*x));
		bzero(new,slabvalue);
		return new;
                
	}
	if(assigned==0)
	{
		if(size%16!=0)
		{
			size=size+(16-size%16);
		}
		int magicvalue=MAGIC;
		int currsize=nextfithead->length;
		process=(struct AllocatedHeader*)nextfithead;		
		process->length=size;
		process->magic=&magicvalue;
		void *temp=nextfithead;
		temp=temp+size+sizeof(struct FreeHeader);
		nextfithead=temp;
		nextfithead->next=NULL;
                nextfithead->length=currsize-size-sizeof(struct AllocatedHeader);
		return (process+sizeof(struct AllocatedHeader));		
	}
	return NULL;

}
int Mem_Free(void *ptr)
{
	return 0;
}
void Mem_Dump()
{
	printf("Head length is %d Head is %p Slabhead is %p\n",nextfithead->length,nextfithead,slabhead);
	return;
}

