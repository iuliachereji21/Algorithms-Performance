/*
* author CHEREJI IULIA
* group 30424
* Assignment 9: I implemented the Breadth-First-Search on unoriented graphs. At the end of the algorithm in each node the
* contained informations are: color (black if the node was reached from the source node, white otherwise), distance from
* the source node (root) in the bfs tree, and pointer to the parent node in the tree. For that I had to implement a queue
* and the necessary operations on it. The complexity is: O(V) for setting the initial conditions (V-nr of nodes), O(E) for
* for checking each candidate edge (E- nr of edges). 
* 
* So the complexity of BFS is O(V+E) and some constant for the other operations which take O(1).
* In the performance, my algorithm makes more operations, that is because I implemented the graph to be unoriented, so
* each edge appears in the adjacency list of both nodes, that means every edge is tested twice.
*/
#include <stdlib.h>
#include <string.h>
#include "bfs.h"
#include "Profiler.h"
#define INFINITY 2147483647

typedef struct elem
{
    Node* nod;
    struct elem* next;
} Qelement;

typedef struct queue
{
    Qelement* first;
    Qelement* last;
} Queue;

typedef struct BFSnode
{
    Point position;
    int nrchildren;
    struct BFSnode* children[4];
}BFSNode;

BFSNode* createNodeR2(Point* pozitie)
{
    BFSNode* ptr = (BFSNode*)malloc(sizeof(BFSNode));
    if (ptr != NULL)
    {
        ptr->position = (*pozitie);
        ptr->nrchildren = 0;
        for (int i = 0; i < 4; i++)
        {
            ptr->children[i] = NULL;
        }
    }
    return ptr;
}

BFSNode* Transform(int* v, const int n, Point* repr)
{
    BFSNode* root = NULL;
    BFSNode* nodes[10000];
    for (int i = 0; i < n; i++)
    {
        nodes[i] = createNodeR2(&repr[i]);
    }

    for (int i = 0; i < n; i++)
    {
        if (*(v+i) != -1) //it isn't the root
        {
            int nr = nodes[*(v + i)]->nrchildren; //nr of children of the parent of the node i (that we are currently at)
            nodes[*(v + i)]->children[nr] = nodes[i]; //in the next available position in the array of children of the parent
                                                  //we add a pointer to the current node
            nodes[*(v + i)]->nrchildren++; //increase the nr of children of the parent
        }
        else
        {
            root = nodes[i];
        }
    }
    return root;
}

void pretty_print_multi_way(BFSNode* tree, int depth)
{
    if (tree != NULL)
    {
        for (int i = 0; i < depth; i++)
        {
            printf("	");
        }
        printf("(%d %d)\n", tree->position.row, tree->position.col);
        for (int i = 0; i < tree->nrchildren; i++)
        {
            pretty_print_multi_way(tree->children[i], depth + 1);
        }
    }

}

void clearTree(BFSNode** tree)
{
    if ((*tree) != NULL)
    {
        BFSNode* ptr;
        for (int i = 0; i < (*tree)->nrchildren; i++)
        {
            clearTree(&((*tree)->children[i]));
            (*tree)->children[i] = NULL;
        }
        ptr = (*tree);
        free(ptr);
        *tree = NULL;
    }
}

Queue* createQueue()
{
    Queue* ptr = (Queue*)malloc(sizeof(Queue));
    if (ptr)
    {
        ptr->first = NULL;
        ptr->last = NULL;
    }
    return ptr;
}

Qelement* enqueue(Queue* q, Node* nod)
{
    Qelement* ptr = (Qelement*)malloc(sizeof(Qelement));
    if (ptr)
    {
        ptr->nod = nod;
        ptr->next = NULL;
        if (q->first == NULL) //empty
        {
            q->first = ptr;
            q->last = ptr;
        }
        else
        {
            q->last->next = ptr;
            q->last = ptr;
        }
    }
    return ptr;
}

Node* dequeue(Queue* q)
{
    if (q->first == NULL) //empty
        return NULL;
    Qelement* ptr = q->first;
    q->first = q->first->next;
    if (q->first == NULL) //now is empty;
        q->last = NULL;
    Node* nodetoreturn = ptr->nod;
    free(ptr);
    return nodetoreturn;
}

int get_neighbors(const Grid *grid, Point p, Point neighb[])
{
    // TODO: fill the array neighb with the neighbors of the point p and return the number of neighbors
    // the point p will have at most 4 neighbors (up, down, left, right)
    // avoid the neighbors that are outside the grid limits or fall into a wall
    // note: the size of the array neighb is guaranteed to be at least 4
    //DONE
    int r[] = { -1, 0, 1, 0 };
    int c[] = { 0, 1, 0, -1 };
    int newrow;
    int newcol;
    int nrneigh = 0;
    if (grid->mat[p.row][p.col] == 0)
        for (int i = 0; i < 4; i++)
        {
            newrow = p.row + r[i];
            newcol = p.col + c[i];
            if (newrow < grid->rows && newrow >= 0 && newcol < grid->cols && newcol >= 0 && grid->mat[newrow][newcol] == 0)
            {
                neighb[nrneigh].row = newrow;
                neighb[nrneigh].col = newcol;
                nrneigh++;
            }
        }

    return nrneigh;
}

void grid_to_graph(const Grid *grid, Graph *graph)
{
    //we need to keep the nodes in a matrix, so we can easily refer to a position in the grid
    Node *nodes[MAX_ROWS][MAX_COLS];
    int i, j, k;
    Point neighb[4];

    //compute how many nodes we have and allocate each node
    graph->nrNodes = 0;
    for(i=0; i<grid->rows; ++i){
        for(j=0; j<grid->cols; ++j){
            if(grid->mat[i][j] == 0){
                nodes[i][j] = (Node*)malloc(sizeof(Node));
                memset(nodes[i][j], 0, sizeof(Node)); //initialize all fields with 0/NULL
                nodes[i][j]->position.row = i;
                nodes[i][j]->position.col = j;
                ++graph->nrNodes;
            }else{
                nodes[i][j] = NULL;
            }
        }
    }
    graph->v = (Node**)malloc(graph->nrNodes * sizeof(Node*));
    k = 0;
    for(i=0; i<grid->rows; ++i){
        for(j=0; j<grid->cols; ++j){
            if(nodes[i][j] != NULL){
                graph->v[k++] = nodes[i][j];
            }
        }
    }

    //compute the adjacency list for each node
    for(i=0; i<graph->nrNodes; ++i){
        graph->v[i]->adjSize = get_neighbors(grid, graph->v[i]->position, neighb);
        if(graph->v[i]->adjSize != 0){
            graph->v[i]->adj = (Node**)malloc(graph->v[i]->adjSize * sizeof(Node*));
            k = 0;
            for(j=0; j<graph->v[i]->adjSize; ++j){
                if( neighb[j].row >= 0 && neighb[j].row < grid->rows &&
                    neighb[j].col >= 0 && neighb[j].col < grid->cols &&
                    grid->mat[neighb[j].row][neighb[j].col] == 0){
                        graph->v[i]->adj[k++] = nodes[neighb[j].row][neighb[j].col];
                }
            }
            if(k < graph->v[i]->adjSize){
                //get_neighbors returned some invalid neighbors
                graph->v[i]->adjSize = k;
                graph->v[i]->adj = (Node**)realloc(graph->v[i]->adj, k * sizeof(Node*));
            }
        }
    }
}

void free_graph(Graph *graph)
{
    if(graph->v != NULL){
        for(int i=0; i<graph->nrNodes; ++i){
            if(graph->v[i] != NULL){
                if(graph->v[i]->adj != NULL){
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

void bfs(Graph *graph, Node *s, Operation *op)
{
    // TODO: implement the BFS algorithm on the graph, starting from the node s
    // at the end of the algorithm, every node reachable from s should have the color BLACK
    // for all the visited nodes, the minimum distance from s (dist) and the parent in the BFS tree should be set
    // for counting the number of operations, the optional op parameter is received
    // since op can be NULL (when we are calling the bfs for display purposes), you should check it before counting:
    // if(op != NULL) op->count();
    //DONE
    if (op != NULL) op->count(3 * graph->nrNodes);
    for (int i = 0; i < graph->nrNodes; i++)
    {
        graph->v[i]->color = 0;
        graph->v[i]->dist = INFINITY;
        graph->v[i]->parent = NULL;
    }
    if (op != NULL) op->count(3);
    s->color = 1; //GRAY
    s->dist = 0;
    s->parent = NULL;
    Queue* q = createQueue();
    if (op != NULL) op->count();
    enqueue(q, s);
    Node* u;
    while (q->first != NULL)
    {
        if (op != NULL) op->count();
        u = dequeue(q);
        for (int i = 0; i < u->adjSize; i++)
        {
            if (op != NULL) op->count(2);
            Node* v = u->adj[i];
            if (v->color == 0)
            {
                if (op != NULL) op->count(4);
                v->color = 1;
                v->dist = u->dist + 1;
                v->parent = u;
                enqueue(q, v);
            }
        }
        if (op != NULL) op->count();
        u->color = 2;
    }
}

void print_bfs_tree(Graph *graph)
{
    //first, we will represent the BFS tree as a parent array
    int n = 0; //the number of nodes
    int *p = NULL; //the parent array
    Point *repr = NULL; //the representation for each element in p

    //some of the nodes in graph->v may not have been reached by BFS
    //p and repr will contain only the reachable nodes
    int *transf = (int*)malloc(graph->nrNodes * sizeof(int));
    for(int i=0; i<graph->nrNodes; ++i){
        if(graph->v[i]->color == COLOR_BLACK){
            transf[i] = n;
            ++n;
        }else{
            transf[i] = -1;
        }
    }
    if(n == 0){
        //no BFS tree
        free(transf);
        return;
    }

    int err = 0;
    p = (int*)malloc(n * sizeof(int));
    repr = (Point*)malloc(n * sizeof(Node));
    for(int i=0; i<graph->nrNodes && !err; ++i){
        if(graph->v[i]->color == COLOR_BLACK){
            if(transf[i] < 0 || transf[i] >= n){
                err = 1;
            }else{
                repr[transf[i]] = graph->v[i]->position;
                if(graph->v[i]->parent == NULL){
                    p[transf[i]] = -1;
                }else{
                    err = 1;
                    for(int j=0; j<graph->nrNodes; ++j){
                        if(graph->v[i]->parent == graph->v[j]){
                            if(transf[j] >= 0 && transf[j] < n){
                                p[transf[i]] = transf[j];
                                err = 0;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    free(transf);
    transf = NULL;

    if(!err){
        // TODO: pretty print the BFS tree
        // the parrent array is p (p[k] is the parent for node k or -1 if k is the root)
        // when printing the node k, print repr[k] (it contains the row and column for that point)
        // you can adapt the code for transforming and printing multi-way trees from the previous labs
        //DONE

        BFSNode* tree = Transform(p, n, repr);
        pretty_print_multi_way(tree, 0);
        clearTree(&tree);
    }

    if(p != NULL){
        free(p);
        p = NULL;
    }
    if(repr != NULL){
        free(repr);
        repr = NULL;
    }
}

int shortest_path(Graph *graph, Node *start, Node *end, Node *path[])
{
    // TODO: compute the shortest path between the nodes start and end in the given graph
    // the nodes from the path, should be filled, in order, in the array path
    // the number of nodes filled in the path array should be returned
    // if end is not reachable from start, return -1
    // note: the size of the array path is guaranteed to be at least 1000
    //DONE
    bfs(graph, start);
    if (end->color == 2)
    {
        int nr = end->dist;
        Node* ptr = end;
        for (int i = nr; i >= 0; i--)
        {
            path[i] = ptr;
            ptr = ptr->parent;
        }
        return nr + 1;
    }
    else
        return -1;
}

void generateEdges1(int nrEdges, int nrNodes, Graph* graph)
{
    short edges[4640][2];
    int nrEdgesPerNode = nrEdges / nrNodes;
    if (nrEdges % nrNodes != 0) //this happens when we vary the nr of nodes from 100 to 200 with step of 10 and the
    {                           //nr of edges is 4500 constant
        nrEdgesPerNode++; //we will at most generate 140 more edges which won't give any problem
    }
    int newNrEdges = nrEdgesPerNode * nrNodes;
    //we will make a cycle through all the nodes at the end to ensure connectivity
    for (int i = 0; i < newNrEdges; i++)
    {
        edges[i][0] = i / nrEdgesPerNode;
        if ((i + 1) % nrEdgesPerNode == 0)
            edges[i][1] = edges[i][0] + 1;
    }
    edges[newNrEdges - 1][1] = 0;

    int added = 0;
    int v[100];
    for (int i = 0; i < nrEdgesPerNode; i++)
    {
        FillRandomArray(v, nrEdgesPerNode - 1, i + 2, i+2 +nrEdgesPerNode, true, 0);
        for (int j = 0; j < nrEdgesPerNode - 1; j++)
        {
            edges[added][1] = v[j];
            added++;
        }
        added++;//to skip the edge already added to make the cycle
    }

    for (int i = nrEdgesPerNode; i < nrNodes - nrEdgesPerNode; i++)
    {
        FillRandomArray(v, nrEdgesPerNode - 1, i + 2, nrNodes-1, true, 0); 
        for (int j = 0; j < nrEdgesPerNode - 1; j++)
        {
            edges[added][1] = v[j];
            added++;
        }
        added++;
    }

    int nrokfound;
    int index;
    boolean ok;
    for (int i = nrNodes - nrEdgesPerNode; i < nrNodes; i++) 
    {
        nrokfound = 0;
        index = 0;
        while (nrokfound < nrEdgesPerNode-1)
        {
            if (index / nrEdgesPerNode < i)
            {
                ok = true;
                for (int k = index; k < index + nrEdgesPerNode; k++)
                {
                    if (edges[k][1] == i) ok = false;
                }
                if (ok)
                {
                    edges[added][1] = index / nrEdgesPerNode;
                    added++;
                    nrokfound++;
                }
                index += nrEdgesPerNode;
            }
            else //should never get here, but just in case
            {
                int nr = nrokfound;
                for (int k = 2; k <= nrEdgesPerNode-nr; k++)
                {
                    if (i + k < nrNodes)
                    {
                        edges[added][1] = i + k;
                        added++;
                        nrokfound++;
                    }                    
                    else 
                    {
                        nrokfound=nrEdgesPerNode - 1; //there was an error
                    } 
                }
            }
        }
        added++; //to jump over the edge already added for the cycle
    }

    for (int i = 0; i < graph->nrNodes; i++)
    {
        graph->v[i]->adj = (Node**)malloc((nrNodes-1) * sizeof(Node*)); //a node might have an edge with all the other nodes
    }
    int n1, n2; //node 1 and node 2
    int addedToGraph = 0;
    for (int i = 0; i < nrNodes; i++)
    {
        n1 = i;
        n2 = i + 1;
        if (n1 == nrNodes - 1) n2 = 0;
        graph->v[n1]->adj[graph->v[n1]->adjSize] = graph->v[n2];
//*********************************************************************************************************************************************************************************************
        //comment next line for oriented graph and for performance of bfs of O(V+E)
        graph->v[n2]->adj[graph->v[n2]->adjSize] = graph->v[n1];

        graph->v[n1]->adjSize += 1;
//*********************************************************************************************************************************************************************************************
        //comment next line for oriented graph and for performance of bfs of O(V+E)
        graph->v[n2]->adjSize += 1;
    }
    addedToGraph = nrNodes;

    for (int i = 0; i < newNrEdges; i++)
    {
        if (addedToGraph >= nrEdges) break;
        else
        {
            n1 = edges[i][0];
            n2 = edges[i][1];
            if (n2 - n1 == 1) continue; //it is a cycle edge and we added it already
            else
            {
                //we make an unoriented graph so each edge will be added in both it's end nodes
                graph->v[n1]->adj[graph->v[n1]->adjSize] = graph->v[n2];
//*********************************************************************************************************************************************************************************************
                //comment next line for oriented graph and for performance of bfs of O(V+E)
                graph->v[n2]->adj[graph->v[n2]->adjSize] = graph->v[n1];

                graph->v[n1]->adjSize += 1;
//*********************************************************************************************************************************************************************************************
                //comment next line for oriented graph and for performance of bfs of O(V+E)
                graph->v[n2]->adjSize += 1;
                addedToGraph++;
            }
        }
    }
}

void performance()
{
    int n, i;
    Profiler p("bfs");

    // vary the number of edges
    for(n=1000; n<=4500; n+=100){
        Operation op = p.createOperation("bfs - 100 nodes, vary edges", n);
        Graph graph;
        graph.nrNodes = 100;
        //initialize the nodes of the graph
        graph.v = (Node**)malloc(graph.nrNodes * sizeof(Node*));
        for(i=0; i<graph.nrNodes; ++i){
            graph.v[i] = (Node*)malloc(sizeof(Node));
            memset(graph.v[i], 0, sizeof(Node));
        }
        // TODO: generate n random edges
        // make sure the generated graph is connected
        //DONE
        generateEdges1(n, graph.nrNodes, &graph);

        bfs(&graph, graph.v[0], &op);
        free_graph(&graph);
    }

    // vary the number of vertices
    for(n=100; n<=200; n+=10){
        Operation op = p.createOperation("bfs - 4500 edges, vary vertices", n);
        Graph graph;
        graph.nrNodes = n;
        //initialize the nodes of the graph
        graph.v = (Node**)malloc(graph.nrNodes * sizeof(Node*));
        for(i=0; i<graph.nrNodes; ++i){
            graph.v[i] = (Node*)malloc(sizeof(Node));
            memset(graph.v[i], 0, sizeof(Node));
        }
        // TODO: generate 4500 random edges
        // make sure the generated graph is connected
        //DONE
        generateEdges1(4500, graph.nrNodes, &graph);

        bfs(&graph, graph.v[0], &op);
        free_graph(&graph);
    }
    p.showReport();
}
