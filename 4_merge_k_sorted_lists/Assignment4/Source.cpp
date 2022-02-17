/*
* author CHEREJI IULIA
* group 30424
* Assignment 4: I implemented an algorithm for merging k ordered lists, using a min-Heap.
*
* The complexity of the merging algorithm is O(nlogk). That is because the nr of lists (k) is the number of nodes in the
* heap, and to apply heapify on a node requires O(log(height)) operations and height=k. And, because we will have to apply
* heapify for every element of every list, the complexity is O(nlogk).
* 
* The algorithm needs additional memory to store the lists, and the heap, but this memory is constant.
* From the second chart we observe that the greater the nr of lists, the worst the algorithm performs, that is because the
* height of the heap increases, and heapify performs then more operations.
* 
* From the first chart, where we vary k, we can observe that: at the beginning, the nr of operations performed by the alg.
* increases faster, i.e. heapify performs more operations. That is because, as the nr of lists(nodes) increases, it takes 
* more nodes to add to increase the height of the tree (and the nr of op performed by heapify). (it takes more nodes to
* complete the last level).
*/

#include <stdio.h>
#include <stdlib.h>
#include "Profiler.h"

#define MAX_LENGTH_LISTS 2000
Profiler p;

typedef struct node
{
	int value;
	struct node* next;
} nodeT;

nodeT* createNode(int val)
{
	nodeT* nod = (nodeT*)malloc(sizeof(nodeT)); 
	if (nod)
	{
		nod->value = val;
		nod->next = NULL;
	}
	return nod;
}

nodeT* addNode(nodeT* list, int val)
{
	if (list == NULL) //list is empty
	{
		list = createNode(val);
		return list;
	}

	nodeT* ptr = list;
	while (ptr->next != NULL)
		ptr = ptr->next;
	ptr->next = createNode(val);
	return list;
}

void clearlist(nodeT* list)
{
	nodeT* p;
	nodeT* l;
	l = list;
	while (l!=NULL)
	{
		p = l->next;
		free(l);
		l = p;
	}
	list = NULL;
}

void generateLists(nodeT* a[], int n, int k)
{
	int len = n / k;
	int v[MAX_LENGTH_LISTS]; //to generate with profiler
	for (int i = 0; i < k; i++)
	{
		FillRandomArray(v, len, 1, 500, false, 1);
		for (int j = 0; j < len; j++)
		{
			a[i] = addNode(a[i], v[j]);
		}
	}
}

void afisareLista(nodeT* list)
{
	nodeT* ptr = list;
	while (ptr!=NULL)
	{
		printf("%d, ", ptr->value);
		ptr = ptr->next;
	}
	printf("\n");
}

//we build a min-heap not a max heap
void heapifyBottomUp(int v[][2], int n, int i, Operation* nrop) //i = position of the node for which we verify if it is in the good place
{                                   //if it isn't we move it down searching for its place
	int smallestIndex;
	if (2 * i + 1 < n) //it has 2 children
	{				   //it also enters here if i=0 and has only 1 child, but it does what it should do
					   //if v[2*i+1] doesn't exist we can't use it (error)
		if(nrop!=NULL) //in this assignment we don't want to count the operations for the build heap so we give a null
			nrop->count(2);
		if (v[i][0] < v[2 * i][0])
			smallestIndex = i;
		else smallestIndex = 2 * i;
		if (v[2 * i + 1][0] < v[smallestIndex][0])
			smallestIndex = 2 * i + 1;

		if (smallestIndex != i) //one child smaller than root
		{
			if (nrop != NULL)
				nrop->count(3);
			int aux = v[i][0];
			v[i][0] = v[smallestIndex][0];
			v[smallestIndex][0] = aux;
			aux = v[i][1];
			v[i][1] = v[smallestIndex][1];
			v[smallestIndex][1] = aux;
			heapifyBottomUp(v, n, smallestIndex, nrop);
		}
	}
	else
	{
		if (2 * i < n) //has only 1 child
		{
			if (nrop != NULL)
				nrop->count();
			if (v[2 * i][0] < v[i][0]) //the child is smaller
			{
				if (nrop != NULL)
					nrop->count(3);
				int aux = v[i][0];
				v[i][0] = v[2 * i][0];
				v[2 * i][0] = aux;
				aux = v[i][1];
				v[i][1] = v[2 * i][1];
				v[2 * i][1] = aux;
				heapifyBottomUp(v, n, 2 * i, nrop);
			}
		}
	}
	//if it has no children => it is a leaf => we are done
}

void buildHeapBottomUP(int v[][2], int n) //apply heapify on all the non-leaves elments of the array, in bottom-up order to the root
{                                      //and eventually all the nodes will go down to their places
	//Operation opBottomUp = p.createOperation("Bottom Up Operations", n);
	int start; //the first non-leaf element
	if (n % 2 == 0)
	{
		start = n / 2 - 1;
	}
	else start = n / 2;

	for (int i = start; i >= 0; i--) //apply heapify 
	{
		heapifyBottomUp(v, n, i, NULL);
	}
}

void merge(int heap[][2], nodeT* lists[], int final_arr[], int k, int n, Operation* nrop)
{
	
	int i = 0; //index where to add the new element in the final array
	int dimension = k;
	while (dimension > 0)
	{
		nrop->count();
		final_arr[i] = heap[0][0];
		i++;
		nrop->count();
		if (lists[heap[0][1]] != NULL) //there are still elements to add
		{
			nrop->count(2);
			heap[0][0] = lists[heap[0][1]]->value; //we add in the heap the next new element from that list
			nodeT* l = lists[heap[0][1]];
			lists[heap[0][1]] = lists[heap[0][1]]->next; //update list to be pointing to the new element to add
			free(l);
			heapifyBottomUp(heap, dimension, 0, nrop);
		}
		else //this list is empty
		{
			//switch root with the last element in the heap
			nrop->count(3); //consideram doar 3 operatii pentru un switch
			int aux = heap[0][0];
			heap[0][0] = heap[dimension - 1][0];
			heap[dimension - 1][0] = aux;
			aux = heap[0][1];
			heap[0][1] = heap[dimension - 1][1];
			heap[dimension - 1][1] = aux;
			dimension--; //decrease the size of the heap
			heapifyBottomUp(heap, dimension, 0, nrop);
		}
	}
}


void demo()
{
	const int n = 20;
	const int k = 4;
	nodeT* lists[k];
	int final_array[n];
	
	for (int i = 0; i < k; i++)
	{
		lists[i] = NULL;
	}

	printf("n= %d, k= %d\n", n, k);
	generateLists(lists, n, k);
	for (int i = 0; i < k; i++)
	{
		printf("list nr. %d: ", i + 1);
		afisareLista(lists[i]);
	}

	//nodeT* ptr;
	int heap[k][2];
	for (int i = 0; i < k; i++) //to insert first values into the heap (which isn't yet a heap)
	{
		heap[i][0] = lists[i]->value;
		heap[i][1] = i; //the index of the list from the "lists" array which we use in this node
		nodeT* l = lists[i];
		lists[i] = lists[i]->next; //to be pointing to the next "to add" value
		free(l);
	}

	buildHeapBottomUP(heap, k);
	Operation dontneed = p.createOperation("dontneed", n);
	merge(heap, lists, final_array, k, n, &dontneed);

	printf("\nfinal sorted array: ");
	for (int i = 0; i < n; i++)
		printf("%d, ", final_array[i]);
	printf("\n");
	
}

void perf_chart1()
{
	const int k1 = 5;
	const int k2 = 10;
	const int k3 = 100;
	int final_array[10000];
	int heap1[k1][2];
	int heap2[k2][2];
	int heap3[k3][2];
	nodeT* lists1[k1];
	nodeT* lists2[k2];
	nodeT* lists3[k3];

	for (int n = 100; n <= 10000; n += 100)
	{
		Operation Op_k5 = p.createOperation("Op_k5", n);
		Operation Op_k10 = p.createOperation("Op_k10", n);
		Operation Op_k100 = p.createOperation("Op_k100", n);
		
		for (int i = 0; i < k1; i++)
		{
			lists1[i] = NULL;
		}
		for (int i = 0; i < k2; i++)
		{
			lists2[i] = NULL;
		}
		for (int i = 0; i < k3; i++)
		{
			lists3[i] = NULL;
		}

		generateLists(lists1, n, k1);
		generateLists(lists2, n, k2);
		generateLists(lists3, n, k3);

		for (int i = 0; i < k1; i++) //to insert first values into the heap (which isn't yet a heap)
		{
			heap1[i][0] = lists1[i]->value;
			heap1[i][1] = i; //the index of the list from the "lists" array which we use in this node
			nodeT* l = lists1[i];
			lists1[i] = lists1[i]->next; //to be pointing to the next "to add" value
			free(l);
		}
		buildHeapBottomUP(heap1, k1);

		for (int i = 0; i < k2; i++) //to insert first values into the heap (which isn't yet a heap)
		{
			heap2[i][0] = lists2[i]->value;
			heap2[i][1] = i; //the index of the list from the "lists" array which we use in this node
			nodeT* l = lists2[i];
			lists2[i] = lists2[i]->next; //to be pointing to the next "to add" value
			free(l);
		}
		buildHeapBottomUP(heap2, k2);

		for (int i = 0; i < k3; i++) //to insert first values into the heap (which isn't yet a heap)
		{
			heap3[i][0] = lists3[i]->value;
			heap3[i][1] = i; //the index of the list from the "lists" array which we use in this node
			nodeT* l = lists3[i];
			lists3[i] = lists3[i]->next; //to be pointing to the next "to add" value
			free(l);
		}
		buildHeapBottomUP(heap3, k3);


		merge(heap1, lists1, final_array, k1, n, &Op_k5);
		merge(heap2, lists2, final_array, k2, n, &Op_k10);
		merge(heap3, lists3, final_array, k3, n, &Op_k100);
		
	}
	p.createGroup("total operations, vary n", "Op_k5", "Op_k10", "Op_k100");
	
	
}

void perf_chart2()
{
	const int n = 10000;
	nodeT* lists[500];
	int heap[500][2];
	int final_array[n];

	for (int k = 10; k <= 500; k += 10)
	{
		Operation Op = p.createOperation("Op", k);
		for (int i = 0; i < k; i++)
		{
			lists[i] = NULL;
		}
		generateLists(lists, n, k);

		for (int i = 0; i < k; i++) //to insert first values into the heap (which isn't yet a heap)
		{
			heap[i][0] = lists[i]->value;
			heap[i][1] = i; //the index of the list from the "lists" array which we use in this node
			nodeT* l = lists[i];
			lists[i] = lists[i]->next; //to be pointing to the next "to add" value
			free(l);
		}
		buildHeapBottomUP(heap, k);

		merge(heap, lists, final_array, k, n, &Op);

		p.createGroup("total operations, vary k", "Op");
	}
}

void perf_all()
{
	perf_chart1();
	perf_chart2();
	p.showReport();
}

int main()
{
	demo();
	//perf_all();
	return 0;
}