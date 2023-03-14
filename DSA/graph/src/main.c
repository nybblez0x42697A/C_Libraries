#include "../include/graph.h"


void free_this(void * data) 
{
    free(data);
}

int compare_int(const void* data1, const void* data2) {
    int a = *(int*)data1;
    int b = *(int*)data2;
    return a - b;
}

int main(int argc, char ** argv)
{
    graph_t * graph = create_graph((*free_this), (*compare_int));
}