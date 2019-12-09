/*****************************************************************************
​ ​* ​ ​ @file​ ​  		fine_grained_lock_BST.c
​ * ​ ​ @brief​ ​ 		Put, get, range query functions using hand-over-hand locking
 *					and regular pthread locks
​ * ​ ​ @Author(s)​  	​​Devansh Mittal
​ * ​ ​ @Date​ ​​ 		December 6th, 2019
​ * ​ ​ @version​ ​ 		1.0
*****************************************************************************/

#include "fine_grained_lock_BST.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>

/*
 * @func_name - CreateNewNode
 * @Description - Create new node and assign and key and value
 * @params - key - key to be assigned to node
 *			 value - corresponding value to be assigned to node
 * @return - allocated node
 */
struct node* CreateNewNode(int key, int value)
{
	struct node* NewNode = (struct node*)malloc(sizeof(struct node));
	NewNode->key = key;
	NewNode->data = value;
	NewNode->left = NewNode->right = NULL;
	pthread_mutex_init(&NewNode->lock,NULL);	
	return NewNode;
}

/*
 * @func_name - put
 * @Description - Insert node to tree
 * @params - root - root node
 *			 parent - parent node
 *			 key - key value to be inserted
 *			 value - corresponding value
 * @return - void
 */
void put(struct node* root, struct node* parent, int key, int value)
{
	if(parent == NULL)
	{
		pthread_mutex_lock(&tree_lock);
		if(actual_root == NULL) //Condition when tree is empty
		{
			actual_root = CreateNewNode(key,value);
			pthread_mutex_unlock(&tree_lock);
			return;
		}

		pthread_mutex_lock(&actual_root->lock);
		root = actual_root;
		pthread_mutex_unlock(&tree_lock);
	}

	if(key < root->key)
	{
		if(root->left == NULL)
		{
			root->left = CreateNewNode(key, value);
			pthread_mutex_unlock(&root->lock);
		}
		else
		{
			pthread_mutex_lock(&root->left->lock);
			pthread_mutex_unlock(&root->lock);
			put(root->left, root, key, value);	
		}
	}
	else if(key > root->key)
	{
		if(root->right == NULL)
		{
			root->right = CreateNewNode(key, value);
			pthread_mutex_unlock(&root->lock);
		}
		else
		{
			pthread_mutex_lock(&root->right->lock);
			pthread_mutex_unlock(&root->lock);
			put(root->right, root, key, value);	
		}
	}
	else
	{
		root->data = value; //In case of same key, update data
		duplicate_count++;
		pthread_mutex_unlock(&root->lock);
	}
}

/*
 * @func_name - get
 * @Description - search node in tree based on key
 * @params - root - root node
 *			 parent - parent node
 *			 key - key value to be searched
 *			 tid - TID of thread which called the function
 * @return - void
 */
void get(struct node* root, struct node* parent, int key, int tid, int actual_value)
{
	if(parent == NULL)
	{
		pthread_mutex_lock(&tree_lock);
		if(actual_root == NULL) //Condition when tree is empty
		{
			isPrint ? printf("Get -> Tree not populated yet\n") : 0;
			get_success_count++;
			pthread_mutex_unlock(&tree_lock);
			return;
		}

		pthread_mutex_lock(&actual_root->lock);
		root = actual_root;
		pthread_mutex_unlock(&tree_lock);
	}

	if(key < root->key)
	{
		if(root->left == NULL)
		{
			isPrint ? printf("Get -> Node does not exist. TID %d\n", tid) : 0;
			get_success_count++;
			pthread_mutex_unlock(&root->lock);
			return;
		}
		else
		{
			pthread_mutex_lock(&root->left->lock);
			pthread_mutex_unlock(&root->lock);
			get(root->left, root, key, tid, actual_value);	
		}
	}
	else if(key > root->key)
	{
		if(root->right == NULL)
		{
			isPrint ? printf("Get -> Node does not exist. TID %d\n", tid) : 0;
			get_success_count++;
			pthread_mutex_unlock(&root->lock);
			return;
		}
		else
		{
			pthread_mutex_lock(&root->right->lock);
			pthread_mutex_unlock(&root->lock);
			get(root->right, root, key, tid, actual_value);	
		}
	}
	else
	{
		isPrint ? printf("Get -> Key->%d; Value->%d\n", root->key, root->data) : 0; 
		if(root->data == actual_value)
		{
			get_success_count++;
		}
		else
			get_fail_count++;
		pthread_mutex_unlock(&root->lock);
	}
}

/*
 * @func_name - range_queries
 * @Description - search nodes in tree between low and high key
 * @params - root - root node
 *			 parent - parent node
 *			 lo - lower bound key value to be searched
 *			 hi - higher bound key value to be searched
 *			 tid - TID of thread which called the function
 * @return - void
 */
int range_queries(struct node* root, struct node* parent, int lo, int hi, int tid)
{
	static int count = 0;
	if(parent == NULL)
	{
		pthread_mutex_lock(&tree_lock);
		if(actual_root == NULL) //Condition when tree is empty
		{
			isPrint ? printf("Range Query -> Tree not populated yet\n"): 0;
			pthread_mutex_unlock(&tree_lock);
			return count;
		}

		pthread_mutex_lock(&actual_root->lock);
		root = actual_root;
		pthread_mutex_unlock(&tree_lock);
	}

	if(lo < root->key)
	{
		if(root->left != NULL)
		{
			pthread_mutex_lock(&root->left->lock);
			pthread_mutex_unlock(&root->lock);
			range_queries(root->left, root, lo, hi, tid);
			pthread_mutex_lock(&root->lock);
		}
	}

	if(lo <= root->key && hi >= root->key)
	{
		if(!tid)
		{
			count++;
		}
		else
		{
			isPrint ? printf("Range Queries for TID %d -> Key->%d; Value->%d\n", tid, root->key, root->data) : 0;
		}
	}

	if(hi > root->key)
	{
		if(root->right != NULL)
		{
			pthread_mutex_lock(&root->right->lock);
			pthread_mutex_unlock(&root->lock);
			range_queries(root->right, root, lo, hi, tid);
			pthread_mutex_lock(&root->lock);
		}
	}

	pthread_mutex_unlock(&root->lock);

	return count;
}

/*
 * @func_name - inorder_rw
 * @Description - inorder traversal of tree
 * @params - root - root node
 * @return - int - Count of nodes in tree
 */
int inorder(struct node* root)
{
	static int count = 0;
	if(root == NULL)
		return 0;

	inorder(root->left);
	count++;
	isPrint ? printf("Key->%d; Value->%d; count: %d\n", root->key, root->data, count) : 0;
	inorder(root->right);

	return count;
}