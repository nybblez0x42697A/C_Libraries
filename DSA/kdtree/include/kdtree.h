/** @file kdtree.h
 *
 * @brief Public api for a simple KD-Tree implementation.
 *
 * @author CW2 Timothy C. DeBerry
 *
 * @version 1.0
 */

#ifndef KDTREE_H
#define KDTREE_H

#define DIMENSIONS 2

typedef struct kdtree_node kdtree_node_t;
struct trunk;

typedef struct kdtree_node {
	double coordinates[DIMENSIONS];
	void *data;
	int depth;
	kdtree_node_t *left;
	kdtree_node_t *right;
} kdtree_node_t;

typedef struct kd_tree {
	kdtree_node_t *root;
	int size;
	int levels;
	int dimensions;
} kd_tree_t;

kd_tree_t *kdtree_create();

kdtree_node_t *insert_node(kdtree_node_t * node,
			   kdtree_node_t * new_node, void *data, int depth);

void insert(kd_tree_t * tree, double x_axis, double y_axis, void *data);

void kdtree_destroy(kd_tree_t * tree);

void print_in_order(kdtree_node_t * node, int level);	// print in order. left node first.

void print_pre_order(kdtree_node_t * node, int level);	// shows tree structure.

void print_post_order(kdtree_node_t * node, int level);

void print_pretty_visual(kdtree_node_t * root);

#endif
