/*
* author CHEREJI IULIA
* group 30424
* Assignment 8: I implemented the base operations for disjoint sets (make set, union, find set), and Kruskal's algorithm for finding the
* minimum spanning tree from a connected random graph, using disjoint sets.
* 
* The make set operation, performs in O(1).
* For the find set operation, in the worst case it would be the whole height of the tree, but since every find set operation updates the
* links so that every node on the path from a node to its representative will point directly to the representative, after a few finds, 
* the complexity of find set will be O(1).
* For the union operation, the complexity is O(1) also, since it performs 2 finds and an update of links.
* 
* For Kruskal: the make set takes O(n), where n is the nr of vertices; for sorting the edges the complexity is O(ElogE), where E is the nr.
* of edges in the graph, and that is because O(nlogn) is the lower bound for the sorting problem => this complexity requires that the 
* sorting algortithm is optimal (we could use Heapsort); going through the edges takes O(E), and the find set and union performed for every
* node at the end of those edges takes O(aV), a is a constant. So the sorting is the most important and the complexity of Kruskal's alg.
* is O(ElogE), depending on the sorting algorithm.
*/

#include <stdio.h>
#include "Profiler.h"
#define MAX_NR_OF_NODES_DEMO 10
#define MAX_NR_OF_NODES_TOTAL 10000
#define MAX_NR_OF_EDGES_TOTAL 40000

Profiler p("Assignment8");

typedef struct nod
{
	int key;
	int height;
	struct nod* parent;
}nodeT;

void clear(nodeT* nodes[], int n)
{
	nodeT* ptr;
	for (int i = 0; i < n; i++)
	{
		if (nodes[i] != NULL)
		{
			ptr = nodes[i];
			nodes[i] = NULL;
			free(ptr);
		}
	}
}

nodeT* makeSet(int key, Operation* nrop)
{
	nodeT* ptr = (nodeT*)malloc(sizeof(nodeT));
	nrop->count();
	if (ptr)
	{
		nrop->count(3);
		ptr->key = key;
		ptr->height = 0;
		ptr->parent = ptr;
	}
	return ptr;
}

nodeT* findSet(int key, nodeT* nodes[], int n, Operation* nrop)
{
	nodeT* nod = NULL;
	for (int i = 0; i < n; i++)
	{
		if (nodes[i] != NULL)
		{
			if (nodes[i]->key == key)
				nod = nodes[i];
		}
	}
	if (nod == NULL) return NULL; //key doesn't exist

	nrop->count();
	if (nod->parent != nod)
	{
		nod->parent = findSet(nod->parent->key, nodes, n, nrop);
	}
		
	return nod->parent;
}

nodeT* unionByRank(int key1, int key2, nodeT* nodes[], int n, Operation* nropUnion, Operation* nropFind)
{
	nodeT* representative1 = findSet(key1, nodes, n, nropFind);
	if (representative1 == NULL) return NULL;
	nodeT* representative2 = findSet(key2, nodes, n, nropFind);
	if (representative2 == NULL) return NULL;

	nropUnion->count();
	if (representative1 == representative2) return representative1; //keys already belong to the same set

	nropUnion->count();
	if (representative1->height > representative2->height)
	{
		nropUnion->count();
		representative2->parent = representative1;
		return representative1;
	}
	else
	{
		nropUnion->count();
		if (representative2->height > representative1->height)
		{
			nropUnion->count();
			representative1->parent = representative2;
			return representative2;
		}
		else //equal heights
		{
			nropUnion->count(2);
			representative2->parent = representative1;
			representative1->height++;
			return representative1;
		}
	}	
}

void createGraph(short edges[MAX_NR_OF_EDGES_TOTAL][3], int n) //n >= 10
{
	int i, j, added = 0;
	int v[MAX_NR_OF_EDGES_TOTAL];
	boolean ok;
	int nrokfound, index;

	FillRandomArray(v, 4 * n, 1, 100, false, 1); //generate edges already sorted for kruskal
	for (i = 0; i < 4 * n; i++)
	{
		edges[i][2] = v[i];
		edges[i][0] = i / 4;
	}
		
	for (i = 0; i < 4; i++)
	{
		FillRandomArray(v, 4, i + 1, i+5, true, 1); //4 values
		for (j = 0; j < 4; j++)
		{
			edges[added][1] = v[j];
			added++;
		}
	}
	for (i = 4; i < n-4; i++)
	{
		FillRandomArray(v, 4, i + 1, n - 1, true, 1); //4 values
		for (j = 0; j < 4; j++)
		{
			edges[added][1] = v[j];
			added++;
		}
	}

	for (i = n - 4; i < n; i++) //4 repetitions
	{
		nrokfound = 0;
		index = 0;
		while (nrokfound < 4)
		{
			if (index / 4 < i)
			{
				ok = true;
				for (int k = index; k < index + 4; k++)
				{
					if (edges[k][1] == i) ok = false;
				}
				if (ok)
				{
					edges[added][1] = index / 4;
					added++;
					nrokfound++;
				}
				index += 4;
			}
			else
			{
				int nr = nrokfound;
				for (int k = 1; k <= 4 - nr; k++)
				{
					edges[added][1] = i + k;
					added++;
					nrokfound++;
				}
			}
			
		}
	}
}

void kruskal(short edges[MAX_NR_OF_EDGES_TOTAL][3], int n, short mst[MAX_NR_OF_EDGES_TOTAL][3], int* nredgesMST)
{
	Operation nropMakeSet = p.createOperation("Make Set", n);
	Operation nropFindSet = p.createOperation("Find Set", n);
	Operation nropUnionByRank = p.createOperation("Union by rank", n);
	*nredgesMST = 0; //initialize the Minimum Spanning Tree to empty (i.e. set the nr of edges in it to 0)
	//here we would sort the edges but now they are given already sorted
	nodeT* nodes[MAX_NR_OF_NODES_TOTAL];
	nodeT* representative1;
	nodeT* representative2;

	for (int i = 0; i < n; i++) //for each vertex (keys from 0 to n-1) make set
		nodes[i] = makeSet(i, &nropMakeSet);
	//the edges are already sorted

	for (int i = 0; i < 4 * n; i++) //for each edge in the graph
	{
		representative1 = findSet(edges[i][0], nodes, n, &nropFindSet);
		representative2 = findSet(edges[i][1], nodes, n, &nropFindSet);
		if (representative1 != representative2) //if the two nodes don't belong to the same set => adding the edge between them
		{                                       //won't create a cycle
			mst[*nredgesMST][0] = edges[i][0];
			mst[*nredgesMST][1] = edges[i][1];
			mst[*nredgesMST][2] = edges[i][2];
			(*nredgesMST)++;
			//new edge added to MST
			unionByRank(representative1->key, representative2->key, nodes, n, &nropUnionByRank, &nropFindSet); //do like this so we don't lose time searching for
																			   //these representatives again in union
		}
	}

	clear(nodes, n);
}

void perf()
{
	short edges[MAX_NR_OF_EDGES_TOTAL][3]; // 0 = node1, 1 = node2, 2=weight (between 1 and 100)
	int n;
	short mst[MAX_NR_OF_EDGES_TOTAL][3];
	int nMst = 0;

	for (n = 100; n <= 10000; n += 100)
	{
		createGraph(edges, n);
		kruskal(edges, n, mst, &nMst);
	}
	p.createGroup("Set operations in kruskal's algorithm", "Make Set", "Find Set", "Union by rank");
	p.showReport();
}

void demoKruskal()
{
	short edges[MAX_NR_OF_EDGES_TOTAL][3]; // 0 = node1, 1 = node2, 2=weight (between 1 and 100)
	int n = 10;
	short mst[MAX_NR_OF_EDGES_TOTAL][3];
	int nMst = 0;
	createGraph(edges, n);
	printf("The %d edges of the weighted connected graph, with %d nodes, with keys between 0 and %d are:\n", 4*n, n, n - 1);
	for (int i = 0; i < n * 4; i++)
	{
		printf("(%d %d)%d\n", edges[i][0], edges[i][1], edges[i][2]);
	}
	kruskal(edges, n, mst, &nMst);
	printf("The %d edges in the Minimum Spanning Tree after Kruskal's algorithm are:\n", nMst);
	for (int i = 0; i < nMst; i++)
	{
		printf("(%d %d)%d\n", mst[i][0], mst[i][1], mst[i][2]);
	}
}

void demoSets()
{
	Operation useless = p.createOperation("useless", 10);
	const int n = MAX_NR_OF_NODES_DEMO;
	int v[n];
	nodeT* nodes[n];
	nodeT* representative;
	FillRandomArray(v, n, 1, 100, true, 0);
	for (int i = 0; i < n; i++)
	{
		nodes[i] = makeSet(v[i], &useless);
	}

	printf("the %d distinct values in the nodes are:\n",n);
	for (int i = 0; i < n; i++)
		printf("%d, ", nodes[i]->key);
	printf("\n\n");

	representative = unionByRank(nodes[0]->key, nodes[1]->key, nodes, n, &useless, &useless);
	if (representative != NULL)
		printf("Union of sets containing keys %d and %d, resulted with the representative %d, and the height of %d\n", nodes[0]->key, nodes[1]->key, representative->key, representative->height);
	else printf("Union of sets containing keys %d and %d failed, one of the keys doesn't exist\n", nodes[0]->key, nodes[1]->key);
	representative = unionByRank(nodes[4]->key, nodes[5]->key, nodes, n, &useless, &useless);
	if (representative != NULL)
		printf("Union of sets containing keys %d and %d, resulted with the representative %d, and the height of %d\n", nodes[4]->key, nodes[5]->key, representative->key, representative->height);
	else printf("Union of sets containing keys %d and %d failed, one of the keys doesn't exist\n", nodes[4]->key, nodes[5]->key);
	representative = unionByRank(nodes[5]->key, nodes[7]->key, nodes, n, &useless, &useless);
	if (representative != NULL)
		printf("Union of sets containing keys %d and %d, resulted with the representative %d, and the height of %d\n", nodes[5]->key, nodes[7]->key, representative->key, representative->height);
	else printf("Union of sets containing keys %d and %d failed, one of the keys doesn't exist\n", nodes[5]->key, nodes[7]->key);
	representative = unionByRank(nodes[5]->key, 102, nodes, n, &useless, &useless);
	if (representative != NULL)
		printf("Union of sets containing keys %d and %d, resulted with the representative %d, and the height of %d\n", nodes[5]->key, 102, representative->key, representative->height);
	else printf("Union of sets containing keys %d and %d failed, one of the keys doesn't exist\n", nodes[5]->key, 102);
	representative = unionByRank(nodes[7]->key, nodes[1]->key, nodes, n, &useless, &useless);
	if (representative != NULL)
		printf("Union of sets containing keys %d and %d, resulted with the representative %d, and the height of %d\n", nodes[7]->key, nodes[1]->key, representative->key, representative->height);
	else printf("Union of sets containing keys %d and %d failed, one of the keys doesn't exist\n", nodes[7]->key, nodes[1]->key);
	
	representative = findSet(nodes[1]->key, nodes, n, &useless);
	if (representative != NULL)
		printf("The representative of the key %d is %d\n", nodes[1]->key, representative->key);
	else printf("The key %d doesn't exist\n", nodes[1]->key);
	representative = findSet(102, nodes, n, &useless);
	if (representative != NULL)
		printf("The representative of the key %d is %d\n", 102, representative->key);
	else printf("The key %d doesn't exist\n", 102);

	clear(nodes, n);
}



int main()
{
	//demoSets();
	demoKruskal();
	//perf();
	return 0;
}