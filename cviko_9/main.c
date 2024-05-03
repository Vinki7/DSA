#include <stdio.h>
#include <stdlib.h>

#define INT_MAX 2147483646// Represents infinity
int firstOutput = 1;// Flag for the first output

typedef struct node {
    int destinationVertex;
    int weight;
    struct node* next;
} Node;

typedef struct {
    int numberOfVertices;
    Node** adjList;// Adjacency list
} Graph;

typedef struct priorityQueue {
    int capacity;
    int size;
    int *elements;  
    int *distance;  
    int *position;  
} PriorityQueue;

int validateVertex(Graph* graph, int vertex) {
    return vertex >= 0 && vertex < graph->numberOfVertices;
}

// ---------------------------- Priority Queue ----------------------------
// Initialize PriorityQueue
PriorityQueue* createPriorityQueue(int capacity) {
    PriorityQueue *priorityQ = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    priorityQ->capacity = capacity;
    priorityQ->size = 0;
    priorityQ->elements = (int*)malloc(sizeof(int) * capacity);
    priorityQ->distance = (int*)malloc(sizeof(int) * capacity);
    priorityQ->position = (int*)malloc(sizeof(int) * capacity);
    for (int i = 0; i < capacity; i++) {
        priorityQ->position[i] = -1;  // Initialize positions as -1
    }
    return priorityQ;
}


// Swap two nodes in the heap
void swapNodes(PriorityQueue *priorityQ, int first, int second) {
    // Swap elements
    int temp = priorityQ->elements[first];
    priorityQ->elements[first] = priorityQ->elements[second];
    priorityQ->elements[second] = temp;

    // Swap distances
    temp = priorityQ->distance[first];
    priorityQ->distance[first] = priorityQ->distance[second];
    priorityQ->distance[second] = temp;

    // Swap positions
    priorityQ->position[priorityQ->elements[first]] = first;
    priorityQ->position[priorityQ->elements[second]] = second;
}


// Insert a new element into the heap
void insertIntoPriorityQueue(PriorityQueue *priorityQ, int vertex, int dist) {
    if (priorityQ->size == priorityQ->capacity) return;

    // Insert the new element at the end of the heap
    int i = priorityQ->size;
    priorityQ->elements[i] = vertex;
    priorityQ->distance[i] = dist;
    priorityQ->position[vertex] = i;
    priorityQ->size++;

    // Fix the min heap property if it is violated
    while (i != 0 && priorityQ->distance[(i - 1) / 2] > priorityQ->distance[i]) {
        swapNodes(priorityQ, i, (i - 1) / 2);
        i = (i - 1) / 2;
    }
}


// Heapify the heap - top-down
void heapify(PriorityQueue *priorityQ, int i) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < priorityQ->size && priorityQ->distance[left] < priorityQ->distance[smallest]) {
        smallest = left;
    }
    if (right < priorityQ->size && priorityQ->distance[right] < priorityQ->distance[smallest]) {
        smallest = right;
    }

    if (smallest != i) {
        swapNodes(priorityQ, i, smallest);
        heapify(priorityQ, smallest);
    }
}


// Min heapify - bottom-up
void minHeapify(PriorityQueue *priorityQ, int i) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < priorityQ->size && priorityQ->distance[left] < priorityQ->distance[smallest]) {
        smallest = left;
    }
    if (right < priorityQ->size && priorityQ->distance[right] < priorityQ->distance[smallest]) {
        smallest = right;
    }

    if (smallest != i) {
        swapNodes(priorityQ, i, smallest);
        minHeapify(priorityQ, smallest);
    }
}


int extractMin(PriorityQueue *priorityQ) {
    if (priorityQ->size <= 0) return -1;

    int root = priorityQ->elements[0];
    priorityQ->size--;
    priorityQ->elements[0] = priorityQ->elements[priorityQ->size];
    priorityQ->distance[0] = priorityQ->distance[priorityQ->size];
    priorityQ->position[priorityQ->elements[0]] = 0;
    priorityQ->position[root] = -1; // This vertex is no longer in the heap

    heapify(priorityQ, 0);

    return root;
}


int isEmpty(PriorityQueue *priorityQ) {
    return priorityQ->size == 0;
}


void decreaseKey(PriorityQueue *priorityQ, int vertex, int dist) {
    int i = priorityQ->position[vertex];
    if (i == -1 || priorityQ->distance[i] <= dist) return;  // No update needed if new distance is not better

    priorityQ->distance[i] = dist;

    while (i != 0 && priorityQ->distance[(i - 1) / 2] > priorityQ->distance[i]) {
        priorityQ->position[priorityQ->elements[i]] = (i - 1) / 2;
        priorityQ->position[priorityQ->elements[(i - 1) / 2]] = i;
        swapNodes(priorityQ, i, (i - 1) / 2);
        i = (i - 1) / 2;
    }
}


void freePriorityQueue(PriorityQueue *priorityQ) {
    free(priorityQ->elements);
    free(priorityQ->distance);
    free(priorityQ->position);
    free(priorityQ);
}


// Create a graph
Graph* initialiseGraph(int vertices) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));// Alloc. mem. for graph
    if (graph == NULL) return NULL;// Alloc. failed
    graph->numberOfVertices = vertices;
    graph->adjList = (Node**)malloc(vertices * sizeof(Node*));
    if (graph->adjList == NULL) { 
        free(graph);
        return NULL;
    }

    for (int i = 0; i < vertices; i++) {
        graph->adjList[i] = NULL;
    }
    return graph;
}


// ---------------------------- Add an edge to the graph ----------------------------
void addEdge(Graph* graph, int source, int destination, int weight) {
    if (source < 0 || source > graph->numberOfVertices || destination < 0 || destination > graph->numberOfVertices || weight < 0 || source == destination) {
        printf(firstOutput ? "insert %d %d failed" : "\ninsert %d %d failed", source, destination);
        firstOutput = 0;
        return;
    }
    // Check if the edge already exists
    Node* currentNode = graph->adjList[source];
    while (currentNode != NULL) {
        if (currentNode->destinationVertex == destination) {
            printf(firstOutput ? "insert %d %d failed" : "\ninsert %d %d failed", source, destination);
            firstOutput = 0;
            return;
        }
        currentNode = currentNode->next;
    }
    // Create a new nodes for source and destination
    Node* newSourceNode = (Node*)malloc(sizeof(Node));
    Node* newDestinationNode = (Node*)malloc(sizeof(Node));
    if (!newSourceNode || !newDestinationNode) {
        printf(firstOutput ? "insert %d %d failed" : "\ninsert %d %d failed", source, destination);
        firstOutput = 0;
        return;
    }
    // Add an edge from source to destination
    newSourceNode->destinationVertex = destination;
    newSourceNode->weight = weight;
    newSourceNode->next = graph->adjList[source];
    graph->adjList[source] = newSourceNode;
    // Add an edge from destination to source
    newDestinationNode->destinationVertex = source;
    newDestinationNode->weight = weight;
    newDestinationNode->next = graph->adjList[destination];
    graph->adjList[destination] = newDestinationNode;
}


//----------------------------- Dijkstra's algorithm ---------------------------- 

void printPath(int parent[], int j) {
    if (parent[j] == -1)
        return;
    printPath(parent, parent[j]);
    printf(", %d", j);
}


void outputSolution(int dist[], int n, int parent[], int src, int target) {
    int src_vertex = src;
    if (dist[target] == INT_MAX) {
       printf(firstOutput ? "search %d %d failed" : "\nsearch %d %d failed", src, target);
        firstOutput = 0;
        return;
        
    } else {
        if (firstOutput) {
            printf("%d: [%d", dist[target], src);
            firstOutput = 0;  
        } else {
            printf("%d: [%d", dist[target], src);
        }
        printPath(parent, target);
        printf("]");
    }
}


// Dijkstra's algorithm
void searchPath(Graph* graph, int sourceVertex, int destinationVertex) {
    if (!validateVertex(graph, sourceVertex) || !validateVertex(graph, destinationVertex)) {// Check if the vertices are valid
        printf(firstOutput ? "search %d %d failed" : "\nsearch %d %d failed", sourceVertex, destinationVertex);
        firstOutput = 0;
        return;
    }

    int V = graph->numberOfVertices;
    //int dist[V], parent[V];
    int *distance = (int*)malloc(V * sizeof(int));
    int *parent = (int*)malloc(V * sizeof(int));
    PriorityQueue *priorityQ = createPriorityQueue(V);// Create a priority queue

    // Initialize the priority queue and the arrays
    for (int i = 0; i < V; i++) {
        distance[i] = INT_MAX;
        parent[i] = -1;
        insertIntoPriorityQueue(priorityQ, i, distance[i]);
    }

    distance[sourceVertex] = 0;
    decreaseKey(priorityQ, sourceVertex, distance[sourceVertex]);  // Update the distance of source in priority queue

    while (!isEmpty(priorityQ)) {
        int u = extractMin(priorityQ);
        if (u == -1 || distance[u] == INT_MAX) break; // Stop if no valid vertex or unreachable vertex

        Node* temp = graph->adjList[u];
        while (temp != NULL) {
            int v = temp->destinationVertex;
            int weight = temp->weight;
            if (distance[u] + weight < distance[v]) { // Relax the edge (u, v)
                distance[v] = distance[u] + weight;
                parent[v] = u;
                decreaseKey(priorityQ, v, distance[v]); // Update the distance in priority queue
            }
            temp = temp->next;
        }
    }

    freePriorityQueue(priorityQ);

    if (distance[destinationVertex] == INT_MAX) {
        printf(firstOutput ? "search %d %d failed" : "\nsearch %d %d failed", sourceVertex, destinationVertex);
        firstOutput = 0;
    } else {
        if (!firstOutput) printf("\n");
        outputSolution(distance, V, parent, sourceVertex, destinationVertex);
        firstOutput = 0;
    }
    free(distance);
    free(parent);
}


// ---------------------------- Remove edge from the graph ----------------------------
// Remove an edge from the adjacency list - helper function
int removeEdge(Node** adjList, int src, int destinationVertex) {
    Node **ptr = &adjList[src], *temp;
    while (*ptr) {
        if ((*ptr)->destinationVertex == destinationVertex) {
            temp = *ptr;
            *ptr = (*ptr)->next;
            free(temp);
            return 1;
        }
        ptr = &((*ptr)->next);
    }
    return 0;
}


// Delete an edge from the graph
void deleteEdge(Graph* graph, int src, int dest) {
    if (src < 0 || src >= graph->numberOfVertices || dest < 0 || dest >= graph->numberOfVertices) {
        printf(firstOutput ? "delete %d %d failed" : "\ndelete %d %d failed", src, dest);
        firstOutput = 0;
        return;
    }
    // Usage of helper functions
    int found = removeEdge(graph->adjList, src, dest);// Remove the edge from source to destination
    found += removeEdge(graph->adjList, dest, src);// Remove the edge from destination to source

    if (found < 2) {  // If either of the ected edges wasn't found
        printf(firstOutput ? "delete %d %d failed" : "\ndelete %d %d failed", src, dest);
        firstOutput = 0;
    }
}


// ---------------------------- Update the weight of an edge in the graph ----------------------------
// Update the weight of an edge in the graph
int checkAndUpdate(Node* list, int target, int change) {
    Node* temp = list;
    // Traverse the list
    while (temp != NULL) {
        if (temp->destinationVertex == target) {
            if (temp->weight + change < 0) {  
                return 0;
            }
            temp->weight += change;
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}


void updateEdge(Graph* graph, int source, int destination, int affection) {
    if (source < 0 || source > graph->numberOfVertices || destination < 0 || destination >= graph->numberOfVertices) {
        printf(firstOutput ? "update %d %d failed" : "\nupdate %d %d failed", source, destination);
        firstOutput = 0;
        return;
    }

    if (!checkAndUpdate(graph->adjList[source], destination, affection) || !checkAndUpdate(graph->adjList[destination], source, affection)) {
        printf(firstOutput ? "update %d %d failed" : "\nupdate %d %d failed", source, destination);
        firstOutput = 0;
    }
}


// ---------------------------- Assisting functions ----------------------------
// Print the graph
void printGraph(Graph* graph) {
    for (int v = 0; v < graph->numberOfVertices; v++) {
        Node* temp = graph->adjList[v];
        printf("\n Vertex %d:\n head", v);
        while(temp) {
            printf(" -> %d(%d)", temp->destinationVertex, temp->weight);
            temp = temp->next;
        }
        printf("\n");
    }
}


void freeGraph(Graph* graph) {
    for (int i = 0; i < graph->numberOfVertices; i++) {
        Node* temp = graph->adjList[i];
        while (temp != NULL) {
            Node* toDelete = temp;
            temp = temp->next;
            free(toDelete);
        }
    }
    free(graph->adjList);
    free(graph);
}


// ---------------------------- Main function ----------------------------
int main() {
    int N, M;
    scanf(" %d %d", &N, &M);
    Graph *graph = initialiseGraph(N);

    for (int i = 0; i < M; i++) {
        int from, to, weight;
        scanf(" (%d, %d, %d)", &from, &to, &weight); 
        addEdge(graph, from, to, weight);
    }

    printGraph(graph);
    char operation;
    while (scanf(" %c", &operation) != EOF) {
        int temp1, temp2, weight;
        switch (operation) {
            case 'i':// Insert
                if (scanf(" %d %d %d", &temp1, &temp2, &weight) == 3);
                addEdge(graph, temp1, temp2, weight);
                break;
            case 'd':// Delete
                if (scanf(" %d %d", &temp1, &temp2) == 2);
                deleteEdge(graph, temp1, temp2);
                break;
            case 'u':// Update
                if (scanf(" %d %d %d", &temp1, &temp2, &weight) == 3);
                updateEdge(graph, temp1, temp2, weight);
                break;
            case 's':// Search
                if (scanf(" %d %d", &temp1, &temp2) == 2);
                searchPath(graph, temp1, temp2);
                break;
            case 'p':// Print
                printGraph(graph);
                break;
        }
    }

    freeGraph(graph);
    return 0;
}

