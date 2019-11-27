#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

pthread_mutex_t tree_lock;
pthread_t* threads;
size_t* args;
size_t NUM_THREADS;

struct node {
	int key;	//BST sorted by key
	int data;	//associated data
	struct node* left;
	struct node* right;
	pthread_mutex_t lock;
};

struct node* actual_root;

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

struct node* CreateNewNode(int key, int value)
{
	struct node* NewNode = (struct node*)malloc(sizeof(struct node));
	NewNode->key = key;
	NewNode->data = value;
	NewNode->left = NewNode->right = NULL;
	pthread_mutex_init(&NewNode->lock,NULL);	
	return NewNode;
}

void put(struct node* root, struct node* parent, int key, int value)
{
	if(parent == NULL)
	{
		//printf("TID %d locking tree lock in put\n", value);
		pthread_mutex_lock(&tree_lock);
		if(actual_root == NULL)
		{
			actual_root = CreateNewNode(key,value);
			//printf("TID %d unlocking tree lock in put\n", value);
			printf("\nInserted root key %d\n", actual_root->key);
			pthread_mutex_unlock(&tree_lock);
			return;
		}

		//printf("TID %d locking root %d in put\n", value, actual_root->key);
		pthread_mutex_lock(&actual_root->lock);
		root = actual_root;
		//printf("TID %d unlocking tree lock in put\n", value);
		pthread_mutex_unlock(&tree_lock);
	}

	if(key < root->key)
	{
		if(root->left == NULL)
		{
			root->left = CreateNewNode(key, value);
			//printf("TID %d unlocking root %d in put\n", value, root->key);
			printf("\nInserted key %d\n", root->left->key);
			pthread_mutex_unlock(&root->lock);
		}
		else
		{
			//printf("TID %d locking root %d in put\n", value, root->left->key);
			pthread_mutex_lock(&root->left->lock);
			//printf("TID %d unlocking root %d in put\n", value, root->key);
			pthread_mutex_unlock(&root->lock);
			put(root->left, root, key, value);	
		}
	}
	else if(key > root->key)
	{
		if(root->right == NULL)
		{
			root->right = CreateNewNode(key, value);
			//printf("TID %d unlocking root %d in put\n", value, root->key);
			printf("\nInserted key %d\n", root->right->key);
			pthread_mutex_unlock(&root->lock);
		}
		else
		{
			//printf("TID %d locking root %d in put\n", value, root->right->key);
			pthread_mutex_lock(&root->right->lock);
			//printf("TID %d unlocking root %d in put\n", value, root->key);
			pthread_mutex_unlock(&root->lock);
			put(root->right, root, key, value);	
		}
	}
	else
	{
		root->data = value; //In case of same key, update data
		printf("\nUpdated key %d with value %d\n", root->key, root->data);
		//printf("TID %d unlocking root %d in put\n", value, root->key);
		pthread_mutex_unlock(&root->lock);
	}
}

void get(struct node* root, struct node* parent, int key, int tid)
{
	if(parent == NULL)
	{
		//printf("TID %d locking tree lock in get\n", tid);
		pthread_mutex_lock(&tree_lock);
		if(actual_root == NULL)
		{
			printf("Tree not populated yet\n");
			//printf("TID %d unlocking tree lock in get\n", tid);
			pthread_mutex_unlock(&tree_lock);
			return;
		}

		//printf("TID %d locking root %d in get\n", tid, actual_root->key);
		pthread_mutex_lock(&actual_root->lock);
		root = actual_root;
		//printf("TID %d unlocking tree lock in get\n", tid);
		pthread_mutex_unlock(&tree_lock);
	}

	if(key < root->key)
	{
		if(root->left == NULL)
		{
			pthread_mutex_unlock(&root->lock);
			printf("Node does not exist. TID %d\n", tid);
			return;
		}
		else
		{
			//printf("TID %d locking root %d in get\n", tid, root->left->key);
			pthread_mutex_lock(&root->left->lock);
			//printf("TID %d unlocking root %d in get\n", tid, root->key);
			pthread_mutex_unlock(&root->lock);
			get(root->left, root, key, tid);	
		}
	}
	else if(key > root->key)
	{
		if(root->right == NULL)
		{
			pthread_mutex_unlock(&root->lock);
			printf("Node does not exist. TID %d\n", tid);
			return;
		}
		else
		{
			//printf("TID %d locking root %d in get\n", tid, root->right->key);
			pthread_mutex_lock(&root->right->lock);
			//printf("TID %d unlocking root %d in get\n", tid, root->key);
			pthread_mutex_unlock(&root->lock);
			get(root->right, root, key, tid);	
		}
	}
	else
	{
		printf("Key->%d; Value->%d\n", root->key, root->data); 
		//printf("TID %d unlocking root %d in get\n", tid, root->key);
		pthread_mutex_unlock(&root->lock);
	}
}

void range_queries(struct node* root, struct node* parent, int lo, int hi)
{
	/*if(parent == NULL)
	{
		//printf("TID %d locking tree lock in get\n", tid);
		pthread_mutex_lock(&tree_lock);
		if(actual_root == NULL)
		{
			printf("Tree not populated yet\n");
			//printf("TID %d unlocking tree lock in get\n", tid);
			pthread_mutex_unlock(&tree_lock);
			return;
		}

		//printf("TID %d locking root %d in get\n", tid, actual_root->key);
		pthread_mutex_lock(&actual_root->lock);
		root = actual_root;
		//printf("TID %d unlocking tree lock in get\n", tid);
		pthread_mutex_unlock(&tree_lock);
	}*/

	if(actual_root == NULL || (lo > hi))
	{
		printf("Invalid parameters\n");
		return;
	}

	//pthread_mutex_lock(&root->lock);

	if(root == NULL)
	{
		//pthread_mutex_unlock(&root->lock);
		return;
	}

	if(lo < root->key)
	{
		//pthread_mutex_lock(&root->left->lock);
		//pthread_mutex_unlock(&root->lock);
		range_queries(root->left, root, lo, hi);
	}

	if(lo <= root->key && hi >= root->key)
		printf("Range Queries -> Key->%d; Value->%d\n", root->key, root->data);

	if(hi > root->key)
	{
		//pthread_mutex_lock(&root->right->lock);
		//pthread_mutex_unlock(&root->lock);
		range_queries(root->right, root, lo, hi);
	}

	//pthread_mutex_unlock(&root->lock);
}

void inorder(struct node* root)
{
	if(root == NULL)
		return;

	inorder(root->left);
	printf("Key->%d\n", root->key);
	inorder(root->right);
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
	NUM_THREADS = 12;

	global_init();

	put(actual_root, NULL, 5, 1);
	put(actual_root, NULL, 4, 2);
	put(actual_root, NULL, 3, 3);
	put(actual_root, NULL, 2, 4);
	put(actual_root, NULL, 1, 5);

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