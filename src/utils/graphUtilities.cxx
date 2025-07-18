#include "utils/graphUtilities.hxx"

double weighted_graph_metrics::averageEdgeWeight(const WeightedEdgeGraph& graph){
    if (graph.getNumEdges() == 0) {
        return 0.0; // Avoid division by zero if there are no edges
    }
    
    double totalWeight = 0.0;
    for (const auto& edge : graph.edgesVector) {
        totalWeight += std::get<2>(edge); // Get the weight from the tuple (node1ID, node2ID, weight)
    }
    
    return totalWeight / graph.getNumEdges(); // Return the average weight
}

std::pair<std::string,double> weighted_graph_metrics::maxEdgeWeight(const WeightedEdgeGraph& graph) {
    if (graph.getNumEdges() == 0) {
        return {"", 0.0}; // Return an empty pair if there are no edges 
    }
    
    double maxWeight = std::numeric_limits<double>::lowest(); // Initialize to the lowest possible value
    std::string maxNodeName = ""; // Initialize an empty string for the node name
    std::pair<std::string, double> maxEdge; // Initialize a pair to hold the maximum edge information
    for (const auto& edge : graph.edgesVector) {
        //maxWeight = std::max(maxWeight, std::get<2>(edge)); // Get the weight from the tuple and find the maximum
        if(std::get<2>(edge) > maxWeight) {
            maxWeight = std::get<2>(edge); // Update the maximum weight
            maxNodeName = graph.getNodeName(std::get<0>(edge)); // Get the name of the node corresponding to the maximum weight
            maxEdge = {maxNodeName, maxWeight}; // Update the pair with the maximum edge information
        } 
    }
    
    return maxEdge; // Return the maximum weight
}

std::pair<std::string,double> weighted_graph_metrics::minEdgeWeight(const WeightedEdgeGraph& graph) {
    if (graph.getNumEdges() == 0) {
        return {"", 0.0}; // Return an empty pair if there are no edges
    }
    
    double minWeight = std::numeric_limits<double>::max(); // Initialize to the maximum possible value
    std::string minNodeName = ""; // Initialize an empty string for the node name
    std::pair<std::string, double> minEdge; // Initialize a pair to hold the minimum edge information
    for (const auto& edge : graph.edgesVector) {
        //minWeight = std::min(minWeight, std::get<2>(edge)); // Get the weight from the tuple and find the minimum
        if(std::get<2>(edge) < minWeight) {
            minWeight = std::get<2>(edge); // Update the minimum weight
            minNodeName = graph.getNodeName(std::get<0>(edge)); // Get the name of the node corresponding to the minimum weight
            minEdge = {minNodeName, minWeight}; // Update the pair with the minimum edge information
        } 
    }
    
    return minEdge; // Return the minimum weight
}

double weighted_graph_metrics::averageEdgeDegree(const WeightedEdgeGraph& graph, DegreeMode mode) {
    if (graph.getNumEdges() == 0) {
        return 0.0;
    }
    
    double totalDegree = 0.0;
    /****** PROBLEMS WITH COUNTING TWO TIMES IN CASE OF FULL DEGREE */
    // for (int i = 0; i < graph.getNumNodes(); ++i) {
    //     switch (mode) {
    //         case DegreeMode::In:
    //             totalDegree += graph.inDegreeOfNode(i); // Get the in-degree of each node
    //             break;
    //         case DegreeMode::Out:
    //             totalDegree += graph.outDegreeOfNode(i); // Get the out-degree of each node
    //             break;
    //         case DegreeMode::Full:
    //             totalDegree += graph.degreeOfNode(i); // Get the full degree of each node
    //             break;
    //     }
    // }
    totalDegree = graph.getNumEdges(); // For average edge degree, we can directly use the number of edges
    
    return totalDegree / graph.getNumNodes(); // Return the average degree
}

std::pair<std::string,int> weighted_graph_metrics::maxEdgeDegree(const WeightedEdgeGraph& graph, DegreeMode mode) {
    if (graph.getNumEdges() == 0) {
        return {"", 0}; // Return an empty pair if there are no edges
    }
    int maxDegree = std::numeric_limits<int>::lowest(); // Initialize to the lowest possible value
    std::string maxNodeName = ""; // Initialize an empty string for the node name
    std::pair<std::string, int> maxEdge; // Initialize a pair to hold the maximum edge information
    for (int i = 0; i < graph.getNumNodes(); ++i) {
        int degree = 0; // Initialize degree variable
        switch (mode) {
            case DegreeMode::In:
                degree = graph.inDegreeOfNode(i); // Get the in-degree of each node
                break;
            case DegreeMode::Out:
                degree = graph.outDegreeOfNode(i); // Get the out-degree of each node
                break;
            case DegreeMode::Full:
                degree = graph.degreeOfNode(i); // Get the full degree of each node
                break;
        }
        if (degree > maxDegree) {
            maxDegree = degree; // Update the maximum degree
            maxNodeName = graph.getNodeName(i); // Get the name of the node corresponding to the maximum degree
            maxEdge = {maxNodeName, maxDegree}; // Update the pair with the maximum edge information
        }
    }
    return maxEdge; // Return the maximum degree
}

std::pair<std::string,int> weighted_graph_metrics::minEdgeDegree(const WeightedEdgeGraph& graph, DegreeMode mode) {
    if (graph.getNumEdges() == 0) {
        return {"", 0}; // Return 0 if there are no edges
    }
    
    int minDegree = std::numeric_limits<int>::max(); // Initialize to the maximum possible value
    std::string minNodeName = ""; // Initialize an empty string for the node name
    std::pair<std::string, int> minEdge; // Initialize a pair to hold the minimum edge information
    for (int i = 0; i < graph.getNumNodes(); ++i) {
        int degree = 0; // Initialize degree variable
        switch (mode) {
            case DegreeMode::In:
                degree = graph.inDegreeOfNode(i); // Get the in-degree of each node
                break;
            case DegreeMode::Out:
                degree = graph.outDegreeOfNode(i); // Get the out-degree of each node
                break;
            case DegreeMode::Full:
                degree = graph.degreeOfNode(i); // Get the full degree of each node
                break;
        }
        if (degree < minDegree) {
            minDegree = degree; // Update the minimum degree
            minNodeName = graph.getNodeName(i); // Get the name of the node corresponding to the minimum degree
            minEdge = {minNodeName, minDegree}; // Update the pair with the minimum edge information
        }
    }
    
    return minEdge; // Return the minimum degree
}

double weighted_graph_metrics::averageEdgeDegreeWeighted(const WeightedEdgeGraph& graph, DegreeMode mode) {
    if (graph.getNumEdges() == 0) {
        return 0.0; // Avoid division by zero if there are no edges
    }
    
    double totalWeightedDegree = 0.0;
    for (int i = 0; i < graph.getNumNodes(); ++i) {
        double weightedDegree = 0.0; // Initialize weighted degree variable
        // sum the edge weights for the node
        std::vector<int> neighborVector;
        switch ( mode) {
            case DegreeMode::In:
                neighborVector = graph.getPredecessors(i); // Get predecessors for in-degree
                for (int neighbor : neighborVector) {
                    weightedDegree += graph.getEdgeWeight(neighbor, i); // Sum the weights of incoming edges
                }
                break;
            case DegreeMode::Out:
                neighborVector = graph.getSuccessors(i); // Get successors for out-degree
                for (int neighbor : neighborVector) {
                    weightedDegree += graph.getEdgeWeight(i, neighbor); // Sum the weights of outgoing edges
                }
                break;
            case DegreeMode::Full:
                neighborVector = graph.getSuccessors(i); // Get first successors
                for (int neighbor : neighborVector) {
                    weightedDegree += graph.getEdgeWeight(i, neighbor); // Sum the weights of outgoing edges
                }
                neighborVector = graph.getPredecessors(i); // Get predecessors
                for (int neighbor : neighborVector) {
                    weightedDegree += graph.getEdgeWeight(neighbor, i); // Sum the weights of incoming edges
                }
                break;
        }

        totalWeightedDegree += weightedDegree; // Accumulate the total weighted degree
    }
    
    return totalWeightedDegree / graph.getNumNodes(); // Return the average weighted degree
}

std::pair<std::string,double> weighted_graph_metrics::maxEdgeDegreeWeighted(const WeightedEdgeGraph& graph, DegreeMode mode) {
    if (graph.getNumEdges() == 0) {
        return {"", 0.0}; // Return an empty pair if there are no edges
    }
    
    double maxWeightedDegree = std::numeric_limits<double>::lowest(); // Initialize to the lowest possible value
    std::string maxNodeName = ""; // Initialize an empty string for the node name
    std::pair<std::string, double> maxEdge; // Initialize a pair to hold the maximum edge information
    for (int i = 0; i < graph.getNumNodes(); ++i) {
        double weightedDegree = 0.0; // Initialize weighted degree variable
        // sum the edge weights for the node
        std::vector<int> neighborVector;
        switch ( mode) {
            case DegreeMode::In:
                neighborVector = graph.getPredecessors(i); // Get predecessors for in-degree
                for (int neighbor : neighborVector) {
                    // weightedDegree += graph.getEdgeWeight(neighbor, i) * graph.getNodeValue(neighbor); // Sum the weights of incoming edges multiplied by node values
                    weightedDegree += graph.getEdgeWeight(neighbor, i); // Sum the weights of incoming edges
                }
                break;
            case DegreeMode::Out:
                neighborVector = graph.getSuccessors(i); // Get successors for out-degree
                for (int neighbor : neighborVector) {
                    // weightedDegree += graph.getEdgeWeight(i, neighbor) * graph.getNodeValue(neighbor); // Sum the weights of outgoing edges multiplied by node values
                    weightedDegree += graph.getEdgeWeight(i, neighbor); // Sum the weights of outgoing edges
                }
                break;
            case DegreeMode::Full:
                neighborVector = graph.getSuccessors(i); // Get first successors
                for (int neighbor : neighborVector) {
                    // weightedDegree += graph.getEdgeWeight(i, neighbor) * graph.getNodeValue(neighbor); // Sum the weights of outgoing edges multiplied by node values
                    weightedDegree += graph.getEdgeWeight(i, neighbor); // Sum the weights of outgoing edges
                }
                neighborVector = graph.getPredecessors(i); // Get predecessors
                for (int neighbor : neighborVector) {
                    // weightedDegree += graph.getEdgeWeight(neighbor, i) * graph.getNodeValue(neighbor); // Sum the weights of incoming edges multiplied by node values
                    weightedDegree += graph.getEdgeWeight(neighbor, i); // Sum the weights of incoming edges
                }
                break;
        }
        if (weightedDegree > maxWeightedDegree) {
            maxWeightedDegree = weightedDegree; // Update the maximum weighted degree
            maxNodeName = graph.getNodeName(i); // Get the name of the node corresponding to the maximum weighted degree
            maxEdge = {maxNodeName, maxWeightedDegree}; // Update the pair with the maximum edge information
        }
    }
    
    return maxEdge; // Return the maximum weighted degree
}

std::pair<std::string,double> weighted_graph_metrics::minEdgeDegreeWeighted(const WeightedEdgeGraph& graph, DegreeMode mode) {
    if (graph.getNumEdges() == 0) {
        return {"", 0.0}; // Return an empty pair if there are no edges
    }
    
    double minWeightedDegree = std::numeric_limits<double>::max(); // Initialize to the maximum possible value
    std::string minNodeName = ""; // Initialize an empty string for the node name
    std::pair<std::string, double> minEdge; // Initialize a pair to hold the minimum edge information
    for (int i = 0; i < graph.getNumNodes(); ++i) {
        double weightedDegree = 0.0; // Initialize weighted degree variable
        std::vector<int> neighborVector;
        switch ( mode) {
            case DegreeMode::In:
                neighborVector = graph.getPredecessors(i); // Get predecessors for in-degree
                for (int neighbor : neighborVector) {
                    // weightedDegree += graph.getEdgeWeight(neighbor, i) * graph.getNodeValue(neighbor); // Sum the weights of incoming edges multiplied by node values
                    weightedDegree += graph.getEdgeWeight(neighbor, i); // Sum the weights of incoming edges
                }
                break;
            case DegreeMode::Out:
                neighborVector = graph.getSuccessors(i); // Get successors for out-degree
                for (int neighbor : neighborVector) {
                    // weightedDegree += graph.getEdgeWeight(i, neighbor) * graph.getNodeValue(neighbor); // Sum the weights of outgoing edges multiplied by node values
                    weightedDegree += graph.getEdgeWeight(i, neighbor); // Sum the weights of outgoing edges
                }
                break;
            case DegreeMode::Full:
                neighborVector = graph.getSuccessors(i); // Get first successors
                for (int neighbor : neighborVector) {
                    // weightedDegree += graph.getEdgeWeight(i, neighbor) * graph.getNodeValue(neighbor); // Sum the weights of outgoing edges multiplied by node values
                    weightedDegree += graph.getEdgeWeight(i, neighbor); // Sum the weights of outgoing edges
                }
                neighborVector = graph.getPredecessors(i); // Get predecessors
                for (int neighbor : neighborVector) {
                    // weightedDegree += graph.getEdgeWeight(neighbor, i) * graph.getNodeValue(neighbor); // Sum the weights of incoming edges multiplied by node values
                    weightedDegree += graph.getEdgeWeight(neighbor, i); // Sum the weights of incoming edges
                }
                break;
        }
        if (weightedDegree < minWeightedDegree) {
            minWeightedDegree = weightedDegree; // Update the minimum weighted degree
            minNodeName = graph.getNodeName(i); // Get the name of the node corresponding to the minimum weighted degree
            minEdge = {minNodeName, minWeightedDegree}; // Update the pair with the minimum edge information
        }
    }
    return minEdge; // Return the minimum weighted degree
}

double weighted_graph_metrics::averageStrengthCentrality(const WeightedEdgeGraph& graph, DegreeMode mode) {
    int numNodes = graph.getNumNodes();
    if (numNodes == 0) {
        return 0.0;
    }

    double totalStrength = 0.0;
    if(mode == DegreeMode::Out){
        for (int v = 0; v < numNodes; ++v) {
            double strength = 0.0;

            // Iterate over outgoing neighbors of node v
            for (int u : graph.getAdjList(v)) {
                strength += graph.getEdgeWeight(v, u);
            }

            // If the graph is undirected, and you're counting both v->u and u->v,
            // make sure not to double-count or adjust accordingly
            totalStrength += strength;
        }
    } else if(mode == DegreeMode::In){
        for (int v = 0; v < numNodes; ++v) {
            double strength = 0.0;
            // Iterate over incoming neighbors of node v
            for (int u : graph.getPredecessors(v)) {
                strength += graph.getEdgeWeight(u, v);
            }
            totalStrength += strength;
        }
    } else if(mode == DegreeMode::Full){
        for (int v = 0; v < numNodes; ++v) {
            double strength = 0.0;
            // Iterate over outgoing neighbors of node v
            for (int u : graph.getAdjList(v)) {
                strength += graph.getEdgeWeight(v, u);
            }
            // Iterate over incoming neighbors of node v
            for (int u : graph.getPredecessors(v)) {
                strength += graph.getEdgeWeight(u, v);
            }
            totalStrength += strength;
        }
    }

    return totalStrength / numNodes;
}

std::pair<std::string, double> weighted_graph_metrics::maxStrengthCentrality(const WeightedEdgeGraph& graph, DegreeMode mode) {
    int numNodes = graph.getNumNodes();
    if (numNodes == 0) {
        return {"", 0.0};
    }

    double maxStrength = std::numeric_limits<double>::lowest();
    std::string maxNodeName = "";
    std::pair<std::string, double> maxCentrality;

    if(mode == DegreeMode::Out){
        for (int v = 0; v < numNodes; ++v) {
            double strength = 0.0;

            // Iterate over outgoing neighbors of node v
            for (int u : graph.getAdjList(v)) {
                strength += graph.getEdgeWeight(v, u);
            }

            if (strength > maxStrength) {
                maxStrength = strength;
                maxNodeName = graph.getNodeName(v);
                maxCentrality = {maxNodeName, maxStrength};
            }
        }
    } else if(mode == DegreeMode::In){
        for (int v = 0; v < numNodes; ++v) {
            double strength = 0.0;
            // Iterate over incoming neighbors of node v
            for (int u : graph.getPredecessors(v)) {
                strength += graph.getEdgeWeight(u, v);
            }

            if (strength > maxStrength) {
                maxStrength = strength;
                maxNodeName = graph.getNodeName(v);
                maxCentrality = {maxNodeName, maxStrength};
            }
        }
    } else if(mode == DegreeMode::Full){
        for (int v = 0; v < numNodes; ++v) {
            double strength = 0.0;
            // Iterate over outgoing neighbors of node v
            for (int u : graph.getAdjList(v)) {
                strength += graph.getEdgeWeight(v, u);
            }
            // Iterate over incoming neighbors of node v
            for (int u : graph.getPredecessors(v)) {
                strength += graph.getEdgeWeight(u, v);
            }

            if (strength > maxStrength) {
                maxStrength = strength;
                maxNodeName = graph.getNodeName(v);
                maxCentrality = {maxNodeName, maxStrength};
            }
        }
    }

    return maxCentrality;
}

std::pair<std::string, double> weighted_graph_metrics::minStrengthCentrality(const WeightedEdgeGraph& graph, DegreeMode mode) {
    int numNodes = graph.getNumNodes();
    if (numNodes == 0) {
        return {"", 0.0};
    }

    double minStrength = std::numeric_limits<double>::max();
    std::string minNodeName = "";
    std::pair<std::string, double> minCentrality;

    if(mode == DegreeMode::Out){
        for (int v = 0; v < numNodes; ++v) {
            double strength = 0.0;

            // Iterate over outgoing neighbors of node v
            for (int u : graph.getAdjList(v)) {
                strength += graph.getEdgeWeight(v, u);
            }

            if (strength < minStrength) {
                minStrength = strength;
                minNodeName = graph.getNodeName(v);
                minCentrality = {minNodeName, minStrength};
            }
        }
    } else if(mode == DegreeMode::In){
        for (int v = 0; v < numNodes; ++v) {
            double strength = 0.0;
            // Iterate over incoming neighbors of node v
            for (int u : graph.getPredecessors(v)) {
                strength += graph.getEdgeWeight(u, v);
            }

            if (strength < minStrength) {
                minStrength = strength;
                minNodeName = graph.getNodeName(v);
                minCentrality = {minNodeName, minStrength};
            }
        }
    } else if(mode == DegreeMode::Full){
        for (int v = 0; v < numNodes; ++v) {
            double strength = 0.0;
            // Iterate over outgoing neighbors of node v
            for (int u : graph.getAdjList(v)) {
                strength += graph.getEdgeWeight(v, u);
            }
            // Iterate over incoming neighbors of node v
            for (int u : graph.getPredecessors(v)) {
                strength += graph.getEdgeWeight(u, v);
            }

            if (strength < minStrength) {
                minStrength = strength;
                minNodeName = graph.getNodeName(v);
                minCentrality = {minNodeName, minStrength};
            }
        }
    }

    return minCentrality;
}

double weighted_graph_metrics::weightedLocalClustering(const WeightedEdgeGraph& graph, int v){
    if (v < 0 || v >= graph.getNumNodes()) {
        throw std::out_of_range("Node index out of range");
    }

    std::vector<int> neighbors = graph.getSuccessors(v); // I am not convinced by the fact that I am callin the successors here, maybe because they are defined as outgoing edges (immediately connected nodes)
    int k = neighbors.size();
    
    if (k < 2) {
        return 0.0; // No clustering possible with less than 2 neighbors
    }

    double totalWeight = 0.0;
    int edgeCount = 0;

    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {
            int u = neighbors[i];
            int w = neighbors[j];
            double weight = graph.getEdgeWeight(u, w);
            if (weight > 0) {
                totalWeight += weight;
                edgeCount++;
            }
        }
    }

    return (edgeCount > 0) ? (totalWeight / (k * (k - 1) / 2)) : 0.0; // Return the average weight of edges between neighbors
}

double weighted_graph_metrics::weightedGlobalClustering(const WeightedEdgeGraph& graph) {
    double totalClustering = 0.0;
    int numNodes = graph.getNumNodes();

    for (int v = 0; v < numNodes; ++v) {
        totalClustering += weightedLocalClustering(graph, v);
    }

    return (numNodes > 0) ? (totalClustering / numNodes) : 0.0; // Return the average clustering coefficient
}


double weighted_graph_metrics::weightedPathWeight(const WeightedEdgeGraph& graph, const std::vector<int>& path){
    if (path.empty()) {
        return 0.0; // Return 0 if the path is empty
    }

    double totalWeight = 0.0;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        int node1 = path[i];
        int node2 = path[i + 1];
        if (node1 < 0 || node2 < 0 || node1 >= graph.getNumNodes() || node2 >= graph.getNumNodes()) {
            throw std::out_of_range("Node index out of range in the path");
        }
        // Check for the validity of the edge
        if(!graph.hasEdge(node1, node2)) {
            throw std::invalid_argument("Invalid edge in the path: " + std::to_string(node1) + " -> " + std::to_string(node2));
        }
        totalWeight += graph.getEdgeWeight(node1, node2); // Sum the weights of edges in the path
    }

    return totalWeight; // Return the total weight of the path
}

std::vector<std::pair<int, std::vector<int>>> weighted_graph_metrics::allUnweightedShortestPathFromSourceBFS(const WeightedEdgeGraph& graph, int source) {
    if (source < 0 || source >= graph.getNumNodes()) {
        throw std::out_of_range("Source node index out of range");
    }

    std::vector<std::pair<int, std::vector<int>>> shortestPaths; // Vector to store pairs of node and its shortest path
    std::vector<bool> visited(graph.getNumNodes(), false); // Vector to track visited nodes
    std::queue<int> queue; // Queue for BFS
    queue.push(source); // Start BFS from the source node
    visited[source] = true; // Mark the source node as visited

    while (!queue.empty()) {
        int currentNode = queue.front();
        queue.pop();
        std::vector<int> path; // Vector to store the path to the current node
        path.push_back(currentNode); // Add the current node to the path
        // Explore all neighbors of the current node
        for (int neighbor : graph.getSuccessors(currentNode)) { // Iterate over neighbors of the current node
            if (!visited[neighbor]) { // If the neighbor has not been visited
                visited[neighbor] = true; // Mark it as visited
                queue.push(neighbor); // Add it to the queue for further exploration
                path.push_back(neighbor); // Add the neighbor to the path
            }
        }

        shortestPaths.emplace_back(currentNode, path); // Store the current node and its path in the result vector
    }

    // Add the nodes that were not reachable from the source node
    for (int i = 0; i < graph.getNumNodes(); ++i) {
        if (!visited[i]) {
            shortestPaths.emplace_back(i, std::vector<int>{}); // Add unreachable nodes with an empty path
        }
    }

    return shortestPaths; // Return all shortest paths from the source node
}

std::vector<std::pair<int, std::vector<int>>> weighted_graph_metrics::allWeightedShortestPathFromSourceDijkstra(const WeightedEdgeGraph& graph, int source) {
    if (source < 0 || source >= graph.getNumNodes()) {
        throw std::out_of_range("Source node index out of range");
    }

    std::vector<double> distances(graph.getNumNodes(), std::numeric_limits<double>::max()); // Vector to store distances from source
    std::vector<int> previous(graph.getNumNodes(), -1); // Vector to store previous nodes in the path
    std::vector<bool> visited(graph.getNumNodes(), false); // Vector to track visited nodes
    distances[source] = 0.0; // Distance to the source node is 0

    for (int i = 0; i < graph.getNumNodes(); ++i) {
        int currentNode = -1;
        double minDistance = std::numeric_limits<double>::max();

        // Find the unvisited node with the smallest distance, also implementable with a priority queue for efficiency
        for (int j = 0; j < graph.getNumNodes(); ++j) {
            if (!visited[j] && distances[j] < minDistance) {
                minDistance = distances[j];
                currentNode = j;
            }
        }

        if (currentNode == -1) {
            break; // All reachable nodes have been visited
        }

        visited[currentNode] = true; // Mark the current node as visited

        // Update distances to neighbors of the current node
        for (int neighbor : graph.getSuccessors(currentNode)) {
            double edgeWeight = graph.getEdgeWeight(currentNode, neighbor);
            if (edgeWeight > 0 && !visited[neighbor]) { // Only consider unvisited neighbors with positive edge weights
                double newDistance = distances[currentNode] + edgeWeight;
                if (newDistance < distances[neighbor]) {
                    distances[neighbor] = newDistance; // Update distance to neighbor
                    previous[neighbor] = currentNode; // Update previous node in the path
                }
            }
        }
    }

    // Construct the shortest paths from the source node to all other nodes
    std::vector<std::pair<int, std::vector<int>>> shortestPaths;
    for (int i = 0; i < graph.getNumNodes(); ++i) {
        std::vector<int> path;
        if (distances[i] < std::numeric_limits<double>::max()) { // If the node is reachable
            for (int v = i; v != -1; v = previous[v]) {
                path.push_back(v); // Backtrack to construct the path
            }
            std::reverse(path.begin(), path.end()); // Reverse the path to get it from source
        }
        shortestPaths.emplace_back(i, path); // Store the node and its path in the result vector
    }
    return shortestPaths; // Return all shortest paths from the source node
}