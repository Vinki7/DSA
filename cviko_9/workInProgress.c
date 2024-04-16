#include <stdio.h>
#include <stdlib.h>

#define INT_MAX 2147483647

int firstOutput = 1;

typedef struct edge{
    int destinationVertex;
    int weight;
    struct edge *next;
}Edge;


typedef struct graph {
    int numberOfVertices;
    Edge **adjList;  // Array of vertices
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

// --------------------------------------------- Graph manipulation functions ---------------------------------------------

// Add an edge to the graph
void addEdge(Graph *graph, int sourceVertex, int destinationVertex, int weight){
    // Create a new edge from sourceVertex to destinationVertex
    Edge *newEdge = (Edge *)malloc(sizeof(Edge));
    newEdge->destinationVertex = destinationVertex;
    newEdge->weight = weight;
    newEdge->next = graph->adjList[sourceVertex];
    graph->adjList[sourceVertex] = newEdge;

    // Create a new edge from destinationVertex to sourceVertex
    Edge *oppositeEdge = (Edge *)malloc(sizeof(Edge));
    oppositeEdge->destinationVertex = sourceVertex;
    oppositeEdge->weight = weight;
    oppositeEdge->next = graph->adjList[destinationVertex];
    graph->adjList[destinationVertex] = oppositeEdge;
}

Edge *findEdge(Graph *graph, int sourceVertex, int destinationVertex){
    // Find the edge from sourceVertex to destinationVertex    
    Edge *currentEdge = graph->adjList[sourceVertex];
    
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
    if (firstEdgeRep == NULL){// Edge not found
        return 0;
    }
    if((firstEdgeRep->weight + weight)<0){
        return 0;
    }
    Edge *secondEdgeRep = findEdge(graph, destinationVertex, sourceVertex);
    // Update the weight of the edge
    firstEdgeRep->weight += weight;
    secondEdgeRep->weight += weight;
    return 1;
}

// Delete an edge from the graph
void deleteEdge(Graph *graph, int sourceVertex, int destinationVertex){

    Edge *currentEdge = (*graph).adjList[sourceVertex];
    Edge *previousEdge = NULL;
    while (currentEdge != NULL){
        if (currentEdge->destinationVertex == destinationVertex){
            if (previousEdge == NULL){// If the edge is the first edge in the list
                (*graph).adjList[sourceVertex] = currentEdge->next;
            }else{
                previousEdge->next = currentEdge->next;
            }
            free(currentEdge);
            break;   
        }
        previousEdge = currentEdge;
        currentEdge = currentEdge->next;
    }
    
    previousEdge = NULL;
    currentEdge = (*graph).adjList[destinationVertex];
    while (currentEdge != NULL)
    {
        if (currentEdge->destinationVertex == sourceVertex)
        {
            if (previousEdge == NULL)// If the edge is the first edge in the list
            {
                (*graph).adjList[destinationVertex] = currentEdge->next;
            }else{
                previousEdge->next = currentEdge->next;
            }
            free(currentEdge);
            break;
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
    graph->adjList = (Edge **)malloc(numberOfVertices * sizeof(Edge *));

    // Initialise adjacency lists
    for (int i = 0; i < numberOfVertices; i++){
        graph->adjList[i] = NULL;
    }

    return graph;
}

void freeGraph(Graph *graph) {
    if (graph == NULL) return;

    for (int i = 0; i < graph->numberOfVertices; i++) {
        Edge *current = graph->adjList[i];
        while (current) {
            Edge *temp = current;
            current = current->next;
            free(temp);
        }
    }

    free(graph->adjList);
    free(graph);
}


void searchPath(int sourceVertex, int destinationVertex, Graph *graph){
    int numberOfVertices = graph->numberOfVertices;
    int *distances = (int *)malloc(graph->numberOfVertices * sizeof(int));// Array of distances
    int *previousVertices = (int *)malloc(graph->numberOfVertices * sizeof(int));// Array of previous vertices
    int *visited = (int *)calloc(graph->numberOfVertices , sizeof(int));// Array of visited vertices
    int *path = NULL;// Array of vertices in the path

    // Init. distances and heap for all vertices
    for (int v = 0; v < numberOfVertices; v++){
        distances[v] = INT_MAX;// Distance from the source vertex is 0 and infinity for the rest
        previousVertices[v] = -1;// No previous vertices
    }

    distances[sourceVertex] = 0;// Distance from the source vertex is 0
    
    MinHeap *minHeap = createMinHeap(numberOfVertices);// Create a min heap
    minHeap->elements[0].vertex = sourceVertex;// Root is the source vertex
    minHeap->elements[0].distance = distances[sourceVertex];// Distance from the source vertex is 0
    minHeap->sizeOfHeap = 1;// Size of the heap is 1

    while (minHeap->sizeOfHeap > 0){// While there is still at least one element in heap
        // Extract the vertex with minimum distance from the root
        MinHeapNode minHeapNode = extractMin(minHeap);// pop the root
        int u = minHeapNode.vertex;// Vertex with the minimum distance from the extracted vertex
        
        if (u == destinationVertex)
        {
            break;
        }
        
        visited[u] = 1; // Mark the vertex as visited
        // Traverse all adjacent vertices of the extracted vertex
        Edge *currentEdge = graph->adjList[u];// Check first from adjList
        while (currentEdge != NULL)
        {
            int v = currentEdge->destinationVertex;
            if (visited[v] != 0){
                currentEdge = currentEdge->next;
                continue;
            }

            int newCost = distances[u] + currentEdge->weight;
            if (distances[v] > newCost)
            {
                distances[v] = newCost;
                previousVertices[v] = u;
                if (minHeap->sizeOfHeap == minHeap->heapCapacity)
                {
                    minHeap->heapCapacity *= 2;
                    minHeap->elements = (MinHeapNode *)realloc(minHeap->elements, minHeap->heapCapacity * sizeof(MinHeapNode));
                }
                minHeap->elements[minHeap->sizeOfHeap].vertex = v;
                minHeap->elements[minHeap->sizeOfHeap].distance = distances[v];
                minHeap->sizeOfHeap++;

                int i = minHeap->sizeOfHeap - 1;// Index of the inserted element
                while (i != 0 && minHeap->elements[(i - 1) / 2].distance > minHeap->elements[i].distance) {// While the parent is greater than the child
                    swapMinHeapNodes(&minHeap->elements[i], &minHeap->elements[(i - 1) / 2]);
                    i = (i - 1) / 2;// Move to the parent
                }
            }
            currentEdge = currentEdge->next;
        }
        
    }
    int count = 0;
    // Extract path from source to destination
    if (distances[destinationVertex] != INT_MAX) {// if the destination vertex is reachable
        int crawl = destinationVertex;// Start from the destination vertex
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
    return;
}

void displayGraph(Graph *graph) {
    if (!graph) {
        printf("Graph is NULL\n");
        return;
    }

    printf("Graph with %d vertices:\n", graph->numberOfVertices);
    for (int i = 0; i < graph->numberOfVertices; i++) {
        printf("Vertex %d:", i);
        Edge *edge = graph->adjList[i];
        while (edge) {
            printf(" -> %d(weight %d)", edge->destinationVertex, edge->weight);
            edge = edge->next;
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
            if ((sscanf(line+2, "%d %d", &temp1, &temp2) == 2)&&(temp1 >= 0 && temp1 < graph->numberOfVertices)&&(temp2 >= 0 && temp2 < graph->numberOfVertices)&&(temp1!=temp2))
            {
                searchPath(temp1, temp2, graph);
            }else{
                if (firstOutput)
                {
                    printf("search %d %d failed", temp1, temp2);
                    firstOutput = 0;
                }else{
                    printf("\nsearch %d %d failed", temp1, temp2);
                }
            }
            //displayGraph(graph);
            break;
        
        case 'i':
            int weight;
            if ((sscanf(line+2, "%d %d %d", &temp1, &temp2, &weight) == 3)&&(temp1 >= 0 && temp1 < graph->numberOfVertices)&&(temp2 >= 0 && temp2 < graph->numberOfVertices)&&(weight>=0)&&(findEdge(graph, temp1, temp2)==NULL)&&(temp1!=temp2))
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
            if ((sscanf(line+2, "%d %d %d", &temp1, &temp2, &weight) == 3)&&(temp1 >= 0 && temp1 < graph->numberOfVertices)&&(temp2 >= 0 && temp2 < graph->numberOfVertices)&&(temp1!=temp2)&&(findEdge(graph, temp1, temp2)!=NULL))
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
            if((sscanf(line+2, "%d %d", &temp1, &temp2) == 2)&&(temp1 >= 0 && temp1 < graph->numberOfVertices)&&(temp2 >= 0 && temp2 < graph->numberOfVertices)&&(temp1!=temp2)&& findEdge(graph, temp1, temp2)!=NULL)
            {
                deleteEdge(graph, temp1, temp2);
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