void searchPath(int sourceVertex, int destinationVertex, Graph *graph){
    int numberOfVertices = graph->numberOfVertices;
    int *distances = (int *)malloc(graph->numberOfVertices * sizeof(int));
    int *previousVertices = (int *)malloc(graph->numberOfVertices * sizeof(int));
    int *path = NULL;
    MinHeap *minHeap = createMinHeap(numberOfVertices);

    // Init. distances and heap for all vertices
    for (int v = 0; v < numberOfVertices; v++){
        distances[v] = (v == sourceVertex) ? 0 : INT_MAX;// Distance from the source vertex is 0 and infinity for the rest
        previousVertices[v] = -1;// No previous vertices
        minHeap->elements[v] = (MinHeapNode){v, distances[v]};// Add the vertex to the heap
        minHeap->sizeOfHeap++;
    }

    // Heapify the MinHeap - bottom-up
    for (int i = (minHeap->sizeOfHeap - 2) / 2; i >= 0; i--){// Start from the last non-leaf node
        minHeapify(minHeap, i);
    }

    while (minHeap->sizeOfHeap > 0){
        // Extract the vertex with minimum distance from the root
        MinHeapNode minHeapNode = extractMin(minHeap);
        int u = minHeapNode.vertex;// Vertex with the minimum distance from the extracted vertex

        // Traverse all buckets in the adjacency list for vertex u
        for (int i = 0; i < numberOfVertices; i++){
            Edge *currentEdge = graph->vertices[u].edges[i];
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

    // Output the results
    if (distances[destinationVertex] != INT_MAX) {
        printf("Shortest path from %d to %d is %d with path: ", sourceVertex, destinationVertex, distances[destinationVertex]);
        int crawl = destinationVertex;
        int count = 0;
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
        for (int i = 0; i < count; i++) {
            if (i > 0) printf(" -> ");
            printf("%d", path[i]);
        }
        printf("\n");
        free(path);
    } else {
        printf("No path exists from %d to %d\n", sourceVertex, destinationVertex);
    }

    free(distances);
    free(previousVertices);
    free(minHeap->elements);
    free(minHeap);
}
