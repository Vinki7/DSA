#include <stdio.h>
#include <stdlib.h>

// Define global variables
int firstOutput = 1;

// Define structures for graph representation
// Define a structure for edge
typedef struct edge{
    int destinationVertex; // (v in V)
    int weight; // w(u, v)
}Edge;

typedef struct adjacencyList{// Adjacency list represents a vertex in the graph
    Edge *edges;    // array of edges - dynamic array, adjacency list
    int numberOfEdges;  // number of edges
    int capacity; // size of the array
}AdjList;

typedef struct graph{
    AdjList *lists; // array of adjacency lists, one list for each vertex
    int numberOfVertices; // number of all vertices
}Graph;

// Let's define a structure for Min-heap
// Node of Min Heap
typedef struct heapNode{
    int vertex;  // Vertex number
    int dist;    // Distance from source
} MinHeapNode;

// Structure to represent a min heap
typedef struct heap{
    int capacity;   // Maximum capacity of the heap
    int size;       // Current size of the heap
    MinHeapNode *elements;  // Array of heap nodes
} MinHeap;


void addEdge(Graph **graph, int sourceVertex, int destinationVertex, int weight){
    // add edge from sourceVertex to destinationVertex with weight
    (*graph)->lists[sourceVertex].capacity++;
    (*graph)->lists[sourceVertex].edges = (Edge *)realloc((*graph)->lists[sourceVertex].edges, (*graph)->lists[sourceVertex].capacity * sizeof(Edge));
    (*graph)->lists[sourceVertex].edges[(*graph)->lists[sourceVertex].numberOfEdges].destinationVertex = destinationVertex;
    (*graph)->lists[sourceVertex].edges[(*graph)->lists[sourceVertex].numberOfEdges].weight = weight;
    (*graph)->lists[sourceVertex].numberOfEdges++;
    // add edge from destinationVertex to sourceVertex with weight
    (*graph)->lists[destinationVertex].capacity++;
    (*graph)->lists[destinationVertex].edges = (Edge *)realloc((*graph)->lists[destinationVertex].edges, (*graph)->lists[destinationVertex].capacity * sizeof(Edge));
    (*graph)->lists[destinationVertex].edges[(*graph)->lists[destinationVertex].numberOfEdges].destinationVertex = sourceVertex;
    (*graph)->lists[destinationVertex].edges[(*graph)->lists[destinationVertex].numberOfEdges].weight = weight;
    (*graph)->lists[destinationVertex].numberOfEdges++;
}

Graph *initialiseGraph(int numberOfVertices, int numberOfEdges){
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    graph->numberOfVertices = numberOfVertices;
    graph->lists = (AdjList *)malloc(numberOfVertices * sizeof(AdjList));

    // initialize all adjacency lists as empty
    for (int i = 0; i < numberOfVertices; i++){
        graph->lists[i].edges = NULL;
        graph->lists[i].numberOfEdges = 0;
        graph->lists[i].capacity = 0;
    }

    for (int i = 0; i < numberOfEdges; i++)
    {
        int sourceVertex;
        int destinationVertex;
        int weight;
        char line[100];
        fgets(line, sizeof(line), stdin);
        if (sscanf(line, " (%d, %d, %d)", &sourceVertex, &destinationVertex, &weight) == 3)
        {
            addEdge(&graph, sourceVertex, destinationVertex, weight);
        }else{
            return NULL;
        }
    }

    return graph;
}

Graph *insertToGraph(Graph **graph, int sourceVertex, int destinationVertex, int weight){
    int err = 0;
    if (sourceVertex > (*graph)->numberOfVertices || destinationVertex > (*graph)->numberOfVertices){
        err = 1;
    }else{
        addEdge(graph, sourceVertex, destinationVertex, weight);
    }

    if (err)
    {
        if (firstOutput)
        {
            printf("Insertion failed");
            firstOutput = 0;
        }else{
            printf("\nInsertion failed");
        }
    }
    return *graph;
    
}

void displayGraph(Graph *graph){
    for (int i = 0; i < graph->numberOfVertices; i++)
    {
        printf("Vertex %d: ", i);
        for (int j = 0; j < graph->lists[i].numberOfEdges; j++)
        {
            printf("(%d, %d) ", graph->lists[i].edges[j].destinationVertex, graph->lists[i].edges[j].weight);
        }
        printf("\n");
    }
}

void freeGraph(Graph *graph) {
    for (int i = 0; i < graph->numberOfVertices; i++) {
        free(graph->lists[i].edges);
    }
    free(graph->lists);
    free(graph);
}


// Function to create a Min Heap
MinHeap *createMinHeap(int capacity) {
    MinHeap *minHeap = (MinHeap *)malloc(sizeof(MinHeap));
    minHeap->capacity = capacity;
    minHeap->size = 0;
    minHeap->elements = (MinHeapNode *)malloc(sizeof(MinHeapNode) * capacity);
    return minHeap;
}

// Declare min-heap operations

// A utility function to swap two nodes of min heap.
void swapMinHeapNode(MinHeapNode *a, MinHeapNode *b) {
    MinHeapNode temp = *a;
    *a = *b;
    *b = temp;
}

// A standard function to heapify at given idx
void minHeapify(MinHeap *minHeap, int idx) {
    // idx is the index of an element in the heap
    int smallest = idx;// Initialize smallest as root
    int left = 2 * idx + 1;// left child
    int right = 2 * idx + 2;// right child
    // compare the element with its left and right children and find the smallest value
    if (left < minHeap->size && minHeap->elements[left].dist < minHeap->elements[smallest].dist){
        smallest = left;
    }

    if (right < minHeap->size && minHeap->elements[right].dist < minHeap->elements[smallest].dist){
        smallest = right;
    }

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->elements[smallest], &minHeap->elements[idx]); // swap the nodes
        minHeapify(minHeap, smallest);// recursively heapify the affected sub-tree
    }
}

// A utility function to check if the given minHeap is empty or not
MinHeapNode extractMin(MinHeap *minHeap) {
    if (minHeap->size <= 0){
        return (MinHeapNode){-1, 0};// return -1 if the heap is empty and the distance is 0
    }
    MinHeapNode root = minHeap->elements[0];
    minHeap->elements[0] = minHeap->elements[minHeap->size - 1];
    minHeap->size--;
    minHeapify(minHeap, 0);// heapify the root - maintaint the heap property
    return root;
}

// A utility function to decrease the distance value of a given vertex v
void decreaseKey(MinHeap *minHeap, int vertex, int dist) {
    // This function decreases the key (distance) of vertex to dist. It is assumed that dist is smaller than the original key of vertex
    // Also called as relaxing the vertex

    int i;
    for (i = 0; i < minHeap->size; i++){
        if (minHeap->elements[i].vertex == vertex){// Match the vertex
            break;
        }
    }

    minHeap->elements[i].dist = dist;
    while (i && minHeap->elements[i].dist < minHeap->elements[(i - 1) / 2].dist) {// Heapify-up the node to maintain the heap property
        swapMinHeapNode(&minHeap->elements[i], &minHeap->elements[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}


// Function to find the shortest path from source vertex to destination vertex - Dijkstra's algorithm
void *searchPath(int sourceVertex, int destinationVertex, Graph *graph) {
    int *distances = (int *)malloc(graph->numberOfVertices * sizeof(int));// array to store the distances from the source vertex
    int *predecessors = (int *)malloc(graph->numberOfVertices * sizeof(int));// array to store the predecessors of the vertices
    int *path = NULL;// array to store the path from source to destination
    MinHeap *minHeap = createMinHeap(graph->numberOfVertices);

    for (int v = 0; v < graph->numberOfVertices; v++) {
        distances[v] = INT_MAX;// Initialize distances to all vertices as INFINITE
        predecessors[v] = -1;// Initialize predecessors for all vertices as -1
        minHeap->elements[v] = (MinHeapNode){v, distances[v]};
        minHeap->size++;
    }

    distances[sourceVertex] = 0;
    decreaseKey(minHeap, sourceVertex, 0);// Make the distance value of the source vertex as 0 so that it is extracted first

    while (minHeap->size > 0) {
        MinHeapNode minNode = extractMin(minHeap);
        int u = minNode.vertex;

        for (int i = 0; i < graph->lists[u].numberOfEdges; i++) {
            int v = graph->lists[u].edges[i].destinationVertex;
            int weight = graph->lists[u].edges[i].weight;

            if (distances[u] != INT_MAX && distances[v] > distances[u] + weight) {
                distances[v] = distances[u] + weight;
                predecessors[v] = u;
                decreaseKey(minHeap, v, distances[v]);
            }
        }
    }

    int count = 0;// Count of vertices in the path
    // Extract path from source to destination
    if (distances[destinationVertex] != INT_MAX) {// if the destination vertex is reachable
        int crawl = destinationVertex; // destination vertex
        while (crawl != -1) {
            count++;
            crawl = predecessors[crawl];
        }
        path = (int *)malloc(count * sizeof(int));
        crawl = destinationVertex;
        for (int i = count - 1; i >= 0; --i) {
            path[i] = crawl;
            crawl = predecessors[crawl];
        }
    }

    free(distances);
    free(predecessors);
    free(minHeap->elements);
    free(minHeap);

    for (int i = 0; i < count; i++)
    {
        if (firstOutput)
        {
            printf("[%d", path[i]);
            firstOutput = 0;
        }else if (!i)
        {
            printf("\n[%d", path[i]);
        }else if (i == count - 1)
        {
            printf(", %d]", path[i]);
        }else{
            printf(", %d", path[i]);
        }   
    }
    free(path);
}


int main(void){
    int temp1;
    int temp2;
    Graph *graph;
    char line[50];
    fgets(line, sizeof(line), stdin);
    sscanf(line, "%d %d", &temp1, &temp2);
    graph = initialiseGraph(temp1, temp2);
    if (graph == NULL)
    {
        printf("Initialization failed");
    }/*else{
        displayGraph(graph);
    }*/

    while(fgets(line, sizeof(line), stdin) != NULL){
        char opCode;
        sscanf(line, "%c", &opCode);
        switch (opCode)
        {
        case 's':
            if (sscanf(line+2, "%d %d", &temp1, &temp2) == 2)
            {
                searchPath(temp1, temp2, graph);
            }else{
                if (firstOutput)
                {
                    printf("Search failed");
                    firstOutput = 0;
                }else{
                    printf("\nSearch failed");
                }
            }
            break;
        
        case 'i':
            int weight;
            if (sscanf(line+2, "%d %d %d", &temp1, &temp2, &weight) == 3)
            {
                graph = insertToGraph(&graph, temp1, temp2, weight);
            }else{
                if (firstOutput)
                {
                    printf("Search failed");
                    firstOutput = 0;
                }else{
                    printf("\nSearch failed");
                }
            }
            break;

        

        default:
            break;
        }
    }

    return 0;
}