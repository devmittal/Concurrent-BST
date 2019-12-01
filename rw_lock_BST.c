#include "rw_lock_BST.h"
#include <stdlib.h>
#include <stdio.h>

struct rw_BST_node* CreateNewNode_rw(int key, int value)
{
	struct rw_BST_node* NewNode = (struct rw_BST_node*)malloc(sizeof(struct rw_BST_node));
	NewNode->key = key;
	NewNode->data = value;
	NewNode->left = NewNode->right = NULL;
	pthread_rwlock_init(&NewNode->lock,NULL);	
	return NewNode;
}

void put_rw(struct rw_BST_node* root, struct rw_BST_node* parent, int key, int value)
{
	if(parent == NULL)
	{
		//printf("TID %d locking tree lock in put_rw\n", value);
		pthread_rwlock_wrlock(&tree_lock_rw);
		if(actual_root_rw == NULL)
		{
			actual_root_rw = CreateNewNode_rw(key,value);
			//printf("TID %d unlocking tree lock in put_rw\n", value);
			printf("\nInserted root key %d\n", actual_root_rw->key);
			pthread_rwlock_unlock(&tree_lock_rw);
			return;
		}

		//printf("TID %d locking root %d in put_rw\n", value, actual_root_rw->key);
		pthread_rwlock_wrlock(&actual_root_rw->lock);
		root = actual_root_rw;
		//printf("TID %d unlocking tree lock in put_rw\n", value);
		pthread_rwlock_unlock(&tree_lock_rw);
	}

	if(key < root->key)
	{
		if(root->left == NULL)
		{
			root->left = CreateNewNode_rw(key, value);
			//printf("TID %d unlocking root %d in put_rw\n", value, root->key);
			printf("\nInserted key %d\n", root->left->key);
			pthread_rwlock_unlock(&root->lock);
		}
		else
		{
			//printf("TID %d locking root %d in put_rw\n", value, root->left->key);
			pthread_rwlock_wrlock(&root->left->lock);
			//printf("TID %d unlocking root %d in put_rw\n", value, root->key);
			pthread_rwlock_unlock(&root->lock);
			put_rw(root->left, root, key, value);	
		}
	}
	else if(key > root->key)
	{
		if(root->right == NULL)
		{
			root->right = CreateNewNode_rw(key, value);
			//printf("TID %d unlocking root %d in put_rw\n", value, root->key);
			printf("\nInserted key %d\n", root->right->key);
			pthread_rwlock_unlock(&root->lock);
		}
		else
		{
			//printf("TID %d locking root %d in put_rw\n", value, root->right->key);
			pthread_rwlock_wrlock(&root->right->lock);
			//printf("TID %d unlocking root %d in put_rw\n", value, root->key);
			pthread_rwlock_unlock(&root->lock);
			put_rw(root->right, root, key, value);	
		}
	}
	else
	{
		root->data = value; //In case of same key, update data
		printf("\nUpdated key %d with value %d\n", root->key, root->data);
		//printf("TID %d unlocking root %d in put_rw\n", value, root->key);
		pthread_rwlock_unlock(&root->lock);
	}
}

void get_rw(struct rw_BST_node* root, struct rw_BST_node* parent, int key, int tid)
{
	if(parent == NULL)
	{
		//printf("TID %d locking tree lock in get_rw\n", tid);
		pthread_rwlock_rdlock(&tree_lock_rw);
		if(actual_root_rw == NULL)
		{
			printf("Tree not populated yet\n");
			//printf("TID %d unlocking tree lock in get_rw\n", tid);
			pthread_rwlock_unlock(&tree_lock_rw);
			return;
		}

		//printf("TID %d locking root %d in get_rw\n", tid, actual_root_rw->key);
		pthread_rwlock_rdlock(&actual_root_rw->lock);
		root = actual_root_rw;
		//printf("TID %d unlocking tree lock in get_rw\n", tid);
		pthread_rwlock_unlock(&tree_lock_rw);
	}

	if(key < root->key)
	{
		if(root->left == NULL)
		{
			pthread_rwlock_unlock(&root->lock);
			printf("Node does not exist. TID %d\n", tid);
			return;
		}
		else
		{
			//printf("TID %d locking root %d in get_rw\n", tid, root->left->key);
			pthread_rwlock_rdlock(&root->left->lock);
			//printf("TID %d unlocking root %d in get_rw\n", tid, root->key);
			pthread_rwlock_unlock(&root->lock);
			get_rw(root->left, root, key, tid);	
		}
	}
	else if(key > root->key)
	{
		if(root->right == NULL)
		{
			pthread_rwlock_unlock(&root->lock);
			printf("Node does not exist. TID %d\n", tid);
			return;
		}
		else
		{
			//printf("TID %d locking root %d in get_rw\n", tid, root->right->key);
			pthread_rwlock_rdlock(&root->right->lock);
			//printf("TID %d unlocking root %d in get_rw\n", tid, root->key);
			pthread_rwlock_unlock(&root->lock);
			get_rw(root->right, root, key, tid);	
		}
	}
	else
	{
		printf("Key->%d; Value->%d\n", root->key, root->data); 
		//printf("TID %d unlocking root %d in get_rw\n", tid, root->key);
		pthread_rwlock_unlock(&root->lock);
	}
}

void range_queries_rw(struct rw_BST_node* root, struct rw_BST_node* parent, int lo, int hi, int tid)
{
	int lock_status = 0;
	if(parent == NULL)
	{
		//printf("TID %d locking tree lock in range queries\n", tid);
		pthread_rwlock_rdlock(&tree_lock_rw);
		if(actual_root_rw == NULL)
		{
			printf("Tree not populated yet\n");
			//printf("TID %d unlocking tree lock in range queries\n", tid);
			pthread_rwlock_unlock(&tree_lock_rw);
			return;
		}

		//printf("TID %d locking root %d in range queries\n", tid, actual_root_rw->key);
		pthread_rwlock_rdlock(&actual_root_rw->lock);
		root = actual_root_rw;
		//printf("TID %d unlocking tree lock in range queries\n", tid);
		pthread_rwlock_unlock(&tree_lock_rw);
	}

	if(lo < root->key)
	{
		if(root->left != NULL)
		{
			//printf("TID %d locking root %d in range queries\n", tid, root->left->key);
			pthread_rwlock_rdlock(&root->left->lock);
			//printf("TID %d unlocking root %d in range queries\n", tid, root->key);
			pthread_rwlock_unlock(&root->lock);
			range_queries_rw(root->left, root, lo, hi, tid);
			pthread_rwlock_rdlock(&root->lock);
		}
	}

	if(lo <= root->key && hi >= root->key)
		printf("Range Queries for TID %d -> Key->%d; Value->%d\n", tid, root->key, root->data);

	if(hi > root->key)
	{
		if(root->right != NULL)
		{
			//printf("TID %d locking root %d in range queries\n", tid, root->right->key);
			pthread_rwlock_rdlock(&root->right->lock);
			//printf("TID %d unlocking root %d in range queries\n", tid, root->key);
			pthread_rwlock_unlock(&root->lock);
			range_queries_rw(root->right, root, lo, hi, tid);
			pthread_rwlock_rdlock(&root->lock);
		}
	}

	//printf("TID %d unlocking root %d in range queries\n", tid, root->key);
	pthread_rwlock_unlock(&root->lock);
}

void inorder_rw(struct rw_BST_node* root)
{
	if(root == NULL)
		return;

	inorder_rw(root->left);
	printf("Key->%d; Value->%d\n", root->key, root->data);
	inorder_rw(root->right);
}