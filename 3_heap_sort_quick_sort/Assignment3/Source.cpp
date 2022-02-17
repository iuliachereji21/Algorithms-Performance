/*
* author CHEREJI IULIA
* group 30424
* I implemented quicksort, heapsort and quickselect.
* 
* For quicksort, the complexity is O(nlgn) in the best case, and O(n^2) in the worst case (for the partition function that I
* used, the worst case is when the array is sorted in ascending order). This depends on the way that the partition is done;
* in the best case the partition should result in two equal size parts; and in the worst, it will always be 1 element in a part
* and the rest of them in the second part.
* 
* For heapsort the complexity is O(nlgn). In the comparison between heapsort and quicksort in the average case, quicksort 
* performed better.
* 
* In the second report, the difference between the no. of operations performed by quicksort in the best and worst case is huge.
* That is because lgn is much smaller than n.
*/


#include <stdio.h>
#include "Profiler.h"

Profiler P("Assignment 3");
#define MAX_SIZE 10000 
#define STEP_SIZE 100 
#define NR_TESTS 5

int partition(int v[], int p, int r, Operation* nrop) //all the elements in the left of the returned index are smaller than the value on that
{									 //index, all the ones from the right are larger
	nrop->count();
	int x = v[r];
	int i = p - 1;
	for (int j = p; j < r; j++)
	{
		nrop->count();
		if (v[j] <= x)
		{
			i++;
			nrop->count(3);
			int aux = v[i];
			v[i] = v[j];
			v[j] = aux;
		}
	}
	nrop->count(3);
	int aux = v[i + 1];
	v[i + 1] = v[r];
	v[r] = aux;
	return i + 1;
}

void quicksort(int v[], int p, int r, Operation* nrop)
{
	
	if (p < r)
	{
		int q = partition(v, p, r, nrop);
		quicksort(v, p, q - 1, nrop);
		quicksort(v, q + 1, r, nrop);
	}
}

void heapifyBottomUp(int v[], int n, int i, Operation* nrop) //i = position of the node for which we verify if it is in the good place
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

void buildHeapBottomUP(int v[], int n, Operation* nrop) //apply heapify on all the non-leaves elments of the array, in bottom-up order to the root
{                                      //and eventually all the nodes will go down to their places
	int start; //the first non-leaf element
	if (n % 2 == 0)
	{
		start = n / 2 - 1;
	}
	else start = n / 2;

	for (int i = start; i >= 0; i--) //apply heapify 
	{
		heapifyBottomUp(v, n, i, nrop);
	}
}

void heapSort(int v[], int n)
{
	Operation HeapSortOp = P.createOperation("HeapSortOp", n);
	buildHeapBottomUP(v, n, &HeapSortOp);
	
	while (n >= 2)
	{
		int aux = v[0];
		v[0] = v[n - 1];
		v[n - 1] = aux;

		n--;
		heapifyBottomUp(v, n, 0, &HeapSortOp);
	}
}

int quickSelect(int v[], int p, int r, int i, Operation* nrop) //returns the elem which in the ordered array would be on position i
{
	if (p == r)
	{
		return v[p];
	}
	int q = partition(v, p, r, nrop);
	int k = q - p + 1;
	if (i <= k)
		return quickSelect(v, p, q - 1, i, nrop);
	else return quickSelect(v, q + 1, r, i - k, nrop);
}

void demo()
{
	int v1[] = { 2, 8, 7, 1, 3, 5, 6, 4 };
	int v2[] = { 2, 8, 7, 1, 3, 5, 6, 4 };
	int n = sizeof(v1) / sizeof(v1[0]);
	Operation useless = P.createOperation("useless", n);
	quicksort(v1, 0, n - 1, &useless);

	heapSort(v2, n);

	printf("Sorted with quicksort:\n");
	for (int i = 0; i < n; i++)
	{
		printf("%d, ", v1[i]);
	}

	printf("\nSorted with heapsort:\n");
	for (int i = 0; i < n; i++)
	{
		printf("%d, ", v2[i]);
	}

	int v3[] = { 2, 8, 7, 1, 3, 5, 6, 4 };
	int nr = quickSelect(v3, 0, 7, 3, &useless);
	printf("\nquickselect: the value which in the sorted array should be on the second position: %d\n", nr);
}

void perf(int chart) //chart = 0 =>quicksort vs heapsort on average case
					 //        1 =>quicksort average, best, worst
{
	int v1[MAX_SIZE];
	int v2[MAX_SIZE];
	int n;
	if (chart == 0)
	{
		for (n = STEP_SIZE; n <= MAX_SIZE; n += STEP_SIZE)
		{
			Operation QuickSortOp = P.createOperation("QuickSortOp", n);
			for (int test = 0; test < NR_TESTS; test++)
			{
				FillRandomArray(v1, n);
				for (int i = 0; i < n; i++)
				{
					v2[i] = v1[i];
				}
				heapSort(v1, n);
				quicksort(v2, 0, n - 1, &QuickSortOp);
				
			}
		}
		P.divideValues("HeapSortOp", NR_TESTS);
		P.divideValues("QuickSortOp", NR_TESTS);
		P.createGroup("Total Operations", "HeapSortOp", "QuickSortOp");
	}
	else
	{
		for (n = STEP_SIZE; n <= MAX_SIZE; n += STEP_SIZE)
		{
				Operation QuickSortOp_Best = P.createOperation("QuickSortOp_Best", n);
				FillRandomArray(v1, n);
				quicksort(v1, 0, n - 1, &QuickSortOp_Best);
		}
		for (n = STEP_SIZE; n <= MAX_SIZE; n += STEP_SIZE)
		{
			Operation QuickSortOp_Worst = P.createOperation("QuickSortOp_Worst", n);
			FillRandomArray(v1, n,10,50000,false,1);
			quicksort(v1, 0, n - 1, &QuickSortOp_Worst);
		}
		P.createGroup("Total Operations Quicksort", "QuickSortOp_Best", "QuickSortOp_Worst");
	}
}

void perf_all()
{
	perf(0);
	P.reset("Quicksort");
	perf(1);
	P.showReport();
}

int main()
{
	demo();
	//perf_all();
	return 0;
}