/*****************************************************************************
​ ​* ​ ​ @file​ ​  		rw_lock_BST.h
​ * ​ ​ @brief​ ​ 		Node structure and function declaration of corresponding
 *					C file.
​ * ​ ​ @Author(s)​  	​​Devansh Mittal
​ * ​ ​ @Date​ ​​ 		December 6th, 2019
​ * ​ ​ @version​ ​ 		1.0
*****************************************************************************/

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

extern struct rw_BST_node* actual_root_rw; //main root of tree
extern pthread_rwlock_t tree_lock_rw; //tree lock

/* Function Declaration */
struct rw_BST_node* CreateNewNode_rw(int key, int value);
void get_rw(struct rw_BST_node* root, struct rw_BST_node* parent, int key, int tid, int actual_value);
void put_rw(struct rw_BST_node* root, struct rw_BST_node* parent, int key, int value);
int range_queries_rw(struct rw_BST_node* root, struct rw_BST_node* parent, int lo, int hi, int tid);
int inorder_rw(struct rw_BST_node* root);

#endif /* RW_LOCK_BST_H */