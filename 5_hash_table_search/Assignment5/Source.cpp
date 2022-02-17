/*
* author CHEREJI IULIA
* group 30424
* Assignment 5: I implemented the search and insert operations in a hash table using open addressing with quadratic probing.
* In theory, the time required for a successful search is (1/a)*ln(1/(1-a)); where a is the fill factor, and for an 
* unsuccessful search is 1/(1-a). That means that the values obtained should be:
*    a  | Avg. Effort Found | Avg. Effort Not Found
*  0.80 |        2.01       |          5
*  0.85 |        2.23       |          6.66
*  0.90 |        2.55       |          10
*  0.95 |        3.15       |          20
*  0.99 |        4.65       |          100
* 
* The number of accessed cells is very small when searching for a key in a hash table using open addressing (the final 
* address is not completely determined by the hash code, it also depends on the elements which are already in the hash 
* table) and quadratic/linear probing (meaning that only 1 item can be stored at a certain index in a table). The time
* is almost constant.
*/

#include <stdio.h>
#include "Profiler.h"

#define MAX_LENGTH_TABLE 10007
#define NR_TESTS 5
const int c1 = 7; 
const int c2 = 13; 

typedef struct {
	int id;
	char name[30];
} Entry;

Entry* createEntry(int id2, const char name2[30])
{
	Entry* person = (Entry*)malloc(sizeof(Entry));
	person->id = id2;
	for (int i = 0; i <= sizeof(name2) / sizeof(name2[0]); i++)
	{
		person->name[i] = name2[i];
	}
	person->name[sizeof(name2)/sizeof(name2[0]) + 1] = '\0';
	return person;
}

int hFunction(int key, int i, int m) //i=a cata incercare
{
	int val = (key % m + c1 * i + c2 * i * i) % m;
	return val;
}

void clearTable(Entry* table[], int m)
{
	Entry* ptr;
	for (int i = 0; i < m; i++)
	{
		ptr = table[i];
		free(ptr);
		table[i] = NULL;
	}
}

int insert(Entry* table[] , int id, const char name[30], int m)
{
	int t = 0;
	int i = hFunction(id, t, m);
	while ((t<m) && (table[i] != NULL))
	{
		if (table[i]->id == id)
			return -2; //this id already in table
		t++;
		i = hFunction(id, t, m);
	}
	if (table[i] == NULL)
	{
		table[i] = createEntry(id, name);
		return i; //position in the table where is added
	}
	return -1; //couldn't find an avalilable spot for this id
	
}

void insert_all(Entry* table[], int n, int m, int v[], int i) //i= indicele de la cat pana la n sa mai inserez
{
	
	for (int j = i; j < n; j++)
	{
		insert(table, v[j], "iulia", m);
	}
}

int search(Entry* table[], int idSearched, int m, int* t) //t=nr of tries
{
	int t2 = 0;
	int i = hFunction(idSearched, t2, m);
	while ((t2<m) && (table[i] != NULL) && (table[i]->id != idSearched))
	{
		t2++;
		i = hFunction(idSearched, t2, m);
	}
	*t = t2 + 1;
	if (table[i] == NULL) //id should have been placed here, since is null => id not in the table
	{
		return -1; //not found
	}
	if (table[i]->id == idSearched)
	{
		return i; //index in the table where we found the id
	}
	//t=m we verified enough
	return -1; //not found
}

void countFound(int* max, double* average, Entry* table[], int v[], int n, int m)
{
	*max = 0;
	int sum = 0;
	int step = n / 1500;
	int t = 0;
	int loc;
	int nrElemFound = 0;

	for (int i = 0; i < 1500*step; i+=step)
	{
		loc = search(table, v[i], m, &t);
		if (loc != -1) //found
		{
			nrElemFound++;
			sum += t;
			if (t > * max)
				*max = t;
		}	
	}
	*average = (double)sum / (double)nrElemFound;
}

void countNotFound(int* max, double* average, Entry* table[], int v[], int n, int m)
{
	*max = 0;
	int sum = 0;
	int t = 0;
	int loc;

	for (int i = 21000; i < 22500; i++)
	{
		loc = search(table, i, m, &t);
		sum += t;
		if (t > * max)
			*max = t;
	}
	*average = (double)sum / (double)1500;
}

void countAll(Entry* table[], int v[], int n, int m, double a, int i)
{
	insert_all(table, n, m, v, i);
	int maxFound;
	int maxNotFound;
	double avgFound;
	double avgNotFound;

	double sumAvgFound = 0;
	double sumAvgNotFound = 0;
	
	for (int i = 0; i < NR_TESTS; i++)
	{
		countFound(&maxFound, &avgFound, table, v, n, m);
		sumAvgFound += avgFound;
		countNotFound(&maxNotFound, &avgNotFound, table, v, n, m);
		sumAvgNotFound += avgNotFound;
	}
	sumAvgFound = sumAvgFound / (double)NR_TESTS;
	sumAvgNotFound = sumAvgNotFound / (double)NR_TESTS;
	if(a<0.86)
		printf("  %.2f   |       %.2f        |       %d         |          %.2f         |          %d\n", a, sumAvgFound, maxFound, sumAvgNotFound, maxNotFound);
	else printf("  %.2f   |       %.2f        |       %d         |          %.2f        |          %d\n", a, sumAvgFound, maxFound, sumAvgNotFound, maxNotFound);
}

void perf()
{
	const int m = MAX_LENGTH_TABLE;
	int n;
	float a;
	Entry* table[m];
	int v[MAX_LENGTH_TABLE];
	int i=0; //0 then n, new n ...(to not add the first n elements all over again)
	int maxFound;
	int maxNotFound;
	double avgFound;
	double avgNotFound;

	for (int i = 0; i < m; i++)
		table[i] = NULL;
	FillRandomArray(v, m, 10, 20000, true, 0);
	printf("    a    | Avg. Effort Found | Max Effort Found | Avg. Effort Not Found | Max Effort Not Found\n");
	printf("_________|___________________|__________________|_______________________|______________________\n");

	a = 0.8;
	n = (int)(a * m);
	countAll(table, v, n, m, a, i); //i=0 at first
	i = n;

	a = 0.85;
	n = (int)(a * m);
	countAll(table, v, n, m, a, i); //i=0 at first
	i = n;

	a = 0.9;
	n = (int)(a * m);
	countAll(table, v, n, m, a, i); //i=0 at first
	i = n;

	a = 0.95;
	n = (int)(a * m);
	countAll(table, v, n, m, a, i); //i=0 at first
	i = n;

	a = 0.99;
	n = (int)(a * m);
	countAll(table, v, n, m, a, i); //i=0 at first
	i = n;
}

void demo()
{
	const int m = 10;
	const int n = 9;
	int v[n];
	FillRandomArray(v, n);
	Entry* table[m];
	for (int i = 0; i < m; i++)
		table[i] = NULL;
	insert(table, v[0], "iulia", m);
	insert(table, v[1], "ana", m);
	insert(table, v[2], "adela", m);
	insert(table, v[3], "razvan", m);
	insert(table, v[4], "alexandra", m);
	insert(table, v[5], "lucia", m);
	insert(table, v[6], "paul", m);
	insert(table, v[7], "mihai", m);
	insert(table, v[8], "sorin", m);

	int tUseless = 0;
	int loc = search(table, v[0], m, &tUseless);
	if (loc != -1)
		printf("name %s found\n", table[loc]->name);
	else printf("not found\n");

	loc = search(table, 6, m, &tUseless);
	if (loc != -1)
		printf("name %s found\n", table[loc]->name);
	else printf("not found\n");

}

int main()
{
	//demo();
	perf();
	return 0;
}