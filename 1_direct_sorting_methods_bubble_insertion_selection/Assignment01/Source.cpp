/*
* author CHEREJI IULIA
* group 30424
* I compared 3 sorting methods: bubble sort, insertion sort and selection sort
* 
* In the average case, the complexity is O(n^2) comparisons for all 3 algorithms (selection sort performs the exact same number of
* comparisons for any input n*(n-1)/2), and O(n^2) assignments for bubble and insertion and O(n) for selection sort. 
* As total nr of op in our trial: bubble > insertion =~ selection
* 
* In the best case(ascending ordered array): bubble and insertion did n-1 comparisons and selection n(n-1)/2 comparations, 
* bubble and selection did 0 assignments and insertion did n-1 assignments because of the auxiliary variable.
* As total nr of op in our trial: selection > insertion > bubble
* 
* In the worst case(descending ordered array): same nr of comparisons: n(n-1)/2 for all 3, O(n^2) assignments for bubble and insertion 
* and O(n)for selection.
* As total nr of op in our trial: bubble > insertion > selection
*/

#include <stdio.h>
#include "Profiler.h"

#define MAX_SIZE 10000 
#define STEP_SIZE 100 //we try on a vector of 100 elements, 200 and so on until 10000
#define NR_TESTS 5

Profiler p("Assignment1");

void bubble_sort(int v[], int n)
{
	Operation opComp = p.createOperation("bubble-comp", n);
	Operation opAttr = p.createOperation("bubble-attr", n);

	int i = 0; //nr of passes so far
	int moved = 1;
	while (moved == 1) //it might still be unordered
	{
		moved = 0;
		for (int j = 1; j < n - i; j++)
		{
			opComp.count();
			if (v[j - 1] > v[j]) //stability!!!!!
			{
				opAttr.count(3);
				int aux = v[j - 1];
				v[j - 1] = v[j];
				v[j] = aux;
				moved = 1;
			}
		}
		i++;
	}
}

void insertion_sort(int v[], int n)
{
	Operation opComp = p.createOperation("insertion-comp", n);
	Operation opAttr = p.createOperation("insertion-attr", n);

	for (int i = 1; i < n; i++) //we consider that the first element is alredy a sorted "array" so we start from position 1 not 0
	{							//and when i=2 the ordered part contains 2 elements and so on
		opAttr.count();
		int aux = v[i]; //first element in the unordered part, which we have to insert
		int j = i - 1; //position of the largest element in the ordered part
		while (j >= 0) //we are in the array
		{
			opComp.count();
			if (v[j] > aux) //we search for the place to insert aux (for the first element we find in the ordered part to be smaller than aux
			{
				//move element to right and keep searching
				opAttr.count();
				v[j + 1] = v[j];
				j--;
			}
			else break; //we found the element smaller than aux
		}
		if (j != i - 1) // if j=i-1 then element is alredy where it should bo => don't move (largest element so far)
		{				//stability!!!!
			opAttr.count();
			v[++j] = aux;
		}
	}
}

void selection_sort(int v[], int n)
{
	Operation opComp = p.createOperation("selection-comp", n);
	Operation opAttr = p.createOperation("selection-attr", n);

	for (int i = 0; i < n-1; i++)
	{
		int jmin = i;
		for (int j = i + 1; j < n; j++)
		{
			opComp.count();
			if (v[j] < v[jmin])
			{
				jmin = j;
			}
		}

		if (jmin != i) //not stable
		{
			opAttr.count(3);
			int aux = v[jmin];
			v[jmin] = v[i];
			v[i] = aux;
		}
	}
}

void afisare(int v[], int n)
{
	for (int i = 0; i < n; i++)
	{
		printf("%d, ", v[i]);
	}
	printf("\n");
}
void demo()
{
	int vtest1[] = { 5, 4, 1, 8, 3, 4 };
	int vtest2[] = { 5, 4, 1, 8, 3, 4 };
	int vtest3[] = { 5, 4, 1, 8, 3, 4 };
	int n = sizeof(vtest1) / sizeof(vtest1[1]);
	bubble_sort(vtest1, n);
	insertion_sort(vtest2, n);
	selection_sort(vtest3, n);
	printf("bubble sort:    ");
	afisare(vtest1, n);
	printf("insertion sort: ");
	afisare(vtest2, n);
	printf("selection sort: ");
	afisare(vtest3, n);
}

void perf(int order)
{
	int v1[MAX_SIZE];
	int v2[MAX_SIZE];
	int v3[MAX_SIZE];
	int n;
	for (n = STEP_SIZE; n <= MAX_SIZE; n += STEP_SIZE) 
	{
		for (int test = 0; test < NR_TESTS; test++) 
		{
			FillRandomArray(v1, n, 10, 50000, false, order);
			for (int i = 0; i < n; i++)
			{
				v2[i] = v1[i];
				v3[i] = v1[i];
			}
			if (order == DESCENDING) //THE WORST CASE FOR SELECTION SORT IS NOT DESCENDING, THIS WILL ACTUALLY IMPROVE
			{						 //THE NR OF ATTRIBUTIONS BECAUSE IT PUTS THE MAXIMAL ELEMENTS AT THE END SO THEY DON'T NEED TO MOVE
				FillRandomArray(v3, n, 10, 50000, false, UNSORTED);
			}
		
			bubble_sort(v1, n);
			insertion_sort(v2, n);
			selection_sort(v3, n);
		}
	}
	p.divideValues("bubble-attr", NR_TESTS);
	p.divideValues("bubble-comp", NR_TESTS);
	p.addSeries("bubble", "bubble-attr", "bubble-comp");
	p.divideValues("insertion-attr", NR_TESTS);
	p.divideValues("insertion-comp", NR_TESTS);
	p.addSeries("insertion", "insertion-attr", "insertion-comp");
	p.divideValues("selection-attr", NR_TESTS);
	p.divideValues("selection-comp", NR_TESTS);
	p.addSeries("selection", "selection-attr", "selection-comp");
	p.createGroup("attr", "bubble-attr", "insertion-attr","selection-attr");
	p.createGroup("comp", "bubble-comp", "insertion-comp","selection-comp");
	p.createGroup("total", "bubble", "insertion","selection");
}

void perf_all()
{
	perf(UNSORTED);
	p.reset("best"); //dam numele urmatorului test
	perf(ASCENDING);
	p.reset("worst");
	perf(DESCENDING);
	p.showReport();
}

int main()
{
	demo();
	//perf_all();
	return 0;
}