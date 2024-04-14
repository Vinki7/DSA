#include <stdio.h>
#include <stdlib.h>

typedef struct edge{
    int destinationVertex;
    int weight;
    struct Edge *next;
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
    int dist;
} MinHeapNode;

typedef struct {
    MinHeapNode *elements;
    int size;
    int capacity;
} MinHeap;


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
        memset(graph->vertices[i].edges, 0, numberOfVertices * sizeof(Edge *));
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

int main(void){
    
    
    return 0;
}