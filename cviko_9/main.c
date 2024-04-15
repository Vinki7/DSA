#include <stdio.h>
#include <stdlib.h>

#define INT_MAX 2147483647

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

// Initialize the graph, arguments: number of vertices which should represent the graph
Graph *initialiseGraph(int numberOfVertices){
    // Alloc. mem. for graph
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    if (graph == NULL)// memory alloc. failure
    {
        return NULL;
    }

    // Initialise graph
    graph->numberOfVertices = numberOfVertices;
    graph->lists = calloc(numberOfVertices, sizeof(AdjList));
    if (graph->lists == NULL)// memory alloc. failure
    {
        free(graph);
        return NULL;
    }

    // Initialise adjacency lists
    for (int i = 0; i < numberOfVertices; i++){
        graph->lists[i].edges = NULL;
        graph->lists[i].numberOfEdges = 0;
        graph->lists[i].capacity = 0;
    }

    return graph;
}

// Function to insert an edge to the graph - add an edge to the adjacency list of the source vertex
int insertEdge(Graph *graph, int sourceVertex, int destinationVertex, int weight){
    if (sourceVertex >= graph->numberOfVertices || destinationVertex >= graph->numberOfVertices){
        return 0;// invalid vertices, 0 - false
    }

    AdjList *list = &graph->lists[sourceVertex];// get the adjacency list of the source vertex

    if (list->numberOfEdges >= list->capacity){
        int newCapacity = (list->capacity == 0)? 4: list->capacity * 2;
        Edge *newEdges = (Edge *)realloc(list->edges, newCapacity * sizeof(Edge));
        if (newEdges == NULL){
            return 0;
        }

        list->edges = newEdges;
        list->capacity = newCapacity;
    }

    list->edges[list->numberOfEdges++] = (Edge){destinationVertex, weight};// add edge to the source vertex
    
    list = &graph->lists[destinationVertex];// get the adjacency list of the destination vertex

    if (list->numberOfEdges >= list->capacity){
        int newCapacity = (list->capacity == 0)? 4: list->capacity * 2;
        Edge *newEdges = (Edge *)realloc(list->edges, newCapacity * sizeof(Edge));
        if (newEdges == NULL){
            return 0;
        }

        list->edges = newEdges;
        list->capacity = newCapacity;
    }

    list->edges[list->numberOfEdges++] = (Edge){sourceVertex, weight};// add edge to the destination vertex

    return 1;// success, 1 - true
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
void searchPath(int sourceVertex, int destinationVertex, Graph *graph) {
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

    while (minHeap->size > 0) {// Extract the vertex with minimum distance value
        MinHeapNode minNode = extractMin(minHeap);
        int u = minNode.vertex;

        for (int i = 0; i < graph->lists[u].numberOfEdges; i++) {
            int v = graph->lists[u].edges[i].destinationVertex;
            int weight = graph->lists[u].edges[i].weight;

            if (distances[u] != INT_MAX && distances[v] > distances[u] + weight) {// here is segmentation fault
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

Graph *update(Graph **graph, int sourceVertex, int destinationVertex, int weight){
    int err = 0;
    if (sourceVertex > (*graph)->numberOfVertices || destinationVertex > (*graph)->numberOfVertices){
        if (firstOutput)
        {
            printf("Update %d %d failed", sourceVertex, destinationVertex);
            firstOutput = 0;
        }else{
            printf("\nUpdate %d %d failed", sourceVertex, destinationVertex);
        }
        return *graph;
    }

    AdjList *sourceList = &(*graph)->lists[sourceVertex];
    AdjList *destinationList = &(*graph)->lists[destinationVertex];

    for(int i = 0; i < sourceList->numberOfEdges; i++){// delete the edge from the source vertex
        if (sourceList->edges[i].destinationVertex == destinationVertex){
            sourceList->edges[i].weight = weight;
        }
    }

    for(int i = 0; i < destinationList->numberOfEdges; i++){// delete the edge from the destination vertex
        if (destinationList->edges[i].destinationVertex == sourceVertex){
            destinationList->edges[i].weight = weight;
        }
    }

    return *graph;
}

Graph* deleteEdge(Graph **graph, int sourceVertex, int destinationVertex){
    int err = 0;
    if (sourceVertex > (*graph)->numberOfVertices || destinationVertex > (*graph)->numberOfVertices){
        if (firstOutput)
        {
            printf("Deletion failed");
            firstOutput = 0;
        }else{
            printf("\nDeletion failed");
        }
        return *graph;
    }else{
        AdjList *sourceList = &(*graph)->lists[sourceVertex];
        AdjList *destinationList = &(*graph)->lists[destinationVertex];

        for(int i = 0; i < sourceList->numberOfEdges; i++){// delete the edge from the source vertex
            if (sourceList->edges[i].destinationVertex == destinationVertex){
                sourceList->numberOfEdges--;
                for (int j = i; j < (sourceList->numberOfEdges); j++)
                {
                    sourceList->edges[j] = sourceList->edges[j + 1];
                }
                break;
            }
        }

        for(int i = 0; i < destinationList->numberOfEdges; i++){// delete the edge from the destination vertex
            if (destinationList->edges[i].destinationVertex == sourceVertex){
                destinationList->numberOfEdges--;
                for (int j = i; j < (destinationList->numberOfEdges); j++)
                {
                    destinationList->edges[j] = destinationList->edges[j + 1];
                }
                break;
            }
        }
        return *graph;
    }
}

int main(void){
    int temp1;
    int temp2;
    Graph *graph;
    char line[50];
    fgets(line, sizeof(line), stdin);
    sscanf(line, "%d %d", &temp1, &temp2);
    graph = initialiseGraph(temp1);
    if (graph == NULL)
    {
        printf("Initialization failed");
    }
    for (int i = 0; i < temp2; i++)
    {
        int source, destination, weight;
        fgets(line, sizeof(line), stdin);
        sscanf(line, "(%d, %d, %d)", &source, &destination, &weight);
        if (!insertEdge(graph, source, destination, weight))
        {
            if (firstOutput)
            {
                printf("Initialisation failed");
                firstOutput = 0;
            }else{
                printf("\nInitialisation failed");
            }
            
            break;
        }
        
    }
    //displayGraph(graph);
    


    while(fgets(line, sizeof(line), stdin) != NULL){
        char opCode;
        sscanf(line, "%c", &opCode);
        int success = 1;

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
                success = insertEdge(graph, temp1, temp2, weight);

            }else{
                success = 0;
            }
            if (!success)
            {
                if (firstOutput)
                {
                    printf("Insertion failed");
                    firstOutput = 0;
                }else{
                    printf("\nInsertion failed");
                }
            }
            //displayGraph(graph);
            break;

        case 'u':
            if (sscanf(line+2, "%d %d %d", &temp1, &temp2, &weight) == 3)
            {
                graph = update(&graph, temp1, temp2, weight);
            }else{
                if (firstOutput)
                {
                    printf("Update failed");
                    firstOutput = 0;
                }else{
                    printf("\nUpdate failed");
                }
            }
            break;

        case 'd':
            if(sscanf(line+2, "%d %d", &temp1, &temp2) == 2)
            {
                graph = deleteEdge(&graph, temp1, temp2);
            }else{
                if (firstOutput)
                {
                    printf("Deletion failed");
                    firstOutput = 0;
                }else{
                    printf("\nDeletion failed");
                }
            }
            break;

        default:
            break;
        }
    }

    return 0;
}