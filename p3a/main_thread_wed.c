/* multi threaded alloc and free calls */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "mymem.h"

#define MAX 100

char* buffer[MAX];
int fill = 0;
int use = 0;
int count = 0;
int loops = 4;

void put(char *ptr)
{
	buffer[fill] = ptr;
	fill = (fill + 1) % MAX;
	count++;
}

char* get()
{
	char* tmp = buffer[use];
	use = (use + 1) % MAX;
	count--;
	return tmp;
}

pthread_cond_t *empty, *full;
pthread_mutex_t *mutex;

void* producer(void *arg)
{
	int i;
	char *nfPtr = NULL;
	for(i=0; i<loops; i++)
	{
		nfPtr = NULL;
		nfPtr = Mem_Alloc(32);
		printf("\n Alloc %d %s",i,(char*)arg);
		Mem_Dump();
		printf("\n need mutex in producer");
		pthread_mutex_lock(mutex);
		printf("\n Inside Mutex lock in producer");
		while (count == MAX)
			pthread_cond_wait(empty, mutex);
		assert(nfPtr != NULL);
		put(nfPtr);
		pthread_cond_signal(full);
		printf("\n Going to be done with mutex lock in producer");
		pthread_mutex_unlock(mutex);
		printf("\nDone with mutex lock in producer");
	}
	return NULL;
}

void* consumer(void *arg)
{
	int i;
	char *nfPtr = NULL;
	for(i=0; i<loops*2; i++)
	{
		printf("\n need mutex in consumer");
		pthread_mutex_lock(mutex);
		printf("\ngot mutex lock in consumer");
		while (count == 0)
			pthread_cond_wait(full, mutex);
		nfPtr = get();
		assert(Mem_Free(nfPtr) == 0);
		printf("\n Free %d",i);
		Mem_Dump();
		pthread_cond_signal(empty);
		printf("\nGoing to unlock mutex in consumer");
		pthread_mutex_unlock(mutex);
		printf("\n Got unlock mutex in consumer");
	}
	return NULL;
}


void initSync()
{
	mutex = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
	empty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	full = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_mutex_init (mutex, NULL);	
	pthread_cond_init (full, NULL);
	pthread_cond_init (empty, NULL);
}


int main()
{
	assert(Mem_Init(8192,64) != NULL);

	initSync();
	Mem_Dump();
	pthread_t p1,c1,p2;//,p2;

	pthread_create(&p1, NULL, producer, "A");
	pthread_create(&p2, NULL, producer, "B");
	pthread_create(&c1, NULL, consumer, NULL);
	//pthread_create(&c2, NULL, consumer, NULL);
	
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);
	pthread_join(c1, NULL);
	//pthread_join(c2, NULL);

	exit(0);	
}
