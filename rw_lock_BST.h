#ifndef RW_LOCK_H
#define RW_LOCK_H

#include <pthread.h>

struct rw_BST_node {
	int key;	//BST sorted by key
	int data;	//associated data
	struct rw_BST_node* left;
	struct rw_BST_node* right;
	pthread_rwlock_t lock;
};

extern struct rw_BST_node* actual_root_rw;
extern pthread_rwlock_t tree_lock_rw;

/* Function Declaration */
struct rw_BST_node* CreateNewNode_rw(int key, int value);
void get_rw(struct rw_BST_node* root, struct rw_BST_node* parent, int key, int tid);
void put_rw(struct rw_BST_node* root, struct rw_BST_node* parent, int key, int value);
void range_queries_rw(struct rw_BST_node* root, struct rw_BST_node* parent, int lo, int hi, int tid);
void inorder_rw(struct rw_BST_node* root);

#endif /* RW_LOCK_BST_H */