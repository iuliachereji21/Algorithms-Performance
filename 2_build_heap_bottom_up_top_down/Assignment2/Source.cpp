/*
* author CHEREJI IULIA
* group 30424
* I implemented two methods for building a heap from an array, bottom-up and top-down, and heapsort
* 
* Bottom-up has complexity O(n), that is because the nodes are bubbling down. At the first step only half of the nr of nodes 
* go down (because half of them are leaves) all the height of the tree (logn), at the next step only half of the previous half
* go down (and with one less level) and so on. At the end, only the root will go down all the height (logn).
* 
* Top-down has complexity O(nlogn), that is because the nodes are bubbling up. We apply the procedure for all the nodes (n),
* and every node might require to be moved all the way up to the root (the lenght of the heap until that node => logn).
* 
* => Bottom-up is more efficient
* 
* The worst case for both of them is when the array is sorted in ascending order, there the difference of efficiency between
* the algorithms is seen better.
*/
#include <stdio.h>
#include "Profiler.h"

#define MAX_SIZE 10000 
#define STEP_SIZE 100 
#define NR_TESTS 5

Profiler p("Assignment 2");

void heapifyBottomUp(int v[], int n, int i, Operation*nrop) //i = position of the node for which we verify if it is in the good place
{                                   //if it isn't we move it down searching for its place
	int largestIndex;
	if (2 * i + 1 < n) //it has 2 children
	{				   //it also enters here if i=0 and has only 1 child, but it does what it should do
		               //if v[2*i+1] doesn't exist we can't use it (error)
		nrop->count(2);
		if (v[i] > v[2 * i])
			largestIndex = i;
		else largestIndex = 2 * i;
		if (v[2 * i + 1] > v[largestIndex])
			largestIndex = 2 * i + 1;

		if (largestIndex != i) //one child larger than root
		{
			nrop->count(3);
			int aux = v[i];
			v[i] = v[largestIndex];
			v[largestIndex] = aux;
			heapifyBottomUp(v, n, largestIndex, nrop);
		}
	}
	else
	{
		if (2 * i < n) //has only 1 child
		{
			nrop->count();
			if (v[2 * i] > v[i]) //the child is larger
			{
				nrop->count(3);
				int aux = v[i];
				v[i] = v[2 * i];
				v[2 * i] = aux;
				heapifyBottomUp(v, n, 2 * i, nrop);
			}
		}
	}
	//if it has no children => it is a leaf => we are done
}

void buildHeapBottomUP(int v[], int n) //apply heapify on all the non-leaves elments of the array, in bottom-up order to the root
{                                      //and eventually all the nodes will go down to their places
	Operation opBottomUp = p.createOperation("Bottom Up Operations", n);
	int start; //the first non-leaf element
	if (n % 2 == 0)
	{
		start = n / 2 - 1;
	}
	else start = n / 2;
	
	for (int i = start; i >= 0; i--) //apply heapify 
	{
		heapifyBottomUp(v, n, i, &opBottomUp);
	}
}

void heapifyTopDown(int v[], int i, Operation*nrop2) //We compare each node with its parent and if the parent is smaller it is moved down
{									//and the child goes up to its place
	nrop2->count();
	int k = v[i]; //the value which we have to insert (to find its place in the heap)
	int parent = i / 2;
	while ((v[parent] < k) && (i > 0))
	{ //move the parent down and keep searching for the place to insert k
		nrop2->count(); //for the comparison in the while
		nrop2->count();
		v[i] = v[parent];
		i = parent;
		parent = i / 2;
	}
	nrop2->count(); //for the comparison in while which was false and got out
	nrop2->count();
	v[i] = k; //we found the place
}

void buildHeapTopDown(int v[], int n) //We compare each node with its parent and if the parent is smaller it is moved down, and
{                                     //the child goes up. We apply this for all the nodes and they will move up to their place
	Operation opTopDown = p.createOperation("Top Down Operations", n);
	for (int i = 1; i < n; i++)
	{
		heapifyTopDown(v, i, &opTopDown);
	}
}

void heapSort(int v[], int n)
{
	Operation notnecesary = p.createOperation("nimic", n);
	buildHeapBottomUP(v, n);

	while (n >= 2)
	{
		int aux = v[0];
		v[0] = v[n - 1];
		v[n - 1] = aux;

		n--;
		heapifyBottomUp(v, n, 0, &notnecesary);
	}
}

void demo()
{
	int vtest1[] = { 7, 2, 8, 1, 0, 1, 9, 5, 4 };
	buildHeapBottomUP(vtest1, 9);
	int vtest2[] = { 7, 2, 8, 1, 0, 1, 9, 5, 4 };
	buildHeapTopDown(vtest2, 9);
	int vtest3[] = { 7, 2, 8, 1, 0, 1, 9, 5, 4 };
	heapSort(vtest3, 9);
	printf("Bottom Up: \n");
	for (int i = 0; i <9 ; i++)
	{
		printf("%d, ", vtest1[i]);
	}
	printf("\nTop Down: \n");
	for (int i = 0; i < 9; i++)
	{
		printf("%d, ", vtest2[i]);
	}
	printf("\nSorted with Heapsort:\n");
	for (int i = 0; i < 9; i++)
	{
		printf("%d, ", vtest3[i]);
	}
}

void perf(int order)
{
	int v1[MAX_SIZE];
	int v2[MAX_SIZE];
	int n;
	for (n = STEP_SIZE; n <= MAX_SIZE; n += STEP_SIZE)
	{
		for (int test = 0; test < NR_TESTS; test++)
		{
			FillRandomArray(v1, n, 10, 50000, false, order);
			for (int i = 0; i < n; i++)
			{
				v2[i] = v1[i];
			}
			buildHeapBottomUP(v1, n);
			buildHeapTopDown(v2, n);
		}
	}
	p.divideValues("Bottom Up Operations", NR_TESTS);
	p.divideValues("Top Down Operations", NR_TESTS);
	p.createGroup("Total Operations", "Bottom Up Operations", "Top Down Operations");
}

void perf_all()
{
	perf(UNSORTED);
	p.reset("worst"); //dam numele urmatorului test
	perf(ASCENDING);
	p.showReport();
}

int main()
{
	//demo();
	perf_all();
	return 0;
}