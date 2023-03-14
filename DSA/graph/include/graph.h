/** @file graph.h
* 
* @brief A data agnostic graph implementation.
*
*/
 
#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>

#define MAX_NUM_NODES 1000

typedef struct node node_t;
typedef struct graph graph_t;


// Function to create a new graph
graph_t* create_graph(void (*free_data)(void*), int (*compare_data)(const void*, const void*));

// Function to add a new node to the graph
node_t* add_node(graph_t* graph, int data);

// Function to add an edge between two nodes
void add_edge(node_t* node1, node_t* node2);

// Function to remove an edge between two nodes
void remove_edge(node_t* node1, node_t* node2);

// Function to remove a node from the graph
void remove_node(graph_t* graph, node_t* node);

// Function to check if there is a path between two nodes
bool is_path(graph_t* graph, node_t* start_node, node_t* end_node);

// Function to calculate the closeness centrality of a node
double closeness_centrality(graph_t* graph, node_t* node);

// Function to calculate the betweenness centrality of a node
double betweenness_centrality(graph_t* graph, node_t* node);

#endif
