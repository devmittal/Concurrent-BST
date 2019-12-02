#include "fine_grained_lock_BST.h"
#include <stdlib.h>
#include <stdio.h>

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
			//printf("\nInserted root key %d\n", actual_root->key);
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
			//printf("\nInserted key %d\n", root->left->key);
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
			//printf("\nInserted key %d\n", root->right->key);
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
		//printf("\nUpdated key %d with value %d\n", root->key, root->data);
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
			//printf("Tree not populated yet\n");
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
			//printf("Node does not exist. TID %d\n", tid);
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
			//printf("Node does not exist. TID %d\n", tid);
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
		//printf("Key->%d; Value->%d\n", root->key, root->data); 
		//printf("TID %d unlocking root %d in get\n", tid, root->key);
		pthread_mutex_unlock(&root->lock);
	}
}

void range_queries(struct node* root, struct node* parent, int lo, int hi, int tid)
{
	if(parent == NULL)
	{
		//printf("TID %d locking tree lock in get\n", tid);
		pthread_mutex_lock(&tree_lock);
		if(actual_root == NULL)
		{
			//printf("Tree not populated yet\n");
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
		//printf("Range Queries for TID %d -> Key->%d; Value->%d\n", tid, root->key, root->data);

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
}

void inorder(struct node* root)
{
	if(root == NULL)
		return;

	inorder(root->left);
	printf("Key->%d; Value->%d\n", root->key, root->data);
	inorder(root->right);
}