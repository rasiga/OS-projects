#include<stdio.h>
#include<string.h>
#include<sys/mman.h>
#include<pthread.h>
#include "mymem.h"

int slabvalue = 0; //size of each slab region
int done = 0; //mem_init flag variable
int totalsize = 0; // size of memory given to next fit and slab allocator (75% and 25% of free space respectively)
struct FreeHeader * nextfithead = NULL, *slabhead = NULL, *nextfit = NULL;
void* ASstart = NULL;
void *magic = NULL;
int magicval=MAGIC;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
void * Mem_Init(int sizeOfRegion, int slabSize)
{
	if(done==0) // make sure mem_init called only once
	{
		int i;
		magic=&magicval;
		totalsize = sizeOfRegion;
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
	//to check if mem_init has been called
	if(done==0)
        {       
                return NULL;
        }
	//printf("\nMem_Alloc Lock call");
	pthread_mutex_lock(&lock);
	//printf("\nMem_Alloc lock acquired");
	int found = 0; //to look through the free list
	int assigned =0;
	struct FreeHeader * liststart = nextfit;
	/*if(nextfit == NULL && nextfithead!=NULL)
	{
		nextfit = nextfithead;
	}*/
	if(slabvalue == size && slabhead!=NULL)
	{
		void *temp = slabhead;
		void *new = temp;
		temp = temp + slabvalue;
		assigned = 1;
		void **x = temp - 8;
		slabhead = *x;
		bzero(new,slabvalue);
		//printf("\n Mem_Alloc unlock request");
		pthread_mutex_unlock(&lock);
		//printf("\n Mem_Alloc unlock done");
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
			//printf("\nLoop");	
			if(nextfit->length < size)
			{
				if(nextfit->next!=NULL)
					nextfit = nextfit->next;	
				else
					nextfit = nextfithead;
				
				if(nextfit ==  liststart)
					break;
				continue;
			}
			else
			{
				struct AllocatedHeader* process = (struct AllocatedHeader*)nextfit;
				int cal = nextfit->length - (size + sizeof(struct AllocatedHeader));
				found = 1;
				int inlist = 0; // added in list
				struct FreeHeader* temp = nextfit;
				//find the free space before nextfit in the free list
				struct FreeHeader* head=nextfithead;
				int remainfree = nextfit->length - size;
				// Only one exists
				if(head->next==NULL || head == temp) //you are allocating at the start of the list - could be the only chunk or there could be others after you
				{ //changed here!!
					/*if(head->next ==NULL)
						printf("\nOnly chunk");
					else 
						printf("\n TEMP AT START");*/
					//allocate for one and push this forward
					if(temp->length == size || remainfree < sizeof(struct FreeHeader))
					{
						if(head->next == NULL)
						{
							//printf("\nAll Alloc");
							temp = NULL;
							nextfithead = NULL;
						}
						else
						{
							//change the nextfithead and nextfit to point to the one after you
							nextfithead = nextfithead->next;
							temp = nextfithead;
						}
					}
					else
					{
					 // if part of this chunk will be allocated, change the nextfit accordingly
                                
                                        	void *x = temp;//adding sizeof struct should be with void*
                                        	x = x + sizeof(struct AllocatedHeader) + size;
                                        	temp = x;
                                        	temp->length = cal;
						if(head->next == NULL)
                                        		temp->next = NULL;
						else
							temp->next = nextfithead->next;
						//nextfit = temp;
						nextfithead = temp;
					}
					inlist = 1;
				}
				else
				{
					 while(head->next!=temp/* && inlist == 0*/)
					 {
						head=head->next;
					 }
					if( temp->next !=NULL  )
					{
					//	printf("\nTEMP AT MIDDLE");
							// need to put in the middle
						if(temp->length == size || remainfree < sizeof(struct FreeHeader))
                                        	{
							head->next = temp->next;
							temp = temp->next; // because temp is allocated now
						}
						else
						{
							void *x = temp;//adding sizeof struct should be with void*
                                                	x = x + sizeof(struct AllocatedHeader) + size;
                                                	temp = x;
                                                	temp->length = cal;
                                                	temp->next=nextfit->next;
							head->next=temp;
							//nextfit=temp;	
						}		
						
						inlist = 1;
						break;
					}
					if(inlist == 0 ) // temp is at the end was at the end temp->next == NULL
					{
						//printf("\nTEMP AT END");
						
						if(temp->length == size || remainfree < sizeof(struct FreeHeader))
                                                {		// whole thing allocated
							 
                                                        head->next = NULL;
                                               		temp = nextfithead; // because temp is allocated now
                                                }
						else
						{
							
							void *x = temp;//adding sizeof struct should be with void*
                                                        x = x + sizeof(struct AllocatedHeader) + size;
                                                        temp = x;
                                                        temp->length = cal;
							temp->next = NULL;
							printf("\n head is:%p",head);
							head->next = temp;
							//nextfit = temp;

						}
					
					}
             			}
				process->length = size;
				process->magic = magic;
				void *p = process;
				nextfit = temp;
				//printf("\nMem_Alloc unlock request");
				pthread_mutex_unlock(&lock);
				//printf("\n Mem_Alloc unlock done");
				return (p+sizeof(struct AllocatedHeader));		   
			}
		}
		
	     }
	 }
	//printf("\nMem_Alloc unlock request");
        
	pthread_mutex_unlock(&lock);
	//printf("\n Mem_Alloc unlock done");
	return NULL;

}
int Mem_Free(void *ptr)
{
	//printf("\nMem_free Lock call");
	pthread_mutex_lock(&lock);
	//printf("\nMem_Free Lock acquired");
        
	int nextregionfree = 0,prevregionfree = 0;
        int slabflag = 0;
        if(ptr == NULL)
        {
		//printf("\nMem_Free unlock request");
		pthread_mutex_unlock(&lock);
		//printf("\n Mem_Free unlock done");
		return -1;
        }
	if(ptr > (ASstart + totalsize) || ptr < ASstart)
	{
		printf("SEGFAULT\n");
		//printf("\nMem_Free unlock request");
		pthread_mutex_unlock(&lock);
		//printf("\nMem_Freeunlock done");
        	return -1;
	}
	if(ptr <= (ASstart + (int)(totalsize*0.25))) //to check if address is within the slab region
	{
		//printf("\n Freeing in slab region!");
        	slabflag = 1;
	}
	if(slabflag ==1)
	{
		if((ptr - ASstart) % slabvalue!=0) // to check for bad pointer
		{	                
			//printf("\nMem_Free unlock request");
			pthread_mutex_unlock(&lock);
			//printf("\n Mem_Free unlock done");
			return -1;
		}
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
		        //printf("\nMem_Free unlock request");
			pthread_mutex_unlock(&lock);
			//printf("\n Mem_Free unlock done");
			return -1;
		}
		struct FreeHeader *pp=(struct FreeHeader*)pp1;
                // if the entire next fit region(75% of total space) was completely full before this free
		// make the nextfithead and nextfit to point to this 
		if(nextfithead == NULL)
		{
			newfreed = (struct FreeHeader*)holder;
                	newfreed->next = NULL;
			nextfithead = newfreed;
			nextfit = newfreed;
	                //printf("\nMem_Free unlock request");
			pthread_mutex_unlock(&lock);
			//printf("\n Mem_Free unlock done");
			return 0;
		}
		// check if can be added at start of head
		
		 if(pp<nextfithead)
                {
                        //newfreed = (struct FreeHeader*)holder;
                        /*newfreed->next = NULL;
                        nextfithead = newfreed;
                        nextfit = newfreed;*/
			void *next = ptr + pp->length;
			struct FreeHeader *p2=NULL;
			//IDA: if the next region is free, then it would definitely be pointed to by nextfithead
			if(next < (ASstart+totalsize) )
                	{
                        	p2 = (struct FreeHeader*)next;
                        	if((void*)p2->next != magic)
                        	{
                                	nextregionfree = 1;
                		        pp->length += p2->length+sizeof(struct FreeHeader);
                        	}
                	}
			if(nextregionfree == 0)
			{
				pp->next=nextfithead;
				nextfithead=pp;
			}
			else
			{
				pp->next=nextfithead->next;
				nextfithead=pp;
			}
             		pthread_mutex_unlock(&lock);
                        //printf("\n Mem_Free unlock done");
                        return 0;
                                                               
                }
		// if the to-be-freed region is below the nextfithead
		//Coalescing
		// Check if next region is free
		void *next = ptr + pp->length;
		struct FreeHeader *p2=NULL;
		if(next < (ASstart+totalsize) )
		{
			p2 = (struct FreeHeader*)next;
			if((void*)p2->next != magic)
			{
				nextregionfree = 1;
				//printf("\nnext region free!\n");
				pp->length += p2->length+sizeof(struct FreeHeader);
			}
		}
	        //Check if previous region is free
                struct FreeHeader *p = nextfithead;
		struct FreeHeader *checkpoint=NULL;
                while(p->next!=NULL)
                {
			//printf("\nLoop in free 1");
			void *prevreg = p;
	                checkpoint = prevreg + sizeof(struct FreeHeader) + p->length;
        	        if((void*)checkpoint == (void*)pp)
                	{
				prevregionfree=1;
				//printf("\nPrev region free!\n");
                        	p->length += sizeof(struct AllocatedHeader) + pp->length;
				//p->next = pp->next; //CHANGED:Remember pp is the newly to-be-freed location! doesnt have a next value!
				break;
                	}
                        p = p->next;
                }
		if(p->next==NULL) // previous region was last of free list
		{
			void *prevreg = p;
			checkpoint = prevreg + sizeof(struct FreeHeader) + p->length;
			if((void*)checkpoint == (void*)pp)
			{
				prevregionfree = 1;
				//printf("Prev region free!\n");
				p->length += sizeof(struct AllocatedHeader) + pp->length;
			}
		}
		// p represents previous free if exists and p2 represents next free if exists
		if(nextregionfree==1 && prevregionfree==1)
			printf("\nNeed to coalesce");	
		//make the free list pointers correct
		if(nextregionfree == 1)
		{
			//CHANGED: Can't happen because you already know that pp>nextfithead
			/*if(nextfithead == p2) // if the next region is the beginning of the free list (ie freehithead)
			{
				if(prevregionfree == 1) //this whole chunk from previous region to the end of this fre space pointed by nextfitfree
				{
					p->next=p2->next; // TODO 
					nextfithead = p;
				}
				else
				{
					pp->next = nextfithead->next;
					nextfithead = pp;	
				}
			}
			else
			{*/
				struct FreeHeader *traverse = nextfithead;
        	        	while(traverse->next!=NULL)
                		{
					printf("\nFree");
                			if(traverse->next == p2)
					{
						if(prevregionfree == 1)
						{
							//traverse->next = p;
							p->next = p2->next;// if p and p2 (ie previous and next regions are free), only possibility was your prev was pointing to next.
						}
						else
						{
							traverse->next = pp;
							pp->next = p2->next;
						}
						break;
					}
					else 
					{
						traverse = traverse->next;
						continue;	
					}	
               			}
			//}
		}
		if(nextregionfree == 0 && prevregionfree == 0)
		{
			//CHANGED: you are not before nextfithead nor are you immediately before or after a free chunk, 
			//so, traverse through the free list and find your spot!
			struct FreeHeader *onlyfree = nextfithead;
			while(onlyfree->next!=NULL)
			{
				if(onlyfree->next > pp) //means you are after onlyfree
				{
					pp->next = onlyfree->next;
					onlyfree->next = pp;
					break;
				}
				onlyfree = onlyfree->next;
			}
			// you are to be added to the end of the list
			if(onlyfree->next == NULL)
			{
				onlyfree->next = pp;
				pp->next = NULL;
			}
		}
	}
        //printf("\nMem_Free unlock request");
	pthread_mutex_unlock(&lock);
	//printf("\n Mem_Free unlock done");
	return 0;
}

void Mem_Dump()
{
	if(nextfithead!=NULL)
		printf("\nNext fit Head is %p Slabhead is %p Nextfit is %p nextfithead->length is %d nextfithead->next is %p",nextfithead,slabhead,nextfit,nextfithead->length,nextfithead->next);
	return;
}

