#include "types.h"
#include "stat.h"
#include "user.h"

struct mem
{
	int pid;
	void* memory_region;
};


int
main(int argc, char *argv[])
{
  int s=5;
  void *p;
  p=(void *)&s;
  int x=0;
  x = thread_create(sample,p);
 // printf(1,"Inside main %d\n",x);
  //printf(1,"clone %d join %d",clone(NULL,NULL,NULL),join(0));
  exit();
}


