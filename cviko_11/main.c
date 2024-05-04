#include <stdio.h>
#include <stdlib.h>
#define INT_MAX 2147483647

int firstOutput = 1; 

typedef struct edge{
    int source;//source vertex
    int destination;//destination vertex
    int weight;//weight of the edge
    struct edge* next;//pointer to the next edge
} Edge;

typedef struct outputEdge{
    int source;//source vertex
    int destination;//destination vertex
    int weight;//weight of the edge
} OutputEdge;

typedef struct graph{
    int numberOfVertices;//number of vertices
    Edge** edges;//array of pointers to edges
} Graph;

typedef struct heapNode {
    int vertex;  // Vertex number
    int weight;     // Key value used to build the MST
} HeapNode;

typedef struct minHeap{
    HeapNode *elements;
    int size;
    int capacity;
}MinHeap;

void newLineHandler(){
    printf("%s", (firstOutput) ? "" : "\n");
    firstOutput = 0;
}

//-------------------------------- MinHeap functions --------------------------------
/*MinHeap* createMinHeap(int capacity){
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->elements = (Edge**)malloc(capacity * sizeof(Edge*));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    return minHeap;
}

void insertToMinHeap(MinHeap *minHeap, Edge *edge){
    
}

Edge* extractMin(MinHeap *minHeap){
    
}

void freeMinHeap(MinHeap *minHeap){
    free(minHeap->elements);
    free(minHeap);
}
*/
//-------------------------------- QuickSort functions --------------------------------
/**
 * @brief Function to swap two edges
 * 
 * @param a pointer to the first edge
 * @param b pointer to the second edge
*/
void swapEdges(OutputEdge *a, OutputEdge *b){
    OutputEdge temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * @brief Function to find the partition position
 * 
 * @param edges array of edges
 * @param low lower bound of the array
 * @param high upper bound of the array
 * @return int partition point
*/
int findPartition(OutputEdge edges[], int low, int high){
    
    // rightmost element as pivot
    OutputEdge pivot = edges[high];

    // pointer for greater element
    int greater = (low - 1);

    // go through the array and compare the elements with the pivot
    for (int current = low; current < high; current++) {
        if (edges[current].source < pivot.source) {
            greater++;
            swapEdges(&edges[greater], &edges[current]);

        }else if(edges[current].source == pivot.source){
            if(edges[current].destination < pivot.destination){
                greater++;
                swapEdges(&edges[greater], &edges[current]);
            }else if(edges[current].destination == pivot.destination){
                if(edges[current].weight < pivot.weight){
                    greater++;
                    swapEdges(&edges[greater], &edges[current]);
                }
            }
        }
    }

    swapEdges(&edges[greater + 1], &edges[high]);
    return (greater + 1);
}

/**
 * @brief Function to sort the edges
 * 
 * @param edges array of edges
 * @param low lower bound of the array
 * @param high upper bound of the array
*/
void quickSort(OutputEdge edges[], int low, int high){
    if (low < high) {// low = edges[0], high = edges[n-1]

        // find the element where the elements smaller than it are on the left and the elements greater than it are on the right
        int partitionPoint = findPartition(edges, low, high);

        // now call recursively the quickSort function on the left and right of the partition point
        quickSort(edges, low, partitionPoint - 1);// left
        quickSort(edges, partitionPoint + 1, high);// right
    }
}

void printEdges(OutputEdge edges[], int numOfVertices){
    
    newLineHandler();
    long long int cost = 0;
    for (int i = 0; i < numOfVertices; i++) {
        cost += edges[i].weight;
    }
    printf("%lld: ", cost);
    printf("[(%d, %d)", edges[0].source, edges[0].destination);

    for (int i = 1; i < numOfVertices; i++) {
        printf(", (%d, %d)", edges[i].source, edges[i].destination);
    }
    printf("]");
}

//-------------------------------- Graph functions --------------------------------

/**
 * @brief Function to create a graph
 * 
 * @param numberOfVertices number of vertices in the graph
 * @return Graph* pointer to the created graph
*/
Graph* createGraph(int numberOfVertices){
    Graph* graph = (Graph*)malloc(sizeof(Graph));//alloc. memory for the graph
    graph->numberOfVertices = numberOfVertices;// set the number of vertices
    graph->edges = (Edge**)malloc(numberOfVertices * sizeof(Edge*));//alloc. memory for the edges

    for (int i = 0; i < numberOfVertices; i++) {//initialize the edges
        graph->edges[i] = NULL;
    }

    return graph;//return the initialised graph
}

void addEdge(Graph* graph, int source, int destination, int weight){

    Edge *newEdge = (Edge*)malloc(sizeof(Edge));//alloc. memory for the new edge
    newEdge->source = source;//set the source vertex
    newEdge->destination = destination;//set the destination vertex
    newEdge->weight = weight;//set the weight of the edge
    newEdge->next = graph->edges[source];//set the next edge to the current edge
    graph->edges[source] = newEdge;//set the current edge to the new edge

    // Add an edge from destination to source
    newEdge = (Edge*)malloc(sizeof(Edge));//alloc. memory for the new edge
    newEdge->source = destination;//set the source vertex
    newEdge->destination = source;//set the destination vertex
    newEdge->weight = weight;//set the weight of the edge
    newEdge->next = graph->edges[destination];//set the next edge to the current edge
    graph->edges[destination] = newEdge;//set the current edge to the new edge
}

void deleteEdge(Graph* graph, int source, int destination){
    Edge **vertexPointer = &graph->edges[source];//pointer to the pointer to the edge
    Edge *temp;

    while (*vertexPointer)
    {
        if ((*vertexPointer)->destination == destination)
        {
            temp = *vertexPointer;
            *vertexPointer = (*vertexPointer)->next;
            free(temp);
            break;
        }
        vertexPointer = &(*vertexPointer)->next;
    }

    vertexPointer = &graph->edges[destination];//pointer to the pointer to the edge
    while (*vertexPointer)
    {
        if ((*vertexPointer)->destination == source)
        {
            temp = *vertexPointer;
            *vertexPointer = (*vertexPointer)->next;
            break;
        }
        vertexPointer = &(*vertexPointer)->next;
    }
    free(temp);
}

Edge* findEdge(Graph* graph, int source, int destination){
    Edge* currentEdge = graph->edges[source];
    while (currentEdge != NULL) {
        if (currentEdge->destination == destination) {
            return currentEdge;
        }
        currentEdge = currentEdge->next;
    }

    return NULL;
}

void updateEdge(Edge *AtoB_edge, Edge *BtoA_edge, int weight){
    AtoB_edge->weight += weight;
    BtoA_edge->weight += weight;
}

void primSearch(Graph* graph, int sourceVertex){//wrong implementation 
    // Define a data structure to store the key values
    int vertices = graph->numberOfVertices;
    int *key;
    int *parent;
    int *inMST;

    // Initialize first known values
    key = (int*)malloc(vertices * sizeof(int));
    parent = (int*)malloc(vertices * sizeof(int));
    inMST = (int*)calloc(vertices, sizeof(int));

    for (int i = 0; i < vertices; i++)
    {
        key[i] = INT_MAX;
        parent[i] = INT_MAX;
    }

    key[sourceVertex] = 0;// weight of the source vertex is 0
    parent[sourceVertex] = -1;

    for (int count = 0; count < vertices; count++)
    {
        int source = -1;
        for (int i = 0; i < vertices; i++)
        {
            if (!inMST[i] && (source == -1 || key[i] < key[source]))
            {
                source = i;
            }
        }

        // Let's add the vertex to the MST
        inMST[source] = 1;


        // Update key and parent values of the adjacent vertices of the picked vertex.
        for (Edge* currentEdge = graph->edges[source]; currentEdge != NULL; currentEdge = currentEdge->next) {
            int destination = currentEdge->destination;
            int weight = currentEdge->weight;

            if (!inMST[destination] && weight < key[destination])
            {
                parent[destination] = source;
                key[destination] = weight;
            }
        } 

    }
    
    // Edge array to store the edges of the MST
    OutputEdge *edges = (OutputEdge*)malloc((vertices - 1) * sizeof(OutputEdge));
    int edgeIndex = 0;
    for (int i = 0; i < vertices; i++)
    {
        if (parent[i] != -1)
        {
            if (parent[i] != INT_MAX){
                if (parent[i] > i)
                {
                    edges[edgeIndex].source = i;
                    edges[edgeIndex].destination = parent[i];
                    edges[edgeIndex].weight = key[i];
                    edgeIndex++;
                    continue;
                }
                
                edges[edgeIndex].source = parent[i];
                edges[edgeIndex].destination = i;
                edges[edgeIndex].weight = key[i];
                edgeIndex++;
            }
        }
    }

    quickSort(edges, 0, edgeIndex - 1);
    printEdges(edges, edgeIndex);
    // Free the memory
    free(edges);
    free(key);
    free(parent);
    free(inMST);
}

void printGraph(Graph* graph){
    for (int i = 0; i < graph->numberOfVertices; i++) {
        Edge* currentEdge = graph->edges[i];
        printf("Vertex %d: ", i);
        while (currentEdge != NULL) {
            printf("(%d, %d, %d) ", currentEdge->source, currentEdge->destination, currentEdge->weight);
            currentEdge = currentEdge->next;
        }
        printf("\n");
    }
}

int main(void){
    
    Graph* graph;
    int numberOfVertices;

    scanf("%d %d", &numberOfVertices);

    graph = createGraph(numberOfVertices);

    char operation;
    while (scanf(" %c", &operation) != EOF)
    {
        int source, destination, weight;
        int vertex;

        if (operation == '(')
        {
            scanf(" %d, %d, %d)", &source, &destination, &weight);
            if (source >= numberOfVertices || destination >= numberOfVertices || source < 0 || destination < 0 || source == destination)
            {
                newLineHandler();
                printf("insert %d %d failed", source, destination);
            } else{
                addEdge(graph, source, destination, weight);
            }
            continue;
        }
        

        switch (operation)
        {
        case 'i':
            scanf("%d %d %d", &source, &destination, &weight);
            if (source >= numberOfVertices || destination >= numberOfVertices || source < 0 || destination < 0 || source == destination || findEdge(graph, source, destination) != NULL)
            {
                newLineHandler();
                printf("insert %d %d failed", source, destination);
            } else{
                addEdge(graph, source, destination, weight);
            }
            break;
        
        case 'd':
            scanf("%d %d", &source, &destination);
            if (source >= numberOfVertices || destination >= numberOfVertices || source < 0 || destination < 0 || source == destination || findEdge(graph, source, destination) == NULL)
            {
                newLineHandler();
                printf("delete %d %d failed", source, destination);
            } else{
                deleteEdge(graph, source, destination);
            }
            break;

        case 'u':
            scanf("%d %d %d", &source, &destination, &weight);
            if (source >= numberOfVertices || destination >= numberOfVertices || source < 0 || destination < 0 || source == destination || findEdge(graph, source, destination) == NULL)
            {
                newLineHandler();
                printf("update %d %d failed", source, destination);
            } else{
                Edge *AtoB_edge = findEdge(graph, source, destination);
                Edge *BtoA_edge = findEdge(graph, destination, source);
                if (AtoB_edge == NULL || BtoA_edge == NULL)
                {
                    newLineHandler();
                    printf("update %d %d failed", source, destination);
                } else{
                    updateEdge(AtoB_edge, BtoA_edge, weight);
                }
            }
            break;

        case 's':
            scanf(" %d", &vertex);
            if (vertex >= numberOfVertices || vertex < 0 || graph->edges[vertex] == NULL)
            {
                newLineHandler();
                printf("search %d failed", vertex);
                break;
            }  
            
            
            primSearch(graph, vertex);
            break;

        default:
            break;
        }
    }
    

    return 0;
}