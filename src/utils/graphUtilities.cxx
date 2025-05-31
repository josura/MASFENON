#include "utils/graphUtilities.hxx"

double weighed_graph_metrics::averageEdgeWeight(const WeightedEdgeGraph& graph){
    if (graph.getNumEdges() == 0) {
        return 0.0; // Avoid division by zero if there are no edges
    }
    
    double totalWeight = 0.0;
    for (const auto& edge : graph.edgesVector) {
        totalWeight += std::get<2>(edge); // Get the weight from the tuple (node1ID, node2ID, weight)
    }
    
    return totalWeight / graph.getNumEdges(); // Return the average weight
}

double weighed_graph_metrics::maxEdgeWeight(const WeightedEdgeGraph& graph) {
    if (graph.getNumEdges() == 0) {
        return 0.0; 
    }
    
    double maxWeight = std::numeric_limits<double>::lowest(); // Initialize to the lowest possible value
    for (const auto& edge : graph.edgesVector) {
        maxWeight = std::max(maxWeight, std::get<2>(edge)); // Get the weight from the tuple and find the maximum
    }
    
    return maxWeight; // Return the maximum weight
}

double weighed_graph_metrics::minEdgeWeight(const WeightedEdgeGraph& graph) {
    if (graph.getNumEdges() == 0) {
        return 0.0;
    }
    
    double minWeight = std::numeric_limits<double>::max(); // Initialize to the maximum possible value
    for (const auto& edge : graph.edgesVector) {
        minWeight = std::min(minWeight, std::get<2>(edge)); // Get the weight from the tuple and find the minimum
    }
    
    return minWeight; // Return the minimum weight
}

double weighed_graph_metrics::averageEdgeDegree(const WeightedEdgeGraph& graph) {
    if (graph.getNumEdges() == 0) {
        return 0.0;
    }
    
    double totalDegree = 0.0;
    for (int i = 0; i < graph.getNumNodes(); ++i) {
        totalDegree += graph.degreeOfNode(i); // Get the degree of each node and sum them up
    }
    
    return totalDegree / graph.getNumNodes(); // Return the average degree
}

int weighed_graph_metrics::maxEdgeDegree(const WeightedEdgeGraph& graph) {
    if (graph.getNumEdges() == 0) {
        return 0; // Avoid division by zero if there are no edges
    }
    
    int maxDegree = 0;
    for (int i = 0; i < graph.getNumNodes(); ++i) {
        maxDegree = std::max(maxDegree, graph.degreeOfNode(i)); // Get the degree of each node and find the maximum
    }
    
    return maxDegree; // Return the maximum degree
}

int weighed_graph_metrics::minEdgeDegree(const WeightedEdgeGraph& graph) {
    if (graph.getNumEdges() == 0) {
        return 0; // Avoid division by zero if there are no edges
    }
    
    int minDegree = std::numeric_limits<int>::max(); // Initialize to the maximum possible value
    for (int i = 0; i < graph.getNumNodes(); ++i) {
        minDegree = std::min(minDegree, graph.degreeOfNode(i)); // Get the degree of each node and find the minimum
    }
    
    return minDegree; // Return the minimum degree
}
