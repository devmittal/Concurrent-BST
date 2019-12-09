/*****************************************************************************
​ ​* ​ ​ @file​ ​  		main.c
​ * ​ ​ @brief​ ​ 		Spawn threads, call put, get and range functions based on
 *					thread id
​ * ​ ​ @Author(s)​  	​​Devansh Mittal
​ * ​ ​ @Date​ ​​ 		December 6th, 2019
​ * ​ ​ @version​ ​ 		1.0
*****************************************************************************/

#include <time.h>
#include "fine_grained_lock_BST.h"
#include "rw_lock_BST.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

pthread_t* threads;
size_t* args;
size_t NUM_THREADS;
size_t NUM_ITERATIONS;
int lock_used;
struct timespec start, time_end;
pthread_barrier_t bar;
int key_limit;

#define HIGH_CONTENTION	5
#define LOW_CONTENTION	1000000
#define MAX_KEY 999999
#define MIN_KEY 0

/* Extern Variables */
struct node* actual_root;
pthread_mutex_t tree_lock;
struct rw_BST_node* actual_root_rw;
pthread_rwlock_t tree_lock_rw;
atomic<int> duplicate_count; //Number of duplicate keys during put
atomic<int> get_success_count; //Number of successful get operations
atomic<int> get_fail_count; //Number of failed get operations
pthread_mutex_t duplicate_count_lock;
pthread_mutex_t get_success_count_lock;
uint8_t isPrint;

enum Lock_Type {
	FINE_GRAINED,
	READER_WRITER
};

/*
 * @func_name - global_init
 * @Description - Initialize global variables
 * @params - void
 * @return - void
 */
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

	pthread_mutex_init(&duplicate_count_lock, NULL);
	pthread_mutex_init(&get_success_count_lock, NULL);
	pthread_barrier_init(&bar, NULL, NUM_THREADS);
	threads = (pthread_t*)malloc(NUM_THREADS*sizeof(pthread_t));
	args = (size_t*)malloc(NUM_THREADS*sizeof(size_t));
}

/*
 * @func_name - global_cleanup
 * @Description - deinitialize global variables
 * @params - void
 * @return - void
 */
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

	pthread_mutex_destroy(&duplicate_count_lock);
	pthread_mutex_destroy(&get_success_count_lock);
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
	int key;

	if(tid==1)
	{
		if(key_limit == HIGH_CONTENTION) //If high contention case selected, pre-construct skewed tree 
		{
			for(int j = (NUM_ITERATIONS*(NUM_THREADS/3)); j>4; j=j-2)
			{
				if(lock_used == FINE_GRAINED)
					put(actual_root, NULL, j, j+1);
				else
					put_rw(actual_root_rw, NULL, j, j+1);
			}

			for(int j = (NUM_ITERATIONS*(NUM_THREADS/3))-1; j>4; j=j-2)
			{
				if(lock_used == FINE_GRAINED)
					put(actual_root, NULL, j, j+1);
				else
					put_rw(actual_root_rw, NULL, j, j+1);
			}
		}
		clock_gettime(CLOCK_MONOTONIC,&start);
	}

	pthread_barrier_wait(&bar);

	if(tid <= NUM_THREADS/3) //Insert operations
	{
		for(int i = 0; i<NUM_ITERATIONS; i++)
		{
			srandom(( (tid) | (i*3))); 
			key = random()%key_limit;
			
			if(lock_used == FINE_GRAINED)
				put(actual_root, NULL, key, key+1);
			else
				put_rw(actual_root_rw, NULL, key, key+1);
		}
	}
	else if((tid > NUM_THREADS/3) && (tid <= ((NUM_THREADS/3)*2))) //Search operation
	{
		for(int i = 0; i<NUM_ITERATIONS; i++)
		{
			srandom(( (tid-(NUM_THREADS/3)) | (i*3)));
			key = (random()%key_limit);	

			if(lock_used == FINE_GRAINED)	
				get(actual_root, NULL, key, tid, key+1);
			else
				get_rw(actual_root_rw, NULL, key, tid, key+1);
		}
	}
	else //Range query operations
	{
		do
		{
			lo_key = random()%key_limit;
			hi_key = random()%key_limit;
		}while(lo_key > hi_key);

		isPrint ? printf("\nRange Queries btw %d and %d for TID %ld\n", lo_key, hi_key, tid) : 0;

		if(lock_used == FINE_GRAINED)
			range_queries(actual_root, NULL, lo_key, hi_key, tid);
		else
			range_queries_rw(actual_root_rw, NULL, lo_key, hi_key, tid);
	}

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
	duplicate_count.store(0);
	get_success_count.store(0);
	get_fail_count.store(0);
	int node_count;
	int expected_node_count, range_query_count;

	/* Default values; if user does not specify */
	NUM_THREADS = 12;
	lock_used = FINE_GRAINED;
	key_limit = LOW_CONTENTION;
	NUM_ITERATIONS = 1;
	isPrint = 1;

	static struct option long_options[] = {
		{"h", no_argument, 0, 'h'},      //Optional arg - print execution instructions
		{"t",    required_argument, 0, 't'},//Optional arg - specify number of threads
		{"test",  required_argument, 0, 'e'},//Optional arg - test to use
		{"lock", required_argument, 0, 'l'},//Optional arg - lock to use
		{"i",	required_argument,	0,	'i'},//Number of iterations
		{"print", required_argument, 0,	'p'}, //Whether to print or no
		{0     , 0          , 0,  0 }
	};

	/* Loop to get all otpional arguments */
	while((c = getopt_long(argc, argv, "ht:e:l:i:p:", long_options, &option_index)) != -1)
	{
		/* Handle Optional Arguments */		
		switch(c)
		{
			case 'h': help(); //print execution instructions
					  return 0; 
					  break;

			case 'e': if(!strcmp(optarg, "high"))
					  	key_limit = HIGH_CONTENTION;
					  else
					  	key_limit = LOW_CONTENTION;
					  break;

			case 't': NUM_THREADS = atoi(optarg);

					  if(NUM_THREADS < 3)
					  	NUM_THREADS = 3;
					  break;

			case 'l': if(!strcmp(optarg, "fg"))
					  	lock_used = FINE_GRAINED;
					  else
					  	lock_used = READER_WRITER;
					  break;

			case 'i': NUM_ITERATIONS = atoi(optarg);
					  break;

			case 'p': if(!strcmp(optarg, "no"))
					  	isPrint = 0;
					  else
					  	isPrint = 1;
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

	isPrint ? printf("\nInorder traversal\n") : 0;
	if(lock_used == FINE_GRAINED)
	{
		/* Range query between min and max key to verify range query function */ 
		range_query_count = range_queries(actual_root, NULL, MIN_KEY, MAX_KEY, 0);
		/* Determine number of nodes in tree */
		node_count = inorder(actual_root);
	}
	else
	{
		/* Range query between min and max key to verify range query function */
		range_query_count = range_queries_rw(actual_root_rw, NULL, MIN_KEY, MAX_KEY, 0);
		/* Determine number of nodes in tree */
		node_count = inorder_rw(actual_root_rw);
	}

	if(key_limit == HIGH_CONTENTION)
	{
		/* This is expected count of nodes to be present in the tree after accounting for the pre-constructed skewed tree */
		expected_node_count = (((NUM_THREADS/3)*NUM_ITERATIONS)-duplicate_count.load()) + (((NUM_THREADS/3)*NUM_ITERATIONS) - 4);	
	}
	else
	{	
		/* This is expected count of nodes to be present in the tree */
		expected_node_count = (((NUM_THREADS/3)*NUM_ITERATIONS)-duplicate_count.load()); 
	}

	printf("\nTotal number of put operations: %ld\n", ((NUM_THREADS/3)*NUM_ITERATIONS));	
	printf("Total number of duplicates: %d \n", duplicate_count.load());
	printf("Expected number of unique nodes in trees: %d\n", expected_node_count);		
	printf("Counting current number of nodes in tree ... %d \n", node_count);
	
	/* If the expected number of nodes matches the nodes return by inroder traversal, put success */
	if(node_count == expected_node_count)
	{
		printf("Put operation pass!\n");
	}
	else
	{
		printf("Put operation fail\n");
	}

	printf("\nTotal number of get operations: %ld\n", ((NUM_THREADS/3)*NUM_ITERATIONS));
	printf("Total number of successful get operations: %d\n", get_success_count.load());
	printf("Total number of unsuccessful get operations: %d\n", get_fail_count.load());

	/* If all gets were successful, get operation pass */
	if(((NUM_THREADS/3)*NUM_ITERATIONS) == get_success_count.load())
	{
		printf("Get operation pass!\n");
	}
	else
	{
		printf("Get operation fail!\n");
	}

	printf("\nRange query test between %d (lowest key possible) and %d (highest key possible)\n", MIN_KEY, MAX_KEY);
	printf("Number of nodes returned by range query: %d\n", range_query_count);
	printf("Number expected: %d\n", node_count);

	if(node_count == range_query_count)
		printf("Range query operation pass!\n");
	else
		printf("Range query operation fail!\n");

	global_cleanup();

	unsigned long long elapsed_ns;
	elapsed_ns = (time_end.tv_sec-start.tv_sec)*1000000000 + (time_end.tv_nsec-start.tv_nsec);
	printf("\nElapsed (ns): %llu\n",elapsed_ns);
	double elapsed_s = ((double)elapsed_ns)/1000000000.0;
	printf("Elapsed (s): %lf\n",elapsed_s);

	return 0;
}