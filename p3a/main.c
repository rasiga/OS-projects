/* Fill half of next fit region. The next allocation after the fill should be the 
 * one that comes after the last allocation */
#include <assert.h>
#include <stdlib.h>
#include "mymem.h"
#include <stdio.h>

int main() {
   char *ptr = (char *)Mem_Init(4096, 64);
   Mem_Dump();
   assert(ptr != NULL);
   int i = 0;
   char *nfPtr,*nfPtr1,*nfPtr2,*nfPtr3;
   for(i=0; i<32; i++)
   {
   	nfPtr = (char *)Mem_Alloc(32);
	printf("\n I is:%d ",i);
	Mem_Dump();
	assert(nfPtr != NULL);
	assert(nfPtr-ptr-sizeof(struct AllocatedHeader) >= 1024);
	if (i == 13){
		nfPtr1 = nfPtr;
		printf("\nnfPtr1 is:%p",nfPtr1);
	}
	else if (i == 23){
		nfPtr2 = nfPtr;
		printf("\n nfPtr2 is:%p",nfPtr2);
	}
   }
  printf("\n Freeing nfPtr1!");
  assert(Mem_Free(nfPtr1) == 0);
  Mem_Dump();
  printf("\n Freeing nfPtr2!");
  assert(Mem_Free(nfPtr2) == 0);
  Mem_Dump();
  printf("\n Allocating the last 32");
   nfPtr3 = (char *)Mem_Alloc(32);
   Mem_Dump();
   assert(nfPtr3 != NULL);
   assert(nfPtr3-ptr-sizeof(struct AllocatedHeader) >= 1024);

   assert((nfPtr + 32 + sizeof(struct AllocatedHeader)) == nfPtr3);
   exit(0);
}
