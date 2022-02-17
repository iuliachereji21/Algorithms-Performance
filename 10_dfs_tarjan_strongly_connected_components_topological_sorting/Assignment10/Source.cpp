/*
* author CHEREJI IULIA
* group 30424
* Assignment 10: I implemented the Depth-First-Search on oriented graphs. At the end of the algorithm in each node the
* contained informations are the discovery time and finishing time that were produced during the DFS and a pointer to the 
* parent in the tree in which the node was reached (and the color will surely be black even if the graph is not strongly
* connected because we apply DFS for every node if it wasn't already found). 
* 
* In the DFS the complexity is O(V) for setting the initial conditions, and O(V) for checking every node, where V is the
* nr of nodes in the graph. In the DFS_visit the complexity is O(E) for checking every edge, where E is the nr of edges
* in the graph. So the total complexity of the algorithm is O(V + E).
*
* I also implemented the topological sort, which has to be run on a DAG to be correct, otherwise it will still generate an
* order but it won't be correct if there are any cicles. We just run the DFS so the complexity is O(V + E) + O(V) for the 
* V push operations for adding each node into the list, so O(V + E).
* 
* For the Tarjan's algorithm I modified the DFS and replaced the finishing time with a lowlink information which refers to
* the smallest discovery time of a node reachable from this node. The complexity is the same as for DFS + O(V) for pushing
* the nodes in the stack and out of stack for print, and a constant time for setting and modifying the lowlink values.
* So the complexity of Tarjan's algorithm is still O(V + E);
*/
#include <stdio.h>
#include <stdlib.h>
#include "Profiler.h"
using namespace std;
#define MAX_NR_NODES 200

typedef struct _Node {
    int key;
    int adjSize;
    struct _Node** adj;

    int discoveryTime, finishingTime;
    int color; //0-white, 1-gray, 2-black
    struct _Node* parent;
    int lowlink; //for tarjan; the smallest discovery time of a node reachable from this node
}Node;

typedef struct {
    int nrNodes;
    Node** v;
}Graph;


//this is for the list generated in topological sort
typedef struct elem
{
    Node* elem;
    struct elem* next;
}ListElement;

typedef struct list
{
    ListElement* first;
    int size;
}ListType; //used as stack too

ListType* createlist()
{
    ListType* ptr = (ListType*)malloc(sizeof(ListType));
    if (ptr)
    {
        ptr->first = NULL;
        ptr->size = 0;
    }
    return ptr;
}

void pushAtTopOfList(ListType* list, Node* elem)
{
    ListElement* newListElem = (ListElement*)malloc(sizeof(ListElement));
    if (newListElem)
    {
        newListElem->elem = elem;
        newListElem->next = list->first;
        list->first = newListElem;
        list->size++;
    }
}

Node* pop(ListType* list)
{
    if (list->size > 0)
    {
        ListElement* ptr = list->first;
        list->first = list->first->next;
        list->size--;
        Node* el = ptr->elem;
        free(ptr);
        return el;
    }
    return NULL;
}


Graph* generateDAG(int nrEdges, int nrNodes) //directed aciclic graph
{ //maximum nr of edges in a DAG = nrNodes * (nrNodes-1)/2
    short adjMatrix[MAX_NR_NODES][MAX_NR_NODES];
    int v[MAX_NR_NODES];
    int i, j, nrRand, diagonalSum=0;
    //nodes start from key 0 to key nrNodes -1 
    //to gave a dag we will only complete the upper part of the matrix (above main diagonal)
    for (j = 0; j < nrNodes; j++)
    {
        for (i = 0; i < nrNodes; i++)
        {
            adjMatrix[i][j] = 0;
        }
    }

    //find how many edges should each node have and put that nr on the main diagonal
    //at the end the sum of the main diagonal should be nrEdges
    //each node i can have at most nrNodes-i-1 edges going out from it
    adjMatrix[nrNodes - 1][nrNodes - 1] = 0;
    //for (j = 0; (j < nrNodes-1 && diagonalSum<nrEdges); j++) //from the last node which has the key nrNodes-1, cand begin 0 edges, so we only go until key nrNodes-2
    for (j = nrNodes - 2; (j >=0 && diagonalSum < nrEdges); j--)
    {
        nrRand = rand() % (nrNodes - j);
        adjMatrix[j][j] = nrRand;
        adjMatrix[j + 1][j] = nrNodes - j -1 - nrRand; //the nr of edges which we can add to this node if neccessary
        diagonalSum += nrRand;
    }

    j = 0;
    while (diagonalSum < nrEdges) //enters this if we still need to add edges
    { //will at most execute nrNodes times
        if (adjMatrix[j + 1][j] > 0)//we can still add edges to the j node
        {
            adjMatrix[j][j]++;
            adjMatrix[j + 1][j]--;
            diagonalSum++;
        }
        if (j < nrNodes - 2) j++;
        else j = 0;
    }
    while (diagonalSum > nrEdges) //enters this if we need to remove edges; j must be 0 because it didn't enter the first while
    { //will at most execute nrNodes times
        if (adjMatrix[j][j] > 0)//we can still remove edges from the j node
        {
            adjMatrix[j][j]--;
            adjMatrix[j + 1][j]++;
            diagonalSum--;
        }
        if (j < nrNodes - 2) j++;
        else j = 0;
    }
    //now on the main diagonal we must have the nr of nodes from each node's adjacency list
    
    //add nrEdges of 1s in the upper part of the matrix
    for (i = 0; i < nrNodes - 2; i++)
    {
        if (adjMatrix[i][i] > 0)
        {
            FillRandomArray(v, adjMatrix[i][i], i + 1, nrNodes - 1, true, 0);
            for (j = 0; j < adjMatrix[i][i]; j++)
            {
                adjMatrix[i][v[j]] = 1;
            }
        }
    }
    if (adjMatrix[nrNodes - 2][nrNodes - 2] == 1)
        adjMatrix[nrNodes - 2][nrNodes - 1] = 1;

    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->nrNodes = nrNodes;
    graph->v = (Node**)malloc(graph->nrNodes * sizeof(Node*));

    for (int i = 0; i < graph->nrNodes; i++)
    {
        graph->v[i] = (Node*)malloc(sizeof(Node));
        graph->v[i]->key = i;
        graph->v[i]->adjSize = adjMatrix[i][i];
        graph->v[i]->adj = (Node**)malloc((adjMatrix[i][i]) * sizeof(Node*)); 
    }
    int n1, n2; //node 1 and node 2
    for (int i = 0; i < nrNodes-1; i++)
    {
        int index = 0;
        for(int j=i+1;j<nrNodes;j++)
            if (adjMatrix[i][j] == 1)
            {
                graph->v[i]->adj[index] = graph->v[j];
                index++;
            }

    }
    return graph;
}

Graph* createRandomDirectedGraph(int nrEdges, int nrNodes) //nrEdges in [1000, 9000], nrNodes in [100, 200]
{
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->nrNodes = nrNodes;
    graph->v = (Node**)malloc(graph->nrNodes * sizeof(Node*));
    short edges[9200][2];
    int vEdges[9000];
    int i, j, index;
    int nrEdgesPerNode = nrEdges / nrNodes;
    if (nrEdges % nrNodes != 0) //this happens when we vary the nr of nodes from 100 to 200 with step of 10 and the
    {                           //nr of edges is 9000 constant
        nrEdgesPerNode++; //we will at most generate 9120 edges which won't give any problem, because for a directed
                          //graph to be complete it needs nrNodes * (nrNodes-1) edges, for nrNodes=100 =>9900 edges
                          //for nrNodes=200 => 39800 edges
    }
    int newNrEdges = nrEdgesPerNode * nrNodes;

    for (int i = 0; i < newNrEdges; i++)
    {
        edges[i][0] = i / nrEdgesPerNode;
    }

    int v[90]; //at most we will have 90 edges leaving a node (when nrEdges=9000, and nrNodes=100)
    //we accept an edge from a node to itself
    index = 0;
    for (i = 0; i < nrNodes; i++)
    {
        FillRandomArray(v, nrEdgesPerNode, 0, nrNodes - 1, true, 0);
        for (j = 0; j < nrEdgesPerNode; j++)
        {
            edges[index][1] = v[j];
            index++;
        }
    }

    if (newNrEdges != nrEdges) //we take only nrEdges from how many edges we generated
    {
        FillRandomArray(vEdges, nrEdges, 0, newNrEdges - 1, true, 0);
    }
    else
        for (i = 0; i < nrEdges; i++) //we take them all
            vEdges[i] = i;
    

    for (int i = 0; i < graph->nrNodes; i++)
    {
        graph->v[i] = (Node*)malloc(sizeof(Node));
        graph->v[i]->key = i;
        graph->v[i]->adjSize = 0;
        graph->v[i]->adj = (Node**)malloc((nrEdgesPerNode) * sizeof(Node*)); //a node might have an edge with all the other nodes and itself
    }
    int n1, n2; //node 1 and node 2
    for (int i = 0; i < nrEdges; i++)
    {
        n1 = edges[vEdges[i]][0];
        n2 = edges[vEdges[i]][1];
        graph->v[n1]->adj[graph->v[n1]->adjSize] = graph->v[n2];
        graph->v[n1]->adjSize += 1;
    }
    return graph;
}

void free_graph(Graph* graph)
{
    if (graph->v != NULL) {
        for (int i = 0; i < graph->nrNodes; ++i) {
            if (graph->v[i] != NULL) {
                if (graph->v[i]->adj != NULL) {
                    free(graph->v[i]->adj);
                    graph->v[i]->adj = NULL;
                }
                graph->v[i]->adjSize = 0;
                free(graph->v[i]);
                graph->v[i] = NULL;
            }
        }
        free(graph->v);
        graph->v = NULL;
    }
    graph->nrNodes = 0;
}

void dfs_visit(Graph* graph, Node* nod, int* time, ListType* list = NULL, Operation* op = NULL)
{
    if (op != NULL) op->count(3);
    (*time)++;
    nod->discoveryTime = (*time);
    nod->color = 1; //gray
    for (int i = 0; i < nod->adjSize; i++)
    {
        if (op != NULL) op->count();
        if (nod->adj[i]->color == 0)
        {
            if (op != NULL) op->count();
            nod->adj[i]->parent = nod;
            dfs_visit(graph, nod->adj[i], time, list,op);
        }
    }
    if (op != NULL) op->count(3);
    nod->color = 2;
    (*time)++;
    nod->finishingTime = (*time);
    if (list != NULL)
        pushAtTopOfList(list, nod);
}

void dfs(Graph* graph, ListType* list =NULL, Operation* op=NULL)
{
    int i, time;
    if (op != NULL) op->count(2*graph->nrNodes);
    for (i = 0; i < graph->nrNodes; i++)
    {
        graph->v[i]->color = 0; //white
        graph->v[i]->parent = NULL;
    }
    if (op != NULL) op->count();
    time = 0;
    for (i = 0; i < graph->nrNodes; i++)
    {
        if (op != NULL) op->count();
        if (graph->v[i]->color == 0)
            dfs_visit(graph, graph->v[i], &time, list, op);
    }
}

ListType* topological_sort(Graph* graph)
{
    ListType* list = createlist();
    dfs(graph, list);
    return list;
}

void dfs_visit_tarjan(Graph* graph, Node* nod, int* time, ListType* stack_tarjan)
{
    Node* ptr;
    nod->discoveryTime = (*time);
    nod->lowlink = (*time);
    (*time)++;
    pushAtTopOfList(stack_tarjan, nod);
    nod->color = 1; //gray meaning is on stack
    for (int i = 0; i < nod->adjSize; i++)
    {
        if (nod->adj[i]->color == 0)
        {
            dfs_visit_tarjan(graph, nod->adj[i], time, stack_tarjan);
            if (nod->adj[i]->lowlink < nod->lowlink)
                nod->lowlink = nod->adj[i]->lowlink;
        }
        else
            if (nod->adj[i]->color == 1)//on stack => is part of the current SCC
                if (nod->adj[i]->discoveryTime < nod->lowlink)
                    nod->lowlink = nod->adj[i]->discoveryTime;
            //else if color==2 then this edge points to another SCC so we don't care
    }
    if (nod->lowlink == nod->discoveryTime) //nod is the root of a SCC, pop the SCC, print it, and start a new one
    {
        printf("( ");
        do
        {
            ptr = pop(stack_tarjan);
            ptr->color = 2; //not on stack anymore
            printf("%d ", ptr->key);
        } 
        while (ptr != nod);
        printf(") ");
    }
}

void dfs_tarjan(Graph* graph, ListType* stack_tarjan)
{
    int i, time;
    for (i = 0; i < graph->nrNodes; i++)
        graph->v[i]->color = 0; //white
    time = 0;
    for (i = 0; i < graph->nrNodes; i++)
        if (graph->v[i]->color == 0)
            dfs_visit_tarjan(graph, graph->v[i], &time, stack_tarjan);
}

void tarjan(Graph* graph)
{
    ListType* list = createlist();
    dfs_tarjan(graph, list);
}
void perf()
{
    int n, e;
    Profiler p("dfs");
    Graph* graph;
    // vary the number of edges
    n = 100;
    for (e = 1000; e <= 5000; e += 100) 
    {
        Operation op = p.createOperation("dfs - 100 nodes, vary edges", e);
        graph = createRandomDirectedGraph(e, n);
        dfs(graph, NULL, &op);

        free_graph(graph);
        Graph* ptr = graph;
        free(ptr);
        graph = NULL;
    }

    // vary the number of vertices
    e = 9000;
    for (n = 100; n <= 200; n += 10) 
    {
        Operation op = p.createOperation("dfs - 9000 edges, vary vertices", n);
        graph = createRandomDirectedGraph(e, n);
        dfs(graph, NULL, &op);

        free_graph(graph);
        Graph* ptr = graph;
        free(ptr);
        graph = NULL;
    }
    p.showReport();
}

void demoDFS_tarjan()
{
    int n = 10, e = 17;
    Graph* graph;
    Graph* ptr;
    
    graph = createRandomDirectedGraph(e, n);
    printf("The adjacency lists for the initial graph with %d nodes and %d edges:\n", n, e);
    for (int i = 0; i < n; i++)
    {
        printf("%d: ", graph->v[i]->key);
        for (int j = 0; j < graph->v[i]->adjSize; j++)
            printf("%d, ",graph->v[i]->adj[j]->key);
        printf("\n");
    }

    dfs(graph);
    printf("\nThe keys of the nodes and the discovery time and finishing time after running DFS:\n");
    for (int i = 0; i < n; i++)
    {
        printf("%d( %d )%d\n", graph->v[i]->discoveryTime, graph->v[i]->key, graph->v[i]->finishingTime);
    }

    printf("\nThe strongly connected components after running Tarjan's algorithm:\n");
    tarjan(graph);
    printf("\n");

    free_graph(graph);
    ptr = graph;
    free(ptr);
    graph = NULL;
    
}

void demoTopologicalSort()
{
    int n = 10, e = 22;
    Graph* graph;
    Graph* ptr;
    graph = generateDAG(e, n);
    printf("The adjacency lists for the initial DAG with %d nodes and %d edges:\n", n, e);
    for (int i = 0; i < n; i++)
    {
        printf("%d: ", graph->v[i]->key);
        for (int j = 0; j < graph->v[i]->adjSize; j++)
            printf("%d, ", graph->v[i]->adj[j]->key);
        printf("\n");
    }

    dfs(graph);
    printf("\nThe keys of the nodes and the discovery time and finishing time after running DFS on the DAG:\n");
    for (int i = 0; i < n; i++)
    {
        printf("%d( %d )%d\n", graph->v[i]->discoveryTime, graph->v[i]->key, graph->v[i]->finishingTime);
    }

    ListType* topologicalList = topological_sort(graph);
    printf("\nThe nodes sorted topologically:\n");
    ListElement* listptr = topologicalList->first;
    while (listptr != NULL)
    {
        printf("%d, ", listptr->elem->key);
        listptr = listptr->next;
    }
    printf("\n");

    free_graph(graph);
    ptr = graph;
    free(ptr);
    graph = NULL;
}

int main()
{
    //demoDFS_tarjan();
    demoTopologicalSort();
    //perf();
	return 0;
}