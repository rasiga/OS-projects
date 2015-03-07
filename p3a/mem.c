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
		nextfitsize=sizeOfRegion * 0.75; // allocate size
		slabmemory=sizeOfRegion * 0.25;
		void *ptr = mmap(NULL, sizeOfRegion, PROT_READ|PROT_WRITE,MAP_ANON|MAP_PRIVATE, -1, 0);
		nextfithead=ptr+slabmemory;
		nextfithead->next=NULL;
		nextfithead->length=sizeOfRegion-sizeof(nextfithead)-slabmemory;
		slabvalue=slabSize;
		slabhead=ptr;
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
	int assigned=0;
	int add;
	if(size==slabvalue)
	{
		void *temp=slabhead;
		void *new=temp;
                temp=temp+slabvalue;
		slabhead=temp;
		assigned=1;
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
		add=(sizeof(process)+size)/sizeof(nextfithead);
		void *temp=nextfithead;
		temp=temp+size+sizeof(nextfithead);
		nextfithead=temp;
		nextfithead->next=NULL;
                nextfithead->length=currsize-size-sizeof(process);
		return (process+sizeof(process));		
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

