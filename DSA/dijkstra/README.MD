Dijkstra's Algorithm

Dijkstra's Algorithm is a popular algorithm for finding the shortest path between two nodes in a graph. It was first proposed by computer scientist Edsger W. Dijkstra in 1956.

To use the algorithm, you must have a weighted, directed graph with non-negative edge weights. The algorithm works by starting at the source node and repeatedly visiting the node with the lowest distance from the source. As it does so, it updates the distances to each of the neighboring nodes, ensuring that the distance to each node is the shortest possible.
Dijkstra's algorithm is a graph search algorithm that is used to find the shortest path between a given source node and all other nodes in a graph. This algorithm is guaranteed to find the shortest path between the source node and all other nodes, even if the graph contains negative-weight edges.

To use Dijkstra's Algorithm to find the shortest path from node A to node B, follow these steps:

Set the distance from the source node to 0, and the distance to all other nodes to infinity.
Select the unvisited node with the smallest distance from the source node, and mark it as the current node.
Visit each of the unvisited neighbors of the current node, and calculate the distance to each neighbor using the formula distance[neighbor] = min(distance[neighbor], distance[current] + edge_weight).
Repeat steps 2 and 3 until all nodes have been visited or the destination node has been reached.
Once all nodes have been visited, the algorithm will have computed the shortest distances from the source node to all other nodes in the graph. The shortest path from the source node to the destination node can then be reconstructed by following the trail of nodes with the lowest distances.

Steps to Solve
Create a set of unvisited nodes called the unvisited set.

Set the distance of the source node to 0 and the distance of all other nodes to infinity.

Set the current node to the source node and mark it as visited.

For the current node, consider all of its unvisited neighbors and calculate their tentative distances. For example, if the current node A is connected to B with a distance of 6, and the current node's distance is 10, the tentative distance to B would be 10 + 6 = 16.

Compare the tentative distance to the current distance, and if the tentative distance is smaller, update the current distance to the tentative distance. For example, if the current distance to B is 20, then update it to 16.

When all of the unvisited neighbors of the current node have been processed, mark the current node as visited and remove it from the unvisited set.

If the destination node has been marked as visited, or if the smallest tentative distance among the unvisited nodes is infinity, then stop. The algorithm has finished.

Otherwise, set the current node to the unvisited node with the smallest tentative distance, and go back to step 4. Repeat this process until the destination node is visited or until there are no more unvisited nodes.
