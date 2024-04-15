#include <stdio.h>
#include <stdlib.h>

#define INT_MAX 2147483647

int firstOutput = 1;

typedef struct edge{
    int destinationVertex;
    int weight;
    struct edge *next;
}Edge;

typedef struct vertex {
    int id;
    Edge **edges;// Array of edges (adjacency list - hash table)
} Vertex;

typedef struct graph {
    int numberOfVertices;
    Vertex *vertices;  // Array of vertices
} Graph;


typedef struct {
    int vertex;
    int distance;
} MinHeapNode;

typedef struct {
    MinHeapNode *elements;
    int sizeOfHeap;
    int heapCapacity;
} MinHeap;


MinHeap *createMinHeap(int heapCapacity){// Create a min heap - alloc. mem. for the heap and the array of elements
    MinHeap *minHeap = (MinHeap *)malloc(sizeof(MinHeap));
    if (minHeap == NULL){
        return NULL;
    }
    
    minHeap->heapCapacity = heapCapacity;
    minHeap->sizeOfHeap = 0;
    minHeap->elements = (MinHeapNode *)malloc(heapCapacity * sizeof(MinHeapNode));
    if (minHeap->elements == NULL){// Memory alloc. failure
        free(minHeap);
        return NULL;
    }
    return minHeap;
}

void swapMinHeapNodes(MinHeapNode *firstNode, MinHeapNode *secondNode){
    MinHeapNode temp = *firstNode;
    *firstNode = *secondNode;
    *secondNode = temp;
}

void minHeapify(MinHeap *minHeap, int index){
    int smallest = index;// Smallest as root
    int left = 2 * index + 1;// Left child
    int right = 2 * index + 2;// Right child
    // Compare element with left child and right child
    if (left < minHeap->sizeOfHeap && minHeap->elements[left].distance < minHeap->elements[smallest].distance){// I'm not sure about the <= sign
        smallest = left;
    }
    if (right < minHeap->sizeOfHeap && minHeap->elements[right].distance < minHeap->elements[smallest].distance){// If the right child is smaller than the smallest
        smallest = right;// The right becomes the root
    }
    
    if (smallest != index){// If the root is not the smallest
        swapMinHeapNodes(&minHeap->elements[smallest], &minHeap->elements[index]);
        minHeapify(minHeap, smallest);// Recursively call minHeapify
    }
}

MinHeapNode extractMin(MinHeap *minHeap){
    if (minHeap->sizeOfHeap <= 0){
        return (MinHeapNode){-1, INT_MAX};// Return a dummy node - heap is empty
    }
    MinHeapNode root = minHeap->elements[0];// Root is the minimum
    minHeap->elements[0] = minHeap->elements[minHeap->sizeOfHeap - 1];// Replace the root with the last element
    minHeap->sizeOfHeap--;// Decrease the size of the heap
    minHeapify(minHeap, 0);// Heapify the new root
    return root;
}

void decreaseKey(MinHeap *minHeap, int vertex, int distance){
    int i;
    for (i = 0; i < minHeap->sizeOfHeap; i++) {
        if (minHeap->elements[i].vertex == vertex) {// If the vertex is found
            if (minHeap->elements[i].distance > distance) {// If the new distance is smaller
                minHeap->elements[i].distance = distance;
                while (i != 0 && minHeap->elements[(i - 1) / 2].distance > minHeap->elements[i].distance) {// While the parent is greater than the child
                    swapMinHeapNodes(&minHeap->elements[i], &minHeap->elements[(i - 1) / 2]);
                    i = (i - 1) / 2;// Move to the parent
                }
            }
            return;
        }
    }
}

int hashFunction(int vertexId, int numberOfVertices){
    return vertexId % numberOfVertices;
}

void addEdge(Graph *graph, int sourceVertex, int destinationVertex, int weight){
    int index = hashFunction(sourceVertex, graph->numberOfVertices);
    Edge *newEdge = (Edge *)malloc(sizeof(Edge));
    newEdge->destinationVertex = destinationVertex;
    newEdge->weight = weight;
    newEdge->next = graph->vertices[sourceVertex].edges[index];
    graph->vertices[sourceVertex].edges[index] = newEdge;

    // Add the edge in the opposite direction
    index = hashFunction(destinationVertex, graph->numberOfVertices);
    newEdge = (Edge *)malloc(sizeof(Edge));
    newEdge->destinationVertex = sourceVertex;
    newEdge->weight = weight;
    newEdge->next = graph->vertices[destinationVertex].edges[index];
    graph->vertices[destinationVertex].edges[index] = newEdge;
}

Edge *findEdge(Graph *graph, int sourceVertex, int destinationVertex){
    
    int index = hashFunction(sourceVertex, graph->numberOfVertices);
    Edge *currentEdge = graph->vertices[sourceVertex].edges[index];
    
    while (currentEdge != NULL){
        if (currentEdge->destinationVertex == destinationVertex){
            return currentEdge;
        }
        currentEdge = currentEdge->next;
    }
    return NULL;// Edge not found
}

void deleteEdge(Vertex *vertex, int numberOfVertices, int destinationVertex){
    int index = hashFunction(destinationVertex, numberOfVertices);
    Edge *currentEdge = vertex->edges[index];
    Edge *previousEdge = NULL;
    while(currentEdge != NULL){
        if(currentEdge->destinationVertex == destinationVertex){
            if(previousEdge == NULL){
                vertex->edges[index] = currentEdge->next;
            }else{
                previousEdge->next = currentEdge->next;
            }
            free(currentEdge);
            return;
        }
        previousEdge = currentEdge;
        currentEdge = currentEdge->next;
    }
}

Graph *initialiseGraph(int numberOfVertices){
    // Alloc. mem. for graph
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    if (graph == NULL)// memory alloc. failure
    {
        return NULL;
    }

    graph->numberOfVertices = numberOfVertices;
    graph->vertices = (Vertex *)malloc(numberOfVertices * sizeof(Vertex));

    // Initialise adjacency lists
    for (int i = 0; i < numberOfVertices; i++){
        graph->vertices[i].edges = (Edge **)malloc(numberOfVertices * sizeof(Edge *));
        // Handle memory alloc. failure
        if (graph->vertices[i].edges == NULL) {
            // Handle memory allocation failure:
            // Free previously allocated memory before returning NULL
            for (int j = 0; j < i; j++) {
                free(graph->vertices[j].edges);
            }
            free(graph->vertices);
            free(graph);
            return NULL;
        }
        // Init. all edges to NULL
        for (int j = 0; j < numberOfVertices; j++) {
            graph->vertices[i].edges[j] = NULL;
        }
        graph->vertices[i].id = i;
    }

    return graph;
}

void freeGraph(Graph *graph){
    for (int i = 0; i < graph->numberOfVertices; i++)
    {
        for (int j = 0; j < graph->numberOfVertices; i++)
        {
            Edge *currentEdge = graph->vertices[i].edges[j];
            while (currentEdge != NULL)
            {
                Edge *tempEdge = currentEdge;
                currentEdge = currentEdge->next;
                free(tempEdge);
            }
        }
        free(graph->vertices[i].edges);        
    }
    free(graph->vertices);// Free the memory alloc. for the vertices
    free(graph);// Free the memory alloc. for the graph
}


void searchPath(int sourceVertex, int destinationVertex, Graph *graph){
    int numberOfVertices = graph->numberOfVertices;
    int *distances = (int *)malloc(graph->numberOfVertices * sizeof(int));// Array of distances
    int *previousVertices = (int *)malloc(graph->numberOfVertices * sizeof(int));// Array of previous vertices
    int *path = NULL;// Array of vertices in the path
    MinHeap *minHeap = createMinHeap(numberOfVertices);// Create a min heap

    // Init. distances and heap for all vertices
    for (int v = 0; v < numberOfVertices; v++){
        distances[v] = (v == sourceVertex)? 0 : INT_MAX;// Distance from the source vertex is 0 and infinity for the rest
        previousVertices[v] = -1;// No previous vertices
        minHeap->elements[v] = (MinHeapNode){v, distances[v]};// Add the vertex to the heap
        minHeap->sizeOfHeap++;
    }

    // Heapify the MinHeap - direction: bottom-up
    for (int i = (minHeap->sizeOfHeap -2) / 2; i >= 0; i--){// Start from the last non-leaf node
        minHeapify(minHeap, i);
    }

    while (minHeap->sizeOfHeap > 0){// While there is still at least one element in heap
        // Extract the vertex with minimum distance from the root
        MinHeapNode minHeapNode = extractMin(minHeap);
        int u = minHeapNode.vertex;// Vertex with the minimum distance from the extracted vertex

        // Traverse all adjacent vertices of the extracted vertex (u)
        int index = hashFunction(u, numberOfVertices);// Hash table for efficiency
        Edge *currentEdge = graph->vertices[u].edges[index];
        while (currentEdge != NULL){
            int v = currentEdge->destinationVertex;

            // Relaxation
            // If the vertex is still in the heap and the distance is greater than the weight of the edge
            if (findEdge(graph, u, v) != NULL && distances[u] != INT_MAX && currentEdge->weight + distances[u] < distances[v]){
                distances[v] = distances[u] + currentEdge->weight;// Update the distance
                previousVertices[v] = u;// Update the previous vertex
                decreaseKey(minHeap, v, distances[v]);// Decrease the key of the vertex
            }

            currentEdge = currentEdge->next;

        }
    }

    int count = 0;// Count of vertices in the path
    // Extract path from source to destination
    if (distances[destinationVertex] != INT_MAX) {// if the destination vertex is reachable
        int crawl = destinationVertex; // destination vertex
        while (crawl != -1) {
            count++;
            crawl = previousVertices[crawl];
        }
        path = (int *)malloc(count * sizeof(int));
        crawl = destinationVertex;
        for (int i = count - 1; i >= 0; --i) {
            path[i] = crawl;
            crawl = previousVertices[crawl];
        }
    }

    free(distances);
    free(previousVertices);
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

void displayGraph(Graph *graph) {
    if (!graph) {
        printf("Graph is NULL\n");
        return;
    }

    printf("Graph with %d vertices:\n", graph->numberOfVertices);
    for (int i = 0; i < graph->numberOfVertices; i++) {
        printf("Vertex %d:", graph->vertices[i].id);

        // Iterate over each bucket in the hash table
        for (int j = 0; j < graph->numberOfVertices; j++) {
            Edge *edge = graph->vertices[i].edges[j];
            while (edge) {
                printf(" -> %d(weight %d)", edge->destinationVertex, edge->weight);
                edge = edge->next;
            }
        }
        printf("\n");
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
        if (source < 0 || source >= graph->numberOfVertices || destination < 0 || destination >= graph->numberOfVertices)
        {
            if (firstOutput)
            {
                printf("Initialisation failed");
                firstOutput = 0;
            }else{
                printf("\nInitialisation failed");
            }
            
            break;
        }else{
            addEdge(graph, source, destination, weight);
        }
        
    }
    displayGraph(graph);

    return 0;
}