/*****************************************************************************
​ ​* ​ ​ @file​ ​  		rw_lock_BST.c
​ * ​ ​ @brief​ ​ 		Put, get, range query functions using hand-over-hand locking
 *					and reader-writer locks
​ * ​ ​ @Author(s)​  	​​Devansh Mittal
​ * ​ ​ @Date​ ​​ 		December 6th, 2019
​ * ​ ​ @version​ ​ 		1.0
*****************************************************************************/

#include "rw_lock_BST.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * @func_name - CreateNewNode_rw
 * @Description - Create new node and assign and key and value
 * @params - key - key to be assigned to node
 *			 value - corresponding value to be assigned to node
 * @return - allocated node
 */
struct rw_BST_node* CreateNewNode_rw(int key, int value)
{
	struct rw_BST_node* NewNode = (struct rw_BST_node*)malloc(sizeof(struct rw_BST_node));
	NewNode->key = key;
	NewNode->data = value;
	NewNode->left = NewNode->right = NULL;
	pthread_rwlock_init(&NewNode->lock,NULL);	
	return NewNode;
}

/*
 * @func_name - put_rw
 * @Description - Insert node to tree
 * @params - root - root node
 *			 parent - parent node
 *			 key - key value to be inserted
 *			 value - corresponding value
 * @return - void
 */
void put_rw(struct rw_BST_node* root, struct rw_BST_node* parent, int key, int value)
{
	if(parent == NULL)
	{
		pthread_rwlock_wrlock(&tree_lock_rw);
		if(actual_root_rw == NULL) //Condition when tree is empty
		{
			actual_root_rw = CreateNewNode_rw(key,value);
			//printf("\nInserted root key %d\n", actual_root_rw->key);
			pthread_rwlock_unlock(&tree_lock_rw);
			return;
		}

		pthread_rwlock_wrlock(&actual_root_rw->lock);
		root = actual_root_rw;
		pthread_rwlock_unlock(&tree_lock_rw);
	}

	if(key < root->key)
	{
		if(root->left == NULL)
		{
			root->left = CreateNewNode_rw(key, value);
			//printf("\nInserted key %d\n", root->left->key);
			pthread_rwlock_unlock(&root->lock);
		}
		else
		{
			pthread_rwlock_wrlock(&root->left->lock);
			pthread_rwlock_unlock(&root->lock);
			put_rw(root->left, root, key, value);	
		}
	}
	else if(key > root->key)
	{
		if(root->right == NULL)
		{
			root->right = CreateNewNode_rw(key, value);
			//printf("\nInserted key %d\n", root->right->key);
			pthread_rwlock_unlock(&root->lock);
		}
		else
		{
			pthread_rwlock_wrlock(&root->right->lock);
			pthread_rwlock_unlock(&root->lock);
			put_rw(root->right, root, key, value);	
		}
	}
	else
	{
		root->data = value; //In case of same key, update data
		duplicate_count++;
		//printf("\nUpdated key %d with value %d\n", root->key, root->data);
		pthread_rwlock_unlock(&root->lock);
	}
}

/*
 * @func_name - get_rw
 * @Description - search node in tree based on key
 * @params - root - root node
 *			 parent - parent node
 *			 key - key value to be searched
 *			 tid - TID of thread which called the function
 * @return - void
 */
void get_rw(struct rw_BST_node* root, struct rw_BST_node* parent, int key, int tid, int actual_value)
{
	if(parent == NULL)
	{
		pthread_rwlock_rdlock(&tree_lock_rw);
		if(actual_root_rw == NULL) //Condition when tree is empty
		{
			isPrint ? printf("Get -> Tree not populated yet. TID %d\n", tid) : 0;
			get_success_count++;
			pthread_rwlock_unlock(&tree_lock_rw);
			return;
		}

		pthread_rwlock_rdlock(&actual_root_rw->lock);
		root = actual_root_rw;
		pthread_rwlock_unlock(&tree_lock_rw);
	}

	if(key < root->key)
	{
		if(root->left == NULL)
		{
			get_success_count++;
			isPrint ? printf("Get -> Node does not exist. TID %d\n", tid) : 0;
			pthread_rwlock_unlock(&root->lock);
			return;
		}
		else
		{
			pthread_rwlock_rdlock(&root->left->lock);
			pthread_rwlock_unlock(&root->lock);
			get_rw(root->left, root, key, tid, actual_value);	
		}
	}
	else if(key > root->key)
	{
		if(root->right == NULL)
		{
			get_success_count++;
			isPrint ? printf("Get -> Node does not exist. TID %d\n", tid) : 0;
			pthread_rwlock_unlock(&root->lock);
			return;
		}
		else
		{
			pthread_rwlock_rdlock(&root->right->lock);
			pthread_rwlock_unlock(&root->lock);
			get_rw(root->right, root, key, tid, actual_value);	
		}
	}
	else
	{
		if(root->data == actual_value)
			get_success_count++;
		else
			get_fail_count++;
		isPrint ? printf("Get -> Key->%d; Value->%d\n", root->key, root->data) : 0; 
		pthread_rwlock_unlock(&root->lock);
	}
}

/*
 * @func_name - range_queries_rw
 * @Description - search nodes in tree between low and high key
 * @params - root - root node
 *			 parent - parent node
 *			 lo - lower bound key value to be searched
 *			 hi - higher bound key value to be searched
 *			 tid - TID of thread which called the function
 * @return - void
 */
int range_queries_rw(struct rw_BST_node* root, struct rw_BST_node* parent, int lo, int hi, int tid)
{
	static int count = 0;

	if(parent == NULL)
	{
		pthread_rwlock_rdlock(&tree_lock_rw);
		if(actual_root_rw == NULL) //Condition when tree is empty
		{
			isPrint ? printf("Range Query for TID %d -> Tree not populated yet\n", tid) : 0;
			pthread_rwlock_unlock(&tree_lock_rw);
			return count;
		}

		pthread_rwlock_rdlock(&actual_root_rw->lock);
		root = actual_root_rw;
		pthread_rwlock_unlock(&tree_lock_rw);
	}

	if(lo < root->key)
	{
		if(root->left != NULL)
		{
			pthread_rwlock_rdlock(&root->left->lock);
			pthread_rwlock_unlock(&root->lock);
			range_queries_rw(root->left, root, lo, hi, tid);
			pthread_rwlock_rdlock(&root->lock); //lock the node again after returning from function
		}
	}

	if(lo <= root->key && hi >= root->key)
	{
		if(tid == 0)
			count++;
		else
		{
			isPrint ? printf("Range Queries for TID %d -> Key->%d; Value->%d\n", tid, root->key, root->data) : 0;
		}
	}

	if(hi > root->key)
	{
		if(root->right != NULL)
		{
			pthread_rwlock_rdlock(&root->right->lock);
			pthread_rwlock_unlock(&root->lock);
			range_queries_rw(root->right, root, lo, hi, tid);
			pthread_rwlock_rdlock(&root->lock); //lock the node again after returning from function
		}
	}

	pthread_rwlock_unlock(&root->lock);

	return count;
}

/*
 * @func_name - inorder_rw
 * @Description - inorder traversal of tree
 * @params - root - root node
 * @return - int - number of nodes in tree
 */
int inorder_rw(struct rw_BST_node* root)
{
	static int count = 0;
	if(root == NULL)
		return 0;

	inorder_rw(root->left);
	count++;
	isPrint ? printf("Key->%d; Value->%d; count: %d\n", root->key, root->data, count) : 0;
	inorder_rw(root->right);

	return count;
}