#include <time.h>
#include "fine_grained_lock_BST.h"

pthread_t* threads;
size_t* args;
size_t NUM_THREADS;

/* Extern Variables */
struct node* actual_root;
pthread_mutex_t tree_lock;

void global_init()
{
	actual_root = NULL;
	pthread_mutex_init(&tree_lock,NULL);
	threads = (pthread_t*)malloc(NUM_THREADS*sizeof(pthread_t));
	args = (size_t*)malloc(NUM_THREADS*sizeof(size_t));
}

void global_cleanup()
{
	pthread_mutex_destroy(&tree_lock);
	free(threads);
	free(args);
}

/*
 * @func_name - thread_main
 * @Description - Callback function for spawned threads
 * @params - args: thread id
 * @return - void
 */
void* thread_main(void* args)
{
	size_t tid = *((size_t*)args);
	int lo_key, hi_key;

	//thread_local int seed = 1; 

	/*if(tid == 1)
	{
		put(actual_root, NULL, 5, 6);
	}

	else if(tid <= NUM_THREADS/3)
	{
		put(actual_root, NULL, 10, tid);
	}
	else if((tid > NUM_THREADS/3) && (tid <= ((NUM_THREADS/3)*2)))
	{
		get(actual_root, NULL, 10, tid);
	}
	else
	{
		printf("\nRange Queries btw %d and %d\n", 1, 55);
		range_queries(actual_root, NULL, 1, 55);
	}*/

	if(tid <= NUM_THREADS/3)
	{
		//srandom(5);
		srandom(((time(0) & 0xFFFFFFF0) | (tid+1)));
		//rand_key = random()%100;
		//printf("TID: %ld; Random Num: %ld\n", tid, (random()%100));
		put(actual_root, NULL, (random()%5), tid);
	}
	else if((tid > NUM_THREADS/3) && (tid <= ((NUM_THREADS/3)*2)))
	{
		//srandom(5);
		srandom(((time(0) & 0xFFFFFFF0) | ((tid-NUM_THREADS/3)+1)));
		//printf("TID: %ld; Random Num: %ld\n", tid, (random()%100));
		get(actual_root, NULL, (random()%5), tid);
	}
	else
	{
		do
		{
			lo_key = random()%5;
			hi_key = random()%5;
		}while(lo_key > hi_key);
		printf("\nRange Queries btw %d and %d\n", lo_key, hi_key);
		range_queries(actual_root, NULL, lo_key, hi_key);
	}
	//printf("Random number for tid %ld: %ld, %ld\n", tid, random(), random());

}

int main()
{
	int data;
	NUM_THREADS = 6;

	global_init();

/*	put(actual_root, NULL, 5, 1);
	put(actual_root, NULL, 4, 2);
	put(actual_root, NULL, 3, 3);
	put(actual_root, NULL, 2, 4);
	put(actual_root, NULL, 1, 5);*/

	/* Launch threads */
	int ret; size_t i;
  	for(i=1; i<NUM_THREADS; i++)
  	{
		args[i]=i+1;

		#ifdef DEBUG
		printf("creating thread %zu\n",args[i]);
		#endif

		ret = pthread_create(&threads[i], NULL, &thread_main, &args[i]);
		if(ret)
		{
			printf("ERROR; pthread_create: %d\n", ret);
			exit(-1);
		}
	}
	i = 1;
	thread_main(&i); // master also calls thread_main	

	// join threads
	for(i=1; i<NUM_THREADS; i++)
	{
		ret = pthread_join(threads[i],NULL);
		if(ret)
		{
			printf("ERROR; pthread_join: %d\n", ret);
			exit(-1);
		}

		#ifdef DEBUG
		printf("joined thread %zu\n",i+1);
		#endif
	}

	printf("\nInorder traversal\n");
	inorder(actual_root);

	global_cleanup();

	return 0;
}