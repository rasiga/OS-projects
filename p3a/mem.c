#include<stdio.h>
#include<string.h>
#include<sys/mman.h>
#include "mymem.h"

int slabvalue=0; //size of each slab region
int done=0; //mem_init flag variable
int /*nextfitsize=0,slabmemory=0,*/totalsize=0; // size of memory given to next fit and slab allocator (75% and 25% of free space respectively)
struct FreeHeader * nextfithead=NULL, *slabhead=NULL, *nextfit=NULL;
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
		void *ptr = mmap(NULL, sizeOfRegion, PROT_READ|PROT_WRITE,MAP_ANON|MAP_PRIVATE, -1, 0);
		ASstart = ptr;
		//initializing nextfithead
		nextfithead = ptr+(int)(totalsize*0.25);
		nextfithead->next = NULL;
		nextfithead->length = sizeOfRegion-sizeof(struct FreeHeader)-(totalsize*0.25);
		//next fit pointer initialization
		nextfit = nextfithead;
		
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
/***********************************************************************************************************************************************************
 * Mem_Alloc
 * ******************************************************************************************************************************************************************************/
void * Mem_Alloc(int size)
{
	int found = 0; //to look through the free list
	int assigned =0;
	if(nextfit == NULL && nextfithead!=NULL)
	{
		nextfit = nextfithead;
	}
	//to check if mem_init has been called
	if(done==0)
		return NULL;
	if(slabvalue == size && slabhead!=NULL)
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
	//if not assigned in the slab region
	if(assigned == 0)
	{
	   
	   if(nextfithead != NULL)
	   {
		// make is 16-bit aligned
		if(size % 16 !=0)
                {
                        size = size + (16 - size % 16);
                }
                //look through the free list until you find a free region >=size
                while(found==0 && nextfit!=NULL)
		{
			if(nextfit->length < size)
			{
				nextfit = nextfit->next;	
				continue;
			}
			else
			{
				struct AllocatedHeader* process = (struct AllocatedHeader*)nextfit;
				int cal = nextfit->length - (size + sizeof(struct FreeHeader));
				found = 1;
				struct FreeHeader* temp = nextfit;
             			//check if the whole chunk will be allocated- in that case move the nextfit accordingly
				if(temp->length == size)
				{
					if(temp->next !=NULL)
					{
						temp = temp->next;
					}
					else if(nextfithead == temp)
					{
					//	printf("NULLing");
						nextfithead = NULL; 
						nextfit = NULL;
					}
					else
					{
						temp = nextfithead;
					}
				} 
				else // if part of this chunk will be allocated, change the nextfit accordingly
				{
					void *x=temp;
					x=x+sizeof(struct FreeHeader) + size;
					temp=x;
					//temp = temp + sizeof(struct FreeHeader) + size;
					temp->length = cal;
					temp->next = nextfit->next;					
				}
				process->length = size;
				process->magic = magic;
				if(nextfithead == nextfit && nextfithead!=NULL)
					nextfithead = temp;
				if(nextfit!=NULL)
					nextfit = temp;
				void *p = process;
				return (p+sizeof(struct AllocatedHeader));		   
			}
		}
		
	     }
                /*int currsize = nextfithead->length;
                if(currsize >= size)
                {
                        struct AllocatedHeader* process = (struct AllocatedHeader*)nextfithead;
                        void *temp = nextfithead;
                        //printf("\nComapare nextfithead and temp %p %p",temp,nextfithead);
			temp = temp + size + sizeof(struct FreeHeader);
			//void *temp2=temp;
			
			//if there is no free space in the next fit region
			if((void*)nextfithead > (ASstart+totalsize))
			{
//				printf("Next fit head is null\n");
				nextfithead=NULL;
				nextfithead->next=NULL;
				nextfithead->length=0;
			}
			else
			{
				struct FreeHeader* next=nextfithead->next;
                                int cal = currsize - size - sizeof(struct FreeHeader);
                                       
                        	//nextfithead = temp;
				if(next!=NULL)
				{
					if(cal<=0)
					{
						nextfithead=next;
					}
					else
					{
						
						nextfithead=(struct FreeHeader*)temp;
						nextfithead->length=cal;
						nextfithead->next=next;

					}
				}
				else
				{
					nextfithead=(struct FreeHeader*)temp;
                                        nextfithead->length=cal;
                                        nextfithead->next=NULL;

				}
		
			}
			process->length = size;
                        process->magic = magic;
                        void *p=process;
			//
			nextfit=(struct FreeHeader*)temp;
			//printf("\nIn decimal %d",(temp-(ASstart+(int)(totalsize*0.25))));
			//printf("Magic %x\n",*(int *)process->magic);
			//printf("\nEnd memory %p\n",(ASstart+totalsize));
                        return (p+sizeof(struct AllocatedHeader));
                }
		else
		{
			//traverse list to find nextfit

		}*/
	 }
	return NULL;

}
int Mem_Free(void *ptr)
{
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
	if(ptr <= (ASstart + (int)(totalsize*0.25))) //to check if address is within the slab region
	{
        	slabflag = 1;
	}
	if(slabflag ==1)
	{
		if((ptr - ASstart) % slabvalue!=0) // to check for bad pointer
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
//		printf("ptr is:%p\n",ptr);
//		printf("Holder is :%p\n",holder);
                struct AllocatedHeader *pp1 = (struct AllocatedHeader*)holder;
                if((pp1->magic) != magic) // check for bad pointer
                {
			return -1;
		}
		struct FreeHeader *pp=(struct FreeHeader*)pp1;
                // if the entire next fit region(75% of total space) was completely full before this free
		// make the nextfithead to point to this and nextfithead->next=NULL
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
//				printf("next region free!\n");
				pp->length += p2->length+sizeof(struct FreeHeader);
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
//				printf("Prev region free!\n");
                        	p->length += sizeof(struct AllocatedHeader) + pp->length;
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
//				printf("Prev region free!\n");
				p->length += sizeof(struct AllocatedHeader) + pp->length;
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
			pp->next = NULL;
		}
	}
	
	return 0;
}

void Mem_Dump()
{
//	if(nextfithead!=NULL)
//	printf("Next fit Head is %p Slabhead is %p Nextfit is %p nextfithead->length is %d nextfithead->next is %p\n",nextfithead,slabhead,nextfit,nextfithead->length,nextfithead->next);
	return;
}

