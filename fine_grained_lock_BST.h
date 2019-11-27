#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

struct node {
	int key;	//BST sorted by key
	int data;	//associated data
	struct node* left;
	struct node* right;
	pthread_mutex_t lock;
};

extern struct node* actual_root;
extern pthread_mutex_t tree_lock;

/* Function Declarations */
struct node* CreateNewNode(int key, int value);
void put(struct node* root, struct node* parent, int key, int value);
void get(struct node* root, struct node* parent, int key, int tid);
void range_queries(struct node* root, struct node* parent, int lo, int hi);
void inorder(struct node* root);