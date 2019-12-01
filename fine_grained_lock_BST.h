#ifndef FINE_GRAINED_LOCK_BST_H
#define FINE_GRAINED_LOCK_BST_H

#include <pthread.h>

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
void range_queries(struct node* root, struct node* parent, int lo, int hi, int tid);
void inorder(struct node* root);

#endif /* FINE_GRAINED_LOCK_BST_H */