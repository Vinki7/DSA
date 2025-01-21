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
    Vertex *adjList;  // Array of vertices
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

// --------------------------------------------- Min heap functions ---------------------------------------------
MinHeap *createPriorQueue(int heapCapacity){// Create a min heap - alloc. mem. for the heap and the array of elements
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

// Hash function for the vertices (adjacency lists represented as hash tables for efficiency)
int hashFunction(int vertexId, int numberOfVertices){
    //return vertexId % numberOfVertices;
    return vertexId;
}

// --------------------------------------------- Graph manipulation functions ---------------------------------------------

// Add an edge to the graph
void addEdge(Graph *graph, int sourceVertex, int destinationVertex, int weight){
    int index = hashFunction(destinationVertex, graph->numberOfVertices);
    Edge *newEdge = (Edge *)malloc(sizeof(Edge));
    newEdge->destinationVertex = destinationVertex;
    newEdge->weight = weight;
    newEdge->next = graph->adjList[sourceVertex].edges[index];
    graph->adjList[sourceVertex].edges[index] = newEdge;

    // Add the edge in the opposite way
    index = hashFunction(sourceVertex, graph->numberOfVertices);
    newEdge = (Edge *)malloc(sizeof(Edge));
    newEdge->destinationVertex = sourceVertex;
    newEdge->weight = weight;
    newEdge->next = graph->adjList[destinationVertex].edges[index];
    graph->adjList[destinationVertex].edges[index] = newEdge;
}

Edge *findEdge(Graph *graph, int sourceVertex, int destinationVertex){
    
    int index = hashFunction(destinationVertex, graph->numberOfVertices);
    Edge *currentEdge = graph->adjList[sourceVertex].edges[index];
    
    while (currentEdge != NULL){
        if (currentEdge->destinationVertex == destinationVertex){
            return currentEdge;
        }
        currentEdge = currentEdge->next;
    }
    return NULL;// Edge not found
}

int updateEdge(Graph *graph, int sourceVertex, int destinationVertex, int weight){
    int numberOfVertices = graph->numberOfVertices;
    Edge *firstEdgeRep = findEdge(graph, sourceVertex, destinationVertex);
    Edge *secondEdgeRep = findEdge(graph, destinationVertex, sourceVertex);
    if ((firstEdgeRep == NULL) || (secondEdgeRep == NULL)){// Edge not found
        return 0;
    }
    if((firstEdgeRep->weight + weight)<0 || (secondEdgeRep->weight + weight) < 0){
        return 0;
    }
    firstEdgeRep->weight += weight;
    secondEdgeRep->weight += weight;
    return 1;
}

// Delete an edge from the graph
int deleteEdge(Vertex *vertex, int numberOfVertices, int destinationVertex){
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
            return 1;
        }
        previousEdge = currentEdge;
        currentEdge = currentEdge->next;
    }
    return 0;
}

Graph *initialiseGraph(int numberOfVertices){
    // Alloc. mem. for graph
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    if (graph == NULL)// memory alloc. failure
    {
        return NULL;
    }

    graph->numberOfVertices = numberOfVertices;
    graph->adjList = (Vertex *)malloc(numberOfVertices * sizeof(Vertex));

    // Initialise adjacency lists
    for (int i = 0; i < numberOfVertices; i++){
        graph->adjList[i].edges = (Edge **)malloc(numberOfVertices * sizeof(Edge *));
        // Handle memory alloc. failure
        if (graph->adjList[i].edges == NULL) {
            // Handle memory alloc. failure:
            // Free previously alloc. memory before returning NULL
            for (int j = 0; j < i; j++) {
                free(graph->adjList[j].edges);
            }
            free(graph->adjList);
            free(graph);
            return NULL;
        }
        // Init. all edges to NULL
        for (int j = 0; j < numberOfVertices; j++) {
            graph->adjList[i].edges[j] = NULL;
        }
        graph->adjList[i].id = i;
    }

    return graph;
}

void freeGraph(Graph *graph) {
    if (graph == NULL) {
        return; // Prevent NULL pointer dereference - segfault
    }

    // Loop through each vertex
    for (int i = 0; i < graph->numberOfVertices; i++) {
        // Check if the vertex has alloc. edge lists
        if (graph->adjList[i].edges != NULL) {
            // Loop through each bucket in the hash table of edges
            for (int j = 0; j < graph->numberOfVertices; j++) { // Assuming the hash table size is numberOfVertices
                Edge *currentEdge = graph->adjList[i].edges[j];
                // Free all edges in the linked list at this bucket
                while (currentEdge != NULL) {
                    Edge *tempEdge = currentEdge;
                    currentEdge = currentEdge->next;
                    free(tempEdge);
                }
            }
            // Free the array of edge pointers for this vertex
            free(graph->adjList[i].edges);
        }
    }

    // Free the array of vertices
    free(graph->adjList);
    // Finally, free the graph structure itself
    free(graph);
}


void searchPath(int sourceVertex, int destinationVertex, Graph *graph){
    int numberOfVertices = graph->numberOfVertices;
    int *distances = (int *)malloc(graph->numberOfVertices * sizeof(int));// Array of distances
    int *previousVertices = (int *)malloc(graph->numberOfVertices * sizeof(int));// Array of previous vertices
    int *path = NULL;// Array of vertices in the path
    MinHeap *minHeap = createPriorQueue(numberOfVertices);// Create a min heap

    // Init. distances and heap for all vertices
    for (int v = 0; v < numberOfVertices; v++){
        distances[v] = (v == sourceVertex)? 0 : INT_MAX;// Distance from the source vertex is 0 and infinity for the rest
        previousVertices[v] = -1;// No previous vertices
        minHeap->elements[v] = (MinHeapNode){v, distances[v]};// Add the vertex to the heap
        minHeap->sizeOfHeap++;
    }

    // Heapify the MinHeap - bottom-up
    for (int i = (minHeap->sizeOfHeap -2) / 2; i >= 0; i--){// Start from the last non-leaf node
        minHeapify(minHeap, i);
    }

    while (minHeap->sizeOfHeap > 0){// While there is still at least one element in heap
        // Extract the vertex with minimum distance from the root
        MinHeapNode minHeapNode = extractMin(minHeap);
        int u = minHeapNode.vertex;// Vertex with the minimum distance from the extracted vertex

        // Traverse all buckets in the adjacency list for vertex u
        for (int i = 0; i < numberOfVertices; i++){
            Edge *currentEdge = graph->adjList[u].edges[i];
            while (currentEdge != NULL){
                int v = currentEdge->destinationVertex;

                if (distances[u] != INT_MAX && distances[v] > distances[u] + currentEdge->weight){
                    distances[v] = distances[u] + currentEdge->weight;
                    previousVertices[v] = u;
                    decreaseKey(minHeap, v, distances[v]);
                }
                currentEdge = currentEdge->next;
            }
        }
    }
    int count = 0;
    // Extract path from source to destination
    if (distances[destinationVertex] != INT_MAX) {// if the destination vertex is reachable
        int crawl = destinationVertex;
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
    }else{// if the destination vertex is not reachable
        if (firstOutput)
        {
            printf("search %d %d failed", sourceVertex, destinationVertex);
            firstOutput = 0;
        }else{
            printf("\nsearch %d %d failed", sourceVertex, destinationVertex);
        }
        free(distances);
        free(previousVertices);
        free(minHeap->elements);
        free(minHeap);
        free(path);
        return;
    }
    for (int i = 0; i < count; i++)
    {
        if (firstOutput)
        {
            printf("%d: [%d", distances[destinationVertex], path[i]);
            firstOutput = 0;
        }else if (!i)
        {
            printf("\n%d: [%d", distances[destinationVertex], path[i]);
        }else{
            printf(", %d", path[i]);
        }   
    }
    printf("]");
    
    free(distances);
    free(previousVertices);
    free(minHeap->elements);
    free(minHeap);
    free(path);
}

void displayGraph(Graph *graph) {
    if (!graph) {
        printf("Graph is NULL\n");
        return;
    }

    printf("Graph with %d vertices:\n", graph->numberOfVertices);
    for (int i = 0; i < graph->numberOfVertices; i++) {
        printf("Vertex %d:", graph->adjList[i].id);

        // Iterate over each bucket in the hash table
        for (int j = 0; j < graph->numberOfVertices; j++) {
            Edge *edge = graph->adjList[i].edges[j];
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
        printf("initialization failed");
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
                printf("initialisation failed");
                firstOutput = 0;
            }else{
                printf("\ninitialisation failed");
            }
            
            break;
        }else{
            addEdge(graph, source, destination, weight);
        }
        
    }

    while(fgets(line, sizeof(line), stdin) != NULL){
        char opCode;
        sscanf(line, "%c", &opCode);
        int success = 1;

        switch (opCode)
        {
        case 's':
            if ((sscanf(line+2, "%d %d", &temp1, &temp2) == 2)&&(temp1 >= 0 && temp1 < graph->numberOfVertices)&&(temp2 >= 0 && temp2 < graph->numberOfVertices)&&(temp1!=temp2))            {
                searchPath(temp1, temp2, graph);
            }else{
                if (firstOutput)
                {
                    printf("search failed", temp1, temp2);
                    firstOutput = 0;
                }else{
                    printf("\nsearch failed", temp1, temp2);
                }
            }
            //displayGraph(graph);
            break;
        
        case 'i':
            int weight;
            if ((sscanf(line+2, "%d %d %d", &temp1, &temp2, &weight) == 3)&&(temp1 >= 0 && temp1 < graph->numberOfVertices)&&(temp2 >= 0 && temp2 < graph->numberOfVertices)&&(temp1!=temp2)&&(weight>=0)&&(findEdge(graph, temp1, temp2)==NULL)&&(temp1!=temp2))
            {
                addEdge(graph, temp1, temp2, weight);

            }else{
                if (firstOutput)
                {
                    printf("insert %d %d failed", temp1, temp2);
                    firstOutput = 0;
                }else{
                    printf("\ninsert %d %d failed", temp1, temp2);
                }
            }
            //displayGraph(graph);
            break;

        case 'u':
            if ((sscanf(line+2, "%d %d %d", &temp1, &temp2, &weight) == 3)&&(temp1 >= 0 && temp1 < graph->numberOfVertices)&&(temp2 >= 0 && temp2 < graph->numberOfVertices)&&(temp1!=temp2))
            {
                if (!updateEdge(graph, temp1, temp2, weight))
                {
                    if (firstOutput)
                    {
                        printf("update %d %d failed", temp1, temp2);
                        firstOutput = 0;
                    }else{
                        printf("\nupdate %d %d failed", temp1, temp2);
                    }
                }
            }else{
                if (firstOutput)
                {
                    printf("update %d %d failed", temp1, temp2);
                    firstOutput = 0;
                }else{
                    printf("\nupdate %d %d failed", temp1, temp2);
                }
            }
            //displayGraph(graph);
            break;

        case 'd':
            int success = 1;
            if((sscanf(line+2, "%d %d", &temp1, &temp2) == 2)&&(temp1 >= 0 && temp1 < graph->numberOfVertices)&&(temp2 >= 0 && temp2 < graph->numberOfVertices)&&(temp1!=temp2))
            {
                if (!(deleteEdge(&graph->adjList[temp1], graph->numberOfVertices, temp2) && deleteEdge(&graph->adjList[temp2], graph->numberOfVertices, temp1))){
                    if (firstOutput)
                    {
                        printf("delete %d %d failed", temp1, temp2);
                        firstOutput = 0;
                    }else{
                        printf("\ndelete %d %d failed", temp1, temp2);
                    }
                }
                
            }else{
                if (firstOutput)
                {
                    printf("delete %d %d failed", temp1, temp2);
                    firstOutput = 0;
                }else{
                    printf("\ndelete %d %d failed", temp1, temp2);
                }
            }
            //displayGraph(graph);
            break;

        default:
            break;
        }
    }

    freeGraph(graph);
    return 0;
}