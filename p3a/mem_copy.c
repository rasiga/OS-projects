#include<stdio.h>
#include<sys/mman.h>
#include "mymem.h"

int slabvalue=0; //size of each slab region
int done=0; //mem_init flag variable
int nextfitsize=0,slabmemory=0,totalsize=0; // size of memory given to next fit and slab allocator (75% and 25% of free space respectively)
struct FreeHeader * nextfithead=NULL, *slabhead=NULL;
void* ASstart=NULL;
void * Mem_Init(int sizeOfRegion, int slabSize)
{
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
	if(done==0)
		return NULL;
	int assigned = 0;
	if(size == slabvalue && slabhead != NULL)
	{
		void *temp = slabhead;
		void *new = temp;
		temp = temp + slabvalue;
		assigned = 1;
		void **x = temp - 8;
		slabhead = *x;
		//bzero(new,slabvalue);
		return new;
	}
	if(assigned == 0)
	{
                if(size % 16 != 0)
                {
                        size = size + (16 - size % 16);
                }
                int magicvalue = MAGIC;
                int currsize = nextfithead->length;
                if(currsize >= size)
                {
                        struct AllocatedHeader* process = (struct AllocatedHeader*)nextfithead;
                        process->length = size;
                        process->magic = &magicvalue;
                        void *temp = nextfithead;
                        temp = temp + size + sizeof(struct FreeHeader);
                        nextfithead = temp;
                        nextfithead->next = NULL;
                        nextfithead->length = currsize - size - sizeof(struct AllocatedHeader);
                        void *p=process;
			printf("Magic %x\n",*(int *)process->magic);
                        return (p+sizeof(struct AllocatedHeader));
                }
	}
	return NULL;

}
int Mem_Free(void *ptr)
{
	//struct FreeHeader *newfreed;
        /*void *holder = ptr - sizeof(struct AllocatedHeader);
        struct AllocatedHeader *pp = (struct AllocatedHeader*)holder;

	printf("Magic %x\n",*(int *)pp->magic);
	*/

        int slabflag = 0;
        if(ptr == NULL)
        {
		return -1;
        }
	if(ptr > (ASstart + totalsize) || ptr < ASstart)
	{
		printf("SEGFAULT\n");
        	return -1;
	}
	if(ptr <= (ASstart + slabmemory))
	{
        	slabflag = 1;
	}
	if(slabflag ==1)
	{
		//int x=ptr-ASstart;
		//printf("Difference %d\n",x);
		if((ptr-ASstart)%slabvalue!=0)
			return -1;
       		void **p = ptr + slabvalue - 8;
      	 	void *pp=ptr;
       		*p = slabhead;
       		slabhead = pp;
		

	}
	else
	{
                struct FreeHeader *newfreed;
                void *holder = ptr - sizeof(struct AllocatedHeader);
                struct AllocatedHeader *pp = (struct AllocatedHeader*)holder;
                int magic=MAGIC;
                void *mag=&magic;
                if(*(int *)(holder+sizeof(int)) == *(int *)mag)
                {
                       printf("Yes matched:\n");
                }
                else
		{
			printf("No %x %x %x\n", *(int *)(pp->magic),1,1);
                        //printf("Hold %x %x %x \n",*(int*)holder,*(int*)(holder+16),*(int*)pp,*(int*)(pp->magic));
			return -1;
		}
	
                newfreed = (struct FreeHeader*)holder;
                newfreed->next = NULL;
                struct FreeHeader *p = nextfithead;
                while(p->next!=NULL)
                {
                        p = p->next;
                }
                p->next = newfreed;
	}
	
	return 0;
}

void Mem_Dump()
{
	//printf("Head length is %d Head is %p Slabhead is %p NextfitHead->next %p\n",nextfithead->length,nextfithead,slabhead,nextfithead->next);
	return;
}

