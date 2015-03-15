#include<stdio.h>
#include<string.h>
#include<sys/mman.h>
#include "mymem.h"

int slabvalue=0; //size of each slab region
int done=0; //mem_init flag variable
int /*nextfitsize=0,slabmemory=0,*/totalsize=0; // size of memory given to next fit and slab allocator (75% and 25% of free space respectively)
struct FreeHeader * nextfithead=NULL, *slabhead=NULL;
void* ASstart=NULL;
void *magic=NULL;
int magicval=MAGIC;
void * Mem_Init(int sizeOfRegion, int slabSize)
{
	if(done==0) // make sure mem_init called only once
	{
		int i;
		magic=&magicval;
		totalsize = sizeOfRegion;
		//nextfitsize=sizeOfRegion * 0.75; // allocate size
		//slabmemory=sizeOfRegion * 0.25;
		void *ptr = mmap(NULL, sizeOfRegion+100, PROT_READ|PROT_WRITE,MAP_ANON|MAP_PRIVATE, -1, 0);
		ASstart = ptr;
		//initializing nextfithead
		nextfithead = ptr+(int)(totalsize*0.25);
		nextfithead->next = NULL;
		nextfithead->length = sizeOfRegion-sizeof(struct FreeHeader)-(totalsize*0.25);
		//initializing slabhead and slabvalue
		slabvalue = slabSize;
		slabhead = ptr;
		if(ptr == MAP_FAILED)
		{
			//perror("mmap");
			return NULL;
		}
		int numofslabs = (sizeOfRegion * 0.25) / slabvalue;
		void *temp = slabhead;
		void **nextpart=NULL;
		//initializing the pointer to next free slab in each slab region
		for(i=0;i<numofslabs-1;i++)
		{
			nextpart = temp + slabvalue - 8;
			*nextpart = temp + slabvalue;
			temp = temp + slabvalue;
		}
		//initializing the next slab pointer for the last slab
		nextpart = temp + slabvalue - 8;
                *nextpart = NULL;
                //printf("\nStart Memory %p End memory %p\n",nextfithead,(ASstart+totalsize));
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
		bzero(new,slabvalue);
		return new;
	}
	if(assigned == 0)
	{
	   /*if((void*)nextfithead>=(ASstart+totalsize))
	   {
		nextfithead=NULL;
		nextfithead->next=NULL;
	   }
	   else*/ if(nextfithead != NULL)
	   {
                if(size % 16 != 0)
                {
                        size = size + (16 - size % 16);
                }
                //int magicvalue = MAGIC;
                int currsize = nextfithead->length;
                if(currsize >= size)
                {
                        struct AllocatedHeader* process = (struct AllocatedHeader*)nextfithead;
                        process->length = size;
                        process->magic = magic;
                        void *temp = nextfithead;
                        printf("\nComapare nextfithead and temp %p %p",temp,nextfithead);
			temp = temp + size + sizeof(struct FreeHeader);
			//void *temp2=temp;
			
			//if there is no free space in the next fit region
			if((void*)nextfithead > (ASstart+totalsize))
			{
				printf("Next fit head is null\n");
				nextfithead=NULL;
				nextfithead->next=NULL;
				nextfithead->length=0;
			}
			else
			{
                        nextfithead = temp;
                        nextfithead->next = NULL;
                        nextfithead->length = currsize - size - sizeof(struct AllocatedHeader);
			}
                        void *p=process;
			//printf("\nIn decimal %d",(temp-(ASstart+(int)(totalsize*0.25))));
			//printf("Magic %x\n",*(int *)process->magic);
			//printf("\nEnd memory %p\n",(ASstart+totalsize));
                        return (p+sizeof(struct AllocatedHeader));
                }
	 }
	else
		return NULL;
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
	int nextregionfree = 0,prevregionfree = 0;
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
	if(ptr <= (ASstart + (int)(totalsize*0.25)))
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
                struct AllocatedHeader *pp1 = (struct AllocatedHeader*)holder;
                //int magic=MAGIC;
               //void *mag=&magic;
                if((pp1->magic) != magic)
                {
			return -1;
		}
		struct FreeHeader *pp=(struct FreeHeader*)pp1;
                /*newfreed = (struct FreeHeader*)holder;
                newfreed->next = NULL;
                struct FreeHeader *p = nextfithead;
                while(p->next!=NULL)
                {
                        p = p->next;
                }
                p->next = newfreed;*/

		// if the next region is completely full before this free
		// make the nextfithead to poin to this and nextfithead->next=NULL
		if(nextfithead == NULL)
		{
			newfreed = (struct FreeHeader*)holder;
                	newfreed->next = NULL;
			nextfithead = newfreed;
			return 0;
		}
		//Coalescing
		// Check if next region is free
		void *next = ptr+pp->length;
		struct FreeHeader *p2=NULL;
		if(next< (ASstart+totalsize) )
		{
			p2 = (struct FreeHeader*)next;
			if((void*)p2->next != magic)
			{
				nextregionfree = 1;
				pp->length += p2->length+sizeof(struct AllocatedHeader);
			}
		}
	        //Check if previous region is free
                struct FreeHeader *p = nextfithead;
		struct FreeHeader *checkpoint=NULL;
                while(p->next!=NULL)
                {
			void *prevreg = p;
	                checkpoint = prevreg + sizeof(struct FreeHeader) + p->length;
        	        if((void*)checkpoint == (void*)pp)
                	{
				prevregionfree=1;
                        	checkpoint->length += sizeof(struct AllocatedHeader) + pp->length;
				break;
                	}
                        p = p->next;
                }
		if(p->next==NULL)
		{
			void *prevreg = p;
			checkpoint = prevreg + sizeof(struct FreeHeader) + p->length;
			if((void*)checkpoint == (void*)pp)
			{
				prevregionfree = 1;
				checkpoint->length += sizeof(struct AllocatedHeader) + pp->length;
			}
		}
		//make the free list pointers correct
		if(nextregionfree ==1)
		{
			struct FreeHeader *traverse = nextfithead;
        	        while(traverse->next!=NULL)
                	{
                		if(traverse->next == p2)
				{
					if(prevregionfree == 1)
					{
						traverse->next = checkpoint;
					}
					else
					{
						traverse->next = pp;
					}
				}
		        	traverse = traverse->next;		
               		}
		}
		if(nextregionfree ==0 && prevregionfree ==0)
		{
			struct FreeHeader *onlyfree = nextfithead;
			while(onlyfree->next!=NULL)
			{
				onlyfree = onlyfree->next;
			}
			onlyfree->next = pp;
		}
	
		
                //p->next = newfreed;
	}
	
	return 0;
}

void Mem_Dump()
{
	//printf("Head length is %d Head is %p Slabhead is %p NextfitHead->next %p\n",nextfithead->length,nextfithead,slabhead,nextfithead->next);
	return;
}

