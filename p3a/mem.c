#include<fcntl.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<pthread.h>
#include "mymem.h"


int slabvalue; //size of each slab region
int done=0; //mem_init flag variable
int nextfitsize,slabmemory,totalsize; // size of memory given to next fit and slab allocator (75% and 25% of free space respectively)
struct FreeHeader * nextfithead, *slabhead;
struct AllocatedHeader * process;
void* ASstart;
pthread_mutex_t lock;
void * Mem_Init(int sizeOfRegion, int slabSize)
{
	if(pthread_mutex_init(&lock, NULL) != 0)
    	{
        	printf("\n mutex init failed\n");
        	return NULL;
    	}
	if(done==0) // make sure mem_init called only once
	{
		int i;
		totalsize = sizeOfRegion;
		nextfitsize=sizeOfRegion * 0.75; // allocate size
		slabmemory=sizeOfRegion * 0.25;
		void *ptr = mmap(NULL, sizeOfRegion, PROT_READ|PROT_WRITE,MAP_ANON|MAP_PRIVATE, -1, 0);
		ASstart = ptr;
		//initializing nextfithead
		nextfithead = ptr+slabmemory;
		nextfithead->next = NULL;
		nextfithead->length = sizeOfRegion-sizeof(struct FreeHeader)-slabmemory;
		//initializing slabhead and slabvalue
		slabvalue = slabSize;
		slabhead = ptr;
		if(ptr == MAP_FAILED)
		{
			//perror("mmap");
			return NULL;
		}
		int numofslabs = slabmemory / slabvalue;
		void *temp = slabhead;
		//initializing the pointer to next free slab in each slab region
		for(i=0;i<numofslabs-1;i++)
		{
			void ** nextpart = temp + slabvalue - 8;
			*nextpart = temp + slabvalue;
			//void * x1=temp+slabvalue;
			//void **x2=temp+slabvalue-8;
			//printf("Value at nextpart %p | %p\n",x1,*x2);
			//void *print=temp+slabvalue-8;;
			temp = temp + slabvalue;
		}
		//initializing the next slab pointer for the last slab
		void ** nextpart = temp + slabvalue - 8;
                *nextpart = NULL;
                        
		done = 1; // setting the flag to ensure that free space is allocated only once
		return ptr;
	}
		return NULL;

}
void * Mem_Alloc(int size)
{
	pthread_mutex_lock(&lock);
	int assigned = 0;
	//if the requested size is equal to slab size and if there is atleast ine free slab, then allocated in slab region
	if(size == slabvalue && slabhead != NULL)
	{	
		// incomplete here
		void *temp = slabhead;
		void *new = temp;      // conversion to void required because head+slabvalue does head+(slabvalue*sizeof(head)) with void sizeof(void) is 1
                temp = temp + slabvalue;
		//slabhead = temp;
		assigned = 1;
                void **x = temp - 8;
		slabhead = *x;
		//printf("Content at slabhead %p\n",(*x));
		bzero(new,slabvalue);
		pthread_mutex_unlock(&lock);
		return new;
                
	}
	//if requested size is not equal to slab size, then use the next fit allocator
	if(assigned == 0)
	{
		if(size % 16 != 0)
		{
			size = size + (16 - size % 16); //nearest multiple of 16
		}
		int magicvalue = MAGIC;
		int currsize = nextfithead->length;
		if(currsize >= size)
		{
			process = (struct AllocatedHeader*)nextfithead;
			//printf(" process val %p\n",process);		
			process->length = size;
			process->magic = &magicvalue;
			void *temp = nextfithead;
			temp = temp + size + sizeof(struct FreeHeader);
			nextfithead = temp;
			nextfithead->next = NULL;
                	nextfithead->length = currsize - size - sizeof(struct AllocatedHeader);
			//printf("Process %p sizeof value %ld\n",process,sizeof(struct AllocatedHeader));
			void *p=process;
			pthread_mutex_unlock(&lock);
			return (p+sizeof(struct AllocatedHeader));	
		}
		else
		{
			//logic to go through the free list from current location in list
		}
	}
	pthread_mutex_unlock(&lock);
	return NULL;

}
int Mem_Free(void *ptr)
{
	pthread_mutex_lock(&lock);
	int slabflag = 0;
	if(ptr == NULL)
	{
		//do nothing
		pthread_mutex_unlock(&lock);
		return 0;
	}
	//printf("%p  --  %p\n",ptr,(ASstart+totalsize));
	if(ptr > (ASstart + totalsize) || ptr<ASstart) //check if ptr is within the mmaped region
	{
		printf("SEGFAULT\n");
		pthread_mutex_unlock(&lock);
		return -1;
	}
	//determine if ptr points to slab or next fit
	if(ptr <= (ASstart + slabmemory))
	{
		slabflag = 1;
	}
	//freeing ptr from slab
	if(slabflag ==1)
	{
		//make this ptr nextfree to point to what slabhead was pointing to before
		void **p = ptr + slabvalue - 8;
		void *pp=ptr;
		*p = slabhead;	
		//make slabhead point to this newly freed slab
		slabhead = pp;

	}
	//freeing ptr in nextfit
	else
	{
		struct FreeHeader *newfreed;
		//struct AllocatedHeader *oldalloc;
		void *holder = ptr - sizeof(struct AllocatedHeader);
		//oldalloc = (struct AllocatedHeader*)holder;
		newfreed = (struct FreeHeader*)holder;
		newfreed->next = NULL;
				
		struct FreeHeader *p = nextfithead;
		while(p->next!=NULL)
		{
			p = p->next;
		}	
		p->next = newfreed;//- sizeof(struct FreeHeader);//newly freed space		
	}
	pthread_mutex_unlock(&lock);
	return 0;
}
void Mem_Dump()
{
	printf("Head length is %d Head is %p Slabhead is %p NextfitHead->next %p\n",nextfithead->length,nextfithead,slabhead,nextfithead->next);
	return;
}

