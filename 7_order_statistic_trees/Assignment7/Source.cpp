/*
* author CHEREJI IULIA
* group 30424
* Assignment 7: I implemented an ordered statistic tree, which is a balanced, augmented BST. The augmentation is the property
* that in each node, we additionally store the size (the nr of nodes in the subtree rooted at that node) to speed up the process
* of finding the i-th element in the sorted array. The balance reffers to the fact that the difference of sizes between a node's 
* left and right subtree should be at most 1. 
* 
* We build the tree to be balanced from the begining, so we don't need to rebalance it after every delete, since it's height
* will never be larger than logn; and rebalancing will take a lot of time at the deletes.
* 
* OS-SELECT resembles Hoare's QuickSelect which selects the i-th smallest element from an unordered array, without the partition
* because we have a tree and also have the size knowledge.
* 
* For Build operation, the complexity is O(n) (computed with master theorem); I got 5*n operations for a build of size n.
* For Select, it should be O(h) => O(logn), and same for Delete
*/
#include <stdio.h>
#include <stdlib.h>
#include "Profiler.h"

Profiler p("Assignment7");

typedef struct nod
{
	int key;
	struct nod *left, *right;
	int size; //size of the tree roted at this node
} nodeT;

void os_delete(nodeT** tree, int key, nodeT* parent, Operation* nrop); //declared here and defined above (i did that so find_min can see it)

nodeT* createNode(int key)
{
	nodeT* ptr = (nodeT*)malloc(sizeof(nodeT));
	if (ptr!=NULL)
	{
		ptr->key = key;
		ptr->left = NULL;
		ptr->right = NULL;
		ptr->size = 1;
	}
	return ptr;
}

nodeT* build_tree(int left, int right, Operation* nrop)
{
	nrop->count();
	if (left <= right)
	{
		nrop->count(3);
		int middle = (left + right) / 2;
		nodeT* root = createNode(middle);
		root->size += right - left;

		root->left = build_tree(left, middle - 1, nrop);
		root->right = build_tree(middle + 1, right, nrop);
		return root;
	}
	else return NULL;
}

nodeT* os_select(nodeT* tree, int i, Operation* nrop)
{
	if (tree == NULL) return NULL; //i is larger than the nr of elements so there is no value which should be on the i-th position
	nrop->count(2);
	int rank;
	if (tree->left)
		rank = tree->left->size + 1;
	else rank = 1;

	nrop->count();
	if (i == rank) //found it
	{
		return tree;
	}
	else
	{
		nrop->count();
		if (i < rank) //i-th smallest is on the left
		{
			return os_select(tree->left, i, nrop);
		}
		else //i-th smallest is on the right
		{
			return os_select(tree->right, i - rank, nrop);
		}
	}
}

int find_min_and_update_sizes (nodeT *tree, nodeT *parent, Operation* nrop)
{
	nrop->count();
	if (tree->left) //is not the smallest
	{
		nrop->count();
		tree->size--;
		return find_min_and_update_sizes(tree->left, tree, nrop);
	}
	else //found min
	{
		nrop->count();
		int min_value = tree->key;
		os_delete(&tree, min_value, parent, nrop);
		return min_value;
	}
}

void os_delete(nodeT** tree, int key, nodeT* parent, Operation* nrop)
{
    //we suppose that the key is in the tree and we will find it
	nrop->count();
	if (key < (*tree)->key)
	{
		nrop->count();
		(*tree)->size--; //to update the size of this subtree because we will delete a node from the left
		os_delete(&((*tree)->left), key, (*tree), nrop);
	}
	else
	{
		nrop->count();
		if (key > (*tree)->key)
		{
			nrop->count();
			(*tree)->size--; //to update the size of this subtree because we will delete a node from the right
			os_delete(&((*tree)->right), key, (*tree), nrop);
		}
		else //found the node where the key is
		{
			nrop->count();
			if ((*tree)->left && (*tree)->right) //has both children
			{
				//We replace the value with the minimum from the right subtree (its successor)
				//while searching, we also update the sizes of the nodes along that path because we will delete the minimum
				//which is a node on that path (the deletion is also done in find_min).
				//We update in find_min so that we don't need to do traverse 2 times the path from node_to_delete to minimum
				//(once to find the min, and second to make the updates on that path and to delete the min)
				nrop->count(3);
				int min = find_min_and_update_sizes((*tree)->right, (*tree), nrop);
				(*tree)->key = min;
				(*tree)->size--;
			}
			else //only one or no child
			{
				nrop->count();
				if (parent != NULL)
				{
					nrop->count(2);
					nodeT* aux = (*tree);
					if (parent->left == (*tree))
					{
						nrop->count(2);
						if ((*tree)->right != NULL)
							parent->left = (*tree)->right;
						else parent->left = (*tree)->left;
					}
					else
					{
						nrop->count(2);
						if ((*tree)->right != NULL)
							parent->right = (*tree)->right;
						else parent->right = (*tree)->left;
					}
					free(aux);
				}
				
				else //if parent == NULL =>we delete the root of the tree
				{
					nrop->count(3);
					nodeT* aux = (*tree);
					if ((*tree)->right)
						(*tree) = (*tree)->right;
					else (*tree) = (*tree)->left;
					free(aux);
				} 
				
			}
		}
	}
}

void pretty_print(nodeT* tree, int depth) //made so that you see tree if you flip your head to the left
{
	if (tree != NULL)
	{
		for (int i = 0; i < depth; i++)
		{
			printf("	");
		}
		printf("%d\n", tree->key);
		pretty_print(tree->right, depth + 1); //if we want normal pretty print exchage right and left words below
		pretty_print(tree->left, depth + 1);
	}
}

void perf()
{
	int randomNumber;
	int nrdeleted;
	nodeT* tree;
	nodeT* nod;
	int key;
	for (int i = 1; i <= 5; i++) //nr of tests
	{
		for (int n = 100; n <= 10000; n += 100) //nr of nodes of the tree
		{
			Operation opBuild = p.createOperation("Build", n);
			Operation opSelect = p.createOperation("Select", n);
			Operation opDelete = p.createOperation("Delete", n);
			nrdeleted = 0;
			tree = build_tree(1, n, &opBuild);
			while (nrdeleted < n)
			{
				randomNumber = (rand() % (n-nrdeleted)) + 1;
				nod = os_select(tree, randomNumber, &opSelect);
				if (nod != NULL)
				{
					key = nod->key;
					os_delete(&tree, key, NULL, &opDelete);
					nrdeleted++;
				}
			}
		}
	}
	p.divideValues("Build", 5);
	p.divideValues("Select", 5);
	p.divideValues("Delete", 5);
	p.createGroup("Operations", "Build", "Select", "Delete");
	p.showReport();
}

void demo()
{
	int n = 11;
	Operation useless = p.createOperation("Don't need", n);
	nodeT* tree = build_tree(1, n, &useless);
	pretty_print(tree, 0);
	
	int randnr;
	int nrdeleted = 0;
	while (nrdeleted < 3)
	{
		randnr = (rand() % n) + 1;
		nodeT* nod = os_select(tree, randnr, &useless);
		if (nod == NULL)
		{
			printf("The index %d is larger than the nr of nodes left in the tree\n", randnr);
		}
		else
		{
			int k = nod->key;
			printf("The key that in the ordered array will be on position %d is: %d\n", randnr, k);
			os_delete(&tree, k, NULL, &useless);
			printf("The tree after deleting key %d is:\n", k);
			pretty_print(tree, 0);
			nrdeleted++;
		}
	}
	
}

int main()
{
	demo();
	//perf();
	return 0;
}