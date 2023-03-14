#include "../include/graph.h"
#include "../../queue/include/queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Structure to represent a node in the graph
typedef struct node {
    void* data;  // Data stored at the node
    int node_id; // Unique id for node. Used for indexing
    int num_neighbors;  // Number of neighboring nodes
    node_t** neighbors;  // List of neighboring nodes
} node_t;

// Structure to represent the graph
typedef struct graph {
    int num_nodes;  // Number of nodes in the graph
    node_t** nodes;  // List of nodes in the graph
    void (*free_data)(void*);  // pointer to the function to free the data
    int (*compare_data)(const void*, const void*);  // pointer to the function to compare the data
} graph_t;

// Function to create a new node
node_t* create_node(void* data) {
    node_t* new_node = (node_t*) malloc(sizeof(node_t));
    new_node->data = data;
    new_node->num_neighbors = 0;
    return new_node;
}

// Function to create a new graph
graph_t* create_graph(void (*free_data)(void*), int (*compare_data)(const void*, const void*)) {
    graph_t* new_graph = (graph_t*) malloc(sizeof(graph_t));
    new_graph->num_nodes = 0;
    new_graph->free_data = free_data;
    new_graph->compare_data = compare_data;
    return new_graph;
}

// Function to add a new node to the graph
void add_node_to_graph(graph_t* graph, node_t* node) {
    graph->nodes[graph->num_nodes++] = node;
}

// Function to add an edge between two nodes
void add_edge(node_t* node1, node_t* node2) {
    node1->neighbors[node1->num_neighbors++] = node2;
    node2->neighbors[node2->num_neighbors++] = node1;
}

// Function to perform a breadth-first search on the graph
void breadth_first_search(graph_t* graph, node_t* start_node) {
    bool visited[MAX_NUM_NODES] = {false};
    Queue_t* queue = create_queue();
    enqueue(queue, start_node);
    visited[start_node->node_id] = true;

    while (!is_empty(queue)) {
        node_t* current_node = dequeue(queue);
        printf("Visited node with data %d\n", current_node->data);
        for (int i = 0; i < current_node->num_neighbors; i++) {
            node_t* neighbor = current_node->neighbors[i];
            if (!visited[neighbor->node_id]) {
                enqueue(queue, neighbor);
                visited[neighbor->node_id] = true;
            }
        }
    }
}

// Function to calculate the degree centrality of a node
int degree_centrality(node_t* node) {
    return node->num_neighbors;
}

// Function to free all the memory allocated for the graph
// Used for testing. Users of the library must provide their own free function.
void free_graph(graph_t* graph) {
    for (int i = 0; i < graph->num_nodes; i++) {
        node_t* node = graph->nodes[i];
        graph->free_data(node->data);
        for (int j = 0; j < node->num_neighbors; j++) {
            free(node->neighbors[j]);
        }
        free(node);
    }
    free(graph);
}

// Function to find a node with a specific data in the graph
node_t* find_node(graph_t* graph, void* data) {
    for (int i = 0; i < graph->num_nodes; i++) {
        node_t* node = graph->nodes[i];
        if (graph->compare_data(node->data, data) == 0) {
            return node;
        }
    }
    return NULL;
}

// Function to remove a node from the graph
void remove_node(graph_t* graph, node_t* node) {
    // Find the node in the graph and remove it
    int node_index;
    for (int i = 0; i < graph->num_nodes; i++) {
        if (graph->compare_data(graph->nodes[i]->data, node->data) == 0) {
            node_index = i;
            break;
        }
    }
    graph->nodes[node_index] = graph->nodes[graph->num_nodes - 1];
    graph->num_nodes--;

    // Remove edges from neighboring nodes to the removed node
    for (int i = 0; i < graph->num_nodes; i++) {
        node_t* current_node = graph->nodes[i];
        for (int j = 0; j < current_node->num_neighbors; j++) {
            if (graph->compare_data(current_node->neighbors[j]->data, node->data) == 0) {
                current_node->neighbors[j] = current_node->neighbors[current_node->num_neighbors - 1];
                current_node->num_neighbors--;
                break;
            }
        }
    }
    graph->free_data(node->data);
    free(node);
}


// Function to calculate the closeness centrality of a node
double closeness_centrality(graph_t* graph, node_t* node) {
    // Use a breadth-first search to find the shortest distance from the node to all other nodes
    int distances[MAX_NUM_NODES] = {0};
    for (int i = 0; i < graph->num_nodes; i++) {
        distances[i] = -1;
    }
    distances[node->node_id] = 0;
    Queue_t* queue = create_queue();
    enqueue(queue, node);
    while (!is_empty(queue)) {
        node_t* current_node = dequeue(queue);
        for (int i = 0; i < current_node->num_neighbors; i++) {
            node_t* neighbor = current_node->neighbors[i];
            if (distances[neighbor->node_id] == -1) {
                distances[neighbor->node_id] = distances[current_node->node_id] + 1;
                enqueue(queue, neighbor);
            }
        }
    }

    // Sum the shortest distances from the node to all other nodes
    int sum_distances = 0;
    for (int i = 0; i < graph->num_nodes; i++) {
        if (distances[i] != -1) {
            sum_distances += distances[i];
        }
    }

    // Divide the sum of distances by the total number of nodes
    double centrality = ((double) sum_distances) / (graph->num_nodes - 1);
    return centrality;
}

// Function to calculate the between-ness centrality of a node
double betweenness_centrality(graph_t* graph, node_t* node) {
    // Use a breadth-first search to find the number of shortest paths that pass through the node
    int num_shortest_paths[MAX_NUM_NODES] = {0};
    int num_paths[MAX_NUM_NODES] = {0};
    for (int i = 0; i < graph->num_nodes; i++) {
        num_shortest_paths[i] = 0;
        num_paths[i] = 0;
    }
    num_shortest_paths[node->data] = 1;
    num_paths[node->data] = 1;
    Queue_t* queue = create_queue();
    enqueue(queue, node);
    while (!is_empty(queue)) {
        node_t* current_node = dequeue(queue);
        for (int i = 0; i < current_node->num_neighbors; i++) {
            node_t* neighbor = current_node->neighbors[i];
            num_paths[neighbor->node_id] += num_paths[current_node->node_id];
            if (neighbor != node) {
                num_shortest_paths[neighbor->node_id] += num_shortest_paths[current_node->node_id];
            }
            enqueue(queue, neighbor);
        }
    }

        // Divide the number of shortest paths that pass through the node by the total number of shortest paths
    double centrality = 0;
    for (int i = 0; i < graph->num_nodes; i++) {
        if (i != node->data) {
            centrality += ((double) num_shortest_paths[i]) / num_paths[i];
        }
    }
    centrality /= (graph->num_nodes - 1) * (graph->num_nodes - 2);
    return centrality;
}

