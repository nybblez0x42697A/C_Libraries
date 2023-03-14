/** @file kdtree.c
 *
 * @brief A simple KD-Tree implementation.
 *
 * @headerfile kdtree.h
 *
 * @author CW2 Timothy C. DeBerry
 *
 * @version 1.0
 *
 */

#include "../include/kdtree.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_RAND_POINTS 5

// structure to represent a point in 2D space
typedef struct point {
	double x;
	double y;
} point_t;

struct trunk {
	struct trunk *prev;
	const char *str;
};

kd_tree_t *kdtree_create()
{
	kd_tree_t *tree = calloc(1, sizeof(kd_tree_t));
	if (tree == NULL) {
		return NULL;
	}
	tree->root = NULL;
	tree->dimensions = 0;
	tree->levels = 0;
	tree->size = 0;

	return tree;
}

// Recursive function to insert a new node in the kd-tree
kdtree_node_t *insert_node(kdtree_node_t * node, kdtree_node_t * new_node,
			   void *data, int depth)
{
	if (node == NULL) {
        new_node->data = data;
		return new_node;
	}

	int current_dimension = depth % DIMENSIONS;
	node->depth = depth;

	if (new_node->coordinates[current_dimension]
	    < node->coordinates[current_dimension]) {
		node->left = insert_node(node->left, new_node, data, depth + 1);
        
		new_node->data = data;
	} else {
		node->right =
		    insert_node(node->right, new_node, data, depth + 1);
		new_node->data = data;
	}

	return node;
}

// Function to insert a new point in the kd-tree
void insert(kd_tree_t * tree, double x_axis, double y_axis, void *data)
{

	kdtree_node_t *new_node = calloc(1, sizeof(kdtree_node_t));

	new_node->coordinates[0] = x_axis;
	new_node->coordinates[1] = y_axis;

	tree->root = insert_node(tree->root, new_node, data, 0);
	tree->size++;
	return;
}

void kdtree_node_destroy(kdtree_node_t * node)
{
	if (node == NULL) {
		return;
	}

	kdtree_node_destroy(node->right);	// recursively print right child
	free(node->right);
	kdtree_node_destroy(node->left);	// recursively print left child
	free(node->left);
}

void kdtree_destroy(kd_tree_t * tree)
{
	if (tree == NULL) {
		return;
	}
	kdtree_node_destroy(tree->root);
	free(tree->root);
	free(tree);
}

void print_in_order(kdtree_node_t * node, int level)	// print in order. left node first.
{
	if (node == NULL) {
		return;
	}

	for (int i = 0; i < level; i++) {
		printf("  ");	// indent tree by level
	}
	print_in_order(node->left, level + 1);	// recursively print left child
	printf("(%f, %f)", node->coordinates[0], node->coordinates[1]);
	printf("\n");		// print node data
	print_in_order(node->right, level + 1);	// recursively print right child
}

void print_pre_order(kdtree_node_t * node, int level)	// shows tree structure.
{
	if (node == NULL) {
		return;
	}

	for (int i = 0; i < level; i++) {
		printf("  ");	// indent tree by level
	}
	printf("(%f, %f)\n", node->coordinates[0], node->coordinates[1]);	// print node data

	print_pre_order(node->left, level + 1);	// recursively print left child

	print_pre_order(node->right, level + 1);	// recursively print right child
}

void print_post_order(kdtree_node_t * node, int level)
{
	if (node == NULL) {
		return;
	}

	for (int i = 0; i < level; i++) {
		printf("  ");	// indent tree by level
	}
	print_post_order(node->right, level + 1);	// recursively print right child

	print_post_order(node->left, level + 1);	// recursively print left child
	printf("(%f, %f)\n", node->coordinates[0], node->coordinates[1]);	// print node data
}

static void print_trunks(struct trunk *p)
{
	if (!p) {
		return;
	}
	print_trunks(p->prev);
	printf("%s", p->str);
}

static void
print_recursive(kdtree_node_t * root, struct trunk *prev, int is_left)
{
	if (!root) {
		return;
	}

	struct trunk this_disp = { prev, "     " };
	const char *prev_str = this_disp.str;
	print_recursive(root->right, &this_disp, 1);

	if (!prev) {
		this_disp.str = "---";
		printf("\n");
	} else if (is_left) {
		this_disp.str = ".--";
		printf("\n");
		prev_str = "    |";
	} else {
		this_disp.str = "`--";
		printf("\n");
		prev->str = prev_str;
	}
	// printf("\n");
	print_trunks(&this_disp);
	printf("(%.2lf, %.2lf)\n", root->coordinates[0], root->coordinates[1]);

	if (prev) {
		prev->str = prev_str;
	}
	this_disp.str = "    |";

	print_recursive(root->left, &this_disp, 0);
	if (!prev) {
		puts("");
	}
}

void print_pretty_visual(kdtree_node_t * root)
{
	if (!root) {
		return;
	}
	print_recursive(root, NULL, 0);
}

#ifdef __DEBUG__TREE__
int main(void)
{

	kd_tree_t *tree = kdtree_create();
	srand(time(NULL));	// seed the random number generator

	// generate random points
	point_t points[NUM_RAND_POINTS];
	for (int i = 0; i < NUM_RAND_POINTS; i++) {
		points[i].x = rand() % 100;	// generate random x coordinate
		points[i].y = rand() % 100;
	}

	// insert points into tree
	for (int i = 0; i < NUM_RAND_POINTS; i++) {
		insert(tree, points[i].x, points[i].y);
	}

	print_visual(tree->root);

	// printf("\n\n\n\n\nIn Order:\n");
	// print_in_order(tree->root, 0);
	// printf("\n\n\n\n\nPre Order:\n");
	// print_pre_order(tree->root, 0);
	printf("\n\n\n\n\n\nPost Order:\n");
	// print_post_order(tree->root, 0);
	test_print_post_order(tree->root);
	kdtree_destroy(tree);
	return 0;
}
#endif
