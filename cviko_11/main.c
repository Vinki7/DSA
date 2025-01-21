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

typedef struct sortNode{
    int source;//source vertex
    int destination;//destination vertex
    int weight;//weight of the edge
} SortNode;

typedef struct graph{
    int numberOfVertices;//number of vertices
    Edge** edges;//array of pointers to edges
} Graph;

// -------------------------------- Priority Queue structure --------------------------------

// Node structure for the priority queue
typedef struct queueNode {
    int vertex;  // Vertex number
    int weight;  // Key value used to build the MST
} QueueNode;

// Priority queue structure
typedef struct priorQueue{
    QueueNode *elements;
    int size;
    int capacity;
    int *position;
}PriorQueue;

void newLineHandler(){
    printf("%s", (firstOutput) ? "" : "\n");
    firstOutput = 0;
}

// -------------------------------- Priority Queue functions --------------------------------
PriorQueue* createPriorQueue(int capacity);
void swapNodes(QueueNode* a, QueueNode* b);
void insertPriorQueue(PriorQueue* queue, int vertex, int key);
QueueNode extractMin(PriorQueue* heap);
void minHeapify(PriorQueue* heap, int index);
void decreaseKey(PriorQueue* heap, int vertex, int key);
int isInPriorQueue(PriorQueue* heap, int vertex);

// Function to create a priority queue
PriorQueue* createPriorQueue(int capacity){
    PriorQueue* queue = (PriorQueue*)malloc(sizeof(PriorQueue));
    queue->elements = (QueueNode*)malloc(capacity * sizeof(QueueNode));
    queue->size = 0;
    queue->capacity = capacity;
    queue->position = (int*)malloc(capacity * sizeof(int));
    return queue;
}

void swapNodes(QueueNode* a, QueueNode* b){
    QueueNode temp = *a;
    *a = *b;
    *b = temp;
}

void insertPriorQueue(PriorQueue* queue, int vertex, int key) {
    if (queue->size == queue->capacity) {
        queue->capacity *= 2;
        queue->elements = (QueueNode*)realloc(queue->elements, queue->capacity * sizeof(QueueNode));
    }

    int i = queue->size;
    queue->size++;
    queue->elements[i].vertex = vertex;
    queue->elements[i].weight = key;
    queue->position[vertex] = i;

    while (i != 0 && queue->elements[(i - 1) / 2].weight > queue->elements[i].weight) {
        swapNodes(&queue->elements[i], &queue->elements[(i - 1) / 2]);
        queue->position[queue->elements[i].vertex] = i;
        queue->position[queue->elements[(i - 1) / 2].vertex] = (i - 1) / 2;
        i = (i - 1) / 2;
    }

}


QueueNode extractMin(PriorQueue *queue) {
    if (queue->size <= 0) {
        return (QueueNode) {INT_MAX, INT_MAX}; // Empty heap
    }

    if (queue->size == 1) {
        queue->size--;
        return queue->elements[0];
    }

    QueueNode root = queue->elements[0];
    queue->elements[0] = queue->elements[queue->size - 1];
    queue->position[root.vertex] = queue->size - 1;
    queue->size--;
    minHeapify(queue, 0);

    return root;
}

void minHeapify(PriorQueue *queue, int index) {
    int minimum = index;
    int leftChild = 2 * index + 1;
    int rightChild = 2 * index + 2;

    if (leftChild < queue->size && queue->elements[leftChild].weight < queue->elements[minimum].weight) {
        minimum = leftChild;
    }
    if (rightChild < queue->size && queue->elements[rightChild].weight < queue->elements[minimum].weight) {
        minimum = rightChild;
    }
    
    // Swap the nodes if the minimum is not the root
    if (minimum != index) {
        QueueNode minimumNode = queue->elements[minimum];
        QueueNode indexNode = queue->elements[index];

        queue->position[minimumNode.vertex] = index;
        queue->position[indexNode.vertex] = minimum;

        swapNodes(&queue->elements[minimum], &queue->elements[index]);
        minHeapify(queue, minimum);
    }
    
}

void decreaseKey(PriorQueue *queue, int vertex, int newKey) {
    int i = queue->position[vertex];
    queue->elements[i].weight = newKey;

    while (i != 0 && queue->elements[(i - 1) / 2].weight > queue->elements[i].weight) {
        swapNodes(&queue->elements[i], &queue->elements[(i - 1) / 2]);
        queue->position[queue->elements[i].vertex] = i;
        queue->position[queue->elements[(i - 1) / 2].vertex] = (i - 1) / 2;
        i = (i - 1) / 2;
    }
}

int isInPriorQueue(PriorQueue *queue, int vertex) {
    if (queue->position[vertex] < queue->size) {
        return 1;
    }
    return 0;
}

void freePriorQueue(PriorQueue *queue){
    free(queue->elements);
    free(queue->position);
    free(queue);
}
//-------------------------------- QuickSort functions --------------------------------
/**
 * @brief Function to swap two edges
 * 
 * @param a pointer to the first edge
 * @param b pointer to the second edge
*/
void swapEdges(SortNode *a, SortNode *b){
    SortNode temp = *a;
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
int findPartition(SortNode edges[], int low, int high){
    
    // rightmost element as pivot
    SortNode pivot = edges[high];

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
void quickSort(SortNode edges[], int low, int high){
    if (low < high) {// low = edges[0], high = edges[n-1]

        // find the element where the elements smaller than it are on the left and the elements greater than it are on the right
        int partitionPoint = findPartition(edges, low, high);

        // now call recursively the quickSort function on the left and right of the partition point
        quickSort(edges, low, partitionPoint - 1);// left
        quickSort(edges, partitionPoint + 1, high);// right
    }
}

void printEdges(SortNode edges[], int numOfVertices){
    
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
    
    int vertices = graph->numberOfVertices;

    // Initialize the priority queue
    PriorQueue* queue = createPriorQueue(vertices);
    int *parent = (int*)malloc(vertices * sizeof(int));
    int *key = (int*)malloc(vertices * sizeof(int));
    for (int i = 0; i < vertices; i++) {
        parent[i] = -1;
        key[i] = INT_MAX;
    }    
    int *inMST = (int*)calloc(vertices, sizeof(int));

    // Insert the source vertex into the priority queue
    // key value is 0
    key[sourceVertex] = 0;
    insertPriorQueue(queue, sourceVertex, key[sourceVertex]);

    while (queue->size != 0) {
        // Extract the vertex with the minimum key
        QueueNode extractedNode = extractMin(queue);
        inMST[extractedNode.vertex] = 1;

        // Traverse through all adj. vertices of extracted vertex and update the keys
        for (Edge* edge = graph->edges[extractedNode.vertex]; edge != NULL; edge = edge->next) {
            int destination = edge->destination;
            int weight = edge->weight;
            
            // If the destination vertex is not in MST and the weight of the edge is less than the key value
            if (inMST[destination] == 0 && weight < key[destination]) {
                key[edge->destination] = weight;
                parent[destination] = extractedNode.vertex;
                insertPriorQueue(queue, destination, key[destination]);
                decreaseKey(queue, destination, key[destination]);
            }
        }
    }
    
    SortNode *edges = (SortNode*)malloc((vertices - 1) * sizeof(SortNode));
    int edgeIndex = 0;
    for (int i = 0; i < vertices; i++)
    {
        if (inMST[i] == 0)
        {
            continue;
        }
        
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
    free(inMST);
    free(parent);
    free(key);
    freePriorQueue(queue);
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
                if (AtoB_edge == NULL || AtoB_edge->weight + weight < 0)
                {
                    newLineHandler();
                    printf("update %d %d failed", source, destination);
                } else{
                    Edge *BtoA_edge = findEdge(graph, destination, source);
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