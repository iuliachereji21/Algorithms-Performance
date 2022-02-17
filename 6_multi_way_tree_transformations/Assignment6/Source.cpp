/*
* author CHEREJI IULIA
* group 30424
* Assignment 6: I implemented 2 transformations between different representations of multi-way trees: 
* - R1: parent representation: for each key you are given the parent key, in a vector.
* - R2: multi-way tree representation: for each node you have the key and a vector of children nodes. My data structure 
*       r2Node has this, and the nr of children that node has.
* - R3: binary tree representation: for each node, you have the key, and two pointers: one to the first child node, 
*       and one to the next brother on the right.
* For both R2 and R3 we store a pointer to the root node.
*	T1 (parent vector => multi way tree): I used an additional array of size n, of pointers to nodes. I created a node for
*										  every key in the parent vector and stored pointers to them. Then, in an iterative
*										  way, I added in every parent node, pointers to its children. I went through the  
*										  parent array only once to complete the nodes of the tree so the complexity is
*										  O(n), linear.
*	T2 (multi way tree => binary tree) :  I implemented a recursive algorithm, it receives a pointer to a node from the 
*										  multi-way representation, creates a node for the binary representation with its
*										  key, and then, recursively creates all the children of that node, and creates the
*										  links between them. For every node, the procedure is called exactly once so the 
*										  complexity is O(n), linear.
*/

#include <stdio.h>
#include <stdlib.h>
#define MAX_NR_CHILDREN 10
#define MAX_NR_NODES 20

typedef struct r2node
{
	int key;
	int nrchildren;
	struct r2node* children[MAX_NR_CHILDREN];
}r2Node;

typedef struct r3node
{
	int key;
	struct r3node* firstChild;
	struct r3node* nextBrotherOnRight;
}r3Node;

r2Node* createNodeR2(int id)
{
	r2Node* ptr = (r2Node*)malloc(sizeof(r2Node));
	if (ptr != NULL)
	{
		ptr->key = id;
		ptr->nrchildren = 0;
		for (int i = 0; i < MAX_NR_CHILDREN; i++)
		{
			ptr->children[i] = NULL;
		}
	}
	return ptr;
}

r2Node* T1(int v[], const int n)
{
	r2Node* root = NULL;
	r2Node* nodes[MAX_NR_NODES];
	for (int i = 1; i <= n; i++)
	{
		nodes[i] = createNodeR2(i);
	}

	for (int i = 1; i <= n; i++)
	{
		if (v[i] != -1) //it isn't the root
		{
			int nr = nodes[v[i]]->nrchildren; //nr of children of the parent of the node i (that we are currently at)
			nodes[v[i]]->children[nr] = nodes[i]; //in the next available position in the array of children of the parent
												  //we add a pointer to the current node
			nodes[v[i]]->nrchildren++; //increase the nr of children of the parent
		}
		else
		{
			root = nodes[i];
		}
	}
	return root;
}

r3Node* createNodeR3(int id)
{
	r3Node* ptr = (r3Node*)malloc(sizeof(r3Node));
	if (ptr != NULL)
	{
		ptr->key = id;
		ptr->firstChild = NULL;
		ptr->nextBrotherOnRight = NULL;
	}
	return ptr;
}

r3Node* T2(r2Node* treeR2)
{
	r3Node* root = createNodeR3(treeR2->key);
	if (treeR2->nrchildren != 0)
	{
		root->firstChild = T2(treeR2->children[0]);
		r3Node* ptr = root->firstChild;
		for (int i = 1; i < treeR2->nrchildren; i++)
		{
			ptr->nextBrotherOnRight = T2(treeR2->children[i]);
			ptr = ptr->nextBrotherOnRight;
		}
	}
	return root;
}



void pretty_print(r3Node* treeR3, int depth)
{
	if (treeR3 != NULL)
	{
		for (int i = 0; i < depth; i++)
		{
			printf("	");
		}
		printf("%d\n", treeR3->key);
		pretty_print(treeR3->firstChild, depth+1);
		pretty_print(treeR3->nextBrotherOnRight, depth);
	}
	
}

void pretty_print_multi_way(r2Node* treeR2, int depth)
{
	if (treeR2 != NULL)
	{
		for (int i = 0; i < depth; i++)
		{
			printf("	");
		}
		printf("%d\n", treeR2->key);
		for (int i = 0; i < treeR2->nrchildren; i++)
		{
			pretty_print_multi_way(treeR2->children[i], depth + 1);
		}
	}

}

void demo()
{
	const int n = 9;
	int inputParentVector[n+1] = { 0, 2, 7, 5, 2, 7, 7, -1, 5, 2 };
	printf("R1 - parent vector: ");
	for (int i = 1; i <= n; i++)
		printf("%d, ", inputParentVector[i]);
	printf("\n\n");

	r2Node* treeR2 = T1(inputParentVector, n);
	printf("R2 (multi-way):\n");
	pretty_print_multi_way(treeR2, 0);
	printf("\n");

	r3Node* treeR3 = T2(treeR2);
	printf("R3 (binary representation) - pretty print:\n");
	pretty_print(treeR3, 0);

}

int main()
{
	demo();
	return 0;
}