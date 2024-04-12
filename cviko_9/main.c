#include <stdio.h>
#include <stdlib.h>

typedef struct edge{
    int destinationVertex; // (v in V)
    int weight; // w(u, v)
}Edge;

typedef struct adjacencyList{
    Edge *edges;    // array of edges - dynamic array, adjacency list
    int numberOfEdges;  // number of edges
    int capacity; // size of the array
}AdjList;

typedef struct graph{
    AdjList *lists; // array of adjacency lists, one list for each vertex
    int numberOfVertices; // number of all vertices
}Graph;


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

int main(void){
    int numberOfVertices;
    int numberOfEdges;
    Graph *graph;
    char line[50];
    fgets(line, sizeof(line), stdin);
    sscanf(line, "%d %d", &numberOfVertices, &numberOfEdges);
    graph = initialiseGraph(numberOfVertices, numberOfEdges);
    if (graph == NULL)
    {
        printf("Initialization failed");
    }else{
        displayGraph(graph);
    }
    return 0;
}