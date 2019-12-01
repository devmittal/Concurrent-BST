#include <time.h>
#include "fine_grained_lock_BST.h"
#include "rw_lock_BST.h"
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

pthread_t* threads;
size_t* args;
size_t NUM_THREADS;
int lock_used;
struct timespec start, time_end;
pthread_barrier_t bar;

/* Extern Variables */
struct node* actual_root;
pthread_mutex_t tree_lock;
struct rw_BST_node* actual_root_rw;
pthread_rwlock_t tree_lock_rw;

enum Lock_Type {
	FINE_GRAINED,
	READER_WRITER
};

void global_init()
{
	if(lock_used == FINE_GRAINED)
	{
		actual_root = NULL;
		pthread_mutex_init(&tree_lock, NULL);
	}
	else
	{
		actual_root_rw = NULL;
		pthread_rwlock_init(&tree_lock_rw, NULL);
	}

	pthread_barrier_init(&bar, NULL, NUM_THREADS);
	threads = (pthread_t*)malloc(NUM_THREADS*sizeof(pthread_t));
	args = (size_t*)malloc(NUM_THREADS*sizeof(size_t));
}

void global_cleanup()
{
	if(lock_used == FINE_GRAINED)
	{
		pthread_mutex_destroy(&tree_lock);
	}
	else
	{
		pthread_rwlock_destroy(&tree_lock_rw);	
	}

	pthread_barrier_destroy(&bar);
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

	if(tid==1)
	{
		clock_gettime(CLOCK_MONOTONIC,&start);
	}

	pthread_barrier_wait(&bar);

	if(tid <= NUM_THREADS/3)
	{
		//srandom(5);	
		srandom(((time(0) & 0xFFFFFFF0) | (tid+1)));
		//rand_key = random()%100;
		//printf("TID: %ld; Random Num: %ld\n", tid, (random()%100));

		if(lock_used == FINE_GRAINED)
			put(actual_root, NULL, (random()%1000), tid);
		else
			put_rw(actual_root_rw, NULL, (random()%1000), tid);
	}
	else if((tid > NUM_THREADS/3) && (tid <= ((NUM_THREADS/3)*2)))
	{
		//srandom(5);
		srandom(((time(0) & 0xFFFFFFF01) | ((tid-NUM_THREADS/3)+1)));
		//printf("TID: %ld; Random Num: %ld\n", tid, (random()%100));
		if(lock_used == FINE_GRAINED)
			get(actual_root, NULL, (random()%1000), tid);
		else
			get_rw(actual_root_rw, NULL, (random()%1000), tid);
	}
	else
	{
		do
		{
			lo_key = random()%1000;
			hi_key = random()%1000;
		}while(lo_key > hi_key);
		printf("\nRange Queries btw %d and %d for TID %ld\n", lo_key, hi_key, tid);

		if(lock_used == FINE_GRAINED)
			range_queries(actual_root, NULL, lo_key, hi_key, tid);
		else
			range_queries_rw(actual_root_rw, NULL, lo_key, hi_key, tid);
	}

	/*if(tid <= NUM_THREADS/2)
	{
		if(lock_used == FINE_GRAINED)
		{
			for(int i = 0; i<5; i++)
				put(actual_root, NULL, (random()%100000), tid);
		}
		else
		{
			for(int i = 0; i<5000; i++)
				put_rw(actual_root_rw, NULL, (random()%100000), tid);
		}
	}
	else
	{
		if(lock_used == FINE_GRAINED)
		{
			for(int i = 0; i<5; i++)
				put(actual_root, NULL, (random()%100000), tid);
		}
		else
		{
			for(int i = 0; i<5000; i++)
				get_rw(actual_root_rw, NULL, (random()%100000), tid);
		}	
	}*/

	pthread_barrier_wait(&bar);

	if(tid==1)
	{
		clock_gettime(CLOCK_MONOTONIC,&time_end);
	}
}

int main(int argc, char **argv)
{
	int data;
	int c;
	int option_index = 0;

	/* Default values; if user does not specify */
	NUM_THREADS = 6;
	lock_used = FINE_GRAINED;

	static struct option long_options[] = {
		{"h", no_argument, 0, 'h'},      //Optional arg - print name
		{"o",    required_argument, 0, 'o'},//Optional arg - output sorted list to file
		{"t",    required_argument, 0, 't'},//Optional arg - specify number of threads
		{"i",    required_argument, 0, 'i'},//Optional arg - No. of iterations
		{"bar",  required_argument, 0, 'b'},//Optional arg - Barrier to use
		{"lock", required_argument, 0, 'l'},//Optional arg - lock to use
		{0     , 0          , 0,  0 }
	};

	/* Loop to get all otpional arguments */
	while((c = getopt_long(argc, argv, "ho:t:i:b:l:", long_options, &option_index)) != -1)
	{
		/* Handle Optional Arguments */		
		switch(c)
		{
			case 'h': printf("Devansh Mittal\n"); //print execution instructions
					  return 0; 
					  break;

			//case 'o': output_filename = optarg;
			//		  break;

			case 't': NUM_THREADS = atoi(optarg);
					  break;

			/*case 'i': NUM_ITERATIONS = atoi(optarg);
					  break;

			case 'b': barr = optarg;
					  break;*/

			case 'l': if(!strcmp(optarg, "fg"))
					  	lock_used = FINE_GRAINED;
					  else
					  	lock_used = READER_WRITER;

					  break;

			default: printf("No optional arguments\n");
		}
	}

	global_init();

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

	if(lock_used == FINE_GRAINED)
		inorder(actual_root);
	else
		inorder_rw(actual_root_rw);

	global_cleanup();

	unsigned long long elapsed_ns;
	elapsed_ns = (time_end.tv_sec-start.tv_sec)*1000000000 + (time_end.tv_nsec-start.tv_nsec);
	printf("\nElapsed (ns): %llu\n",elapsed_ns);
	double elapsed_s = ((double)elapsed_ns)/1000000000.0;
	printf("Elapsed (s): %lf\n",elapsed_s);

	return 0;
}