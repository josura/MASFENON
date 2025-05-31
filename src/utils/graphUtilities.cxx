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

std::pair<std::string,double> weighed_graph_metrics::maxEdgeWeight(const WeightedEdgeGraph& graph) {
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

std::pair<std::string,double> weighed_graph_metrics::minEdgeWeight(const WeightedEdgeGraph& graph) {
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

std::pair<std::string,int> weighed_graph_metrics::maxEdgeDegree(const WeightedEdgeGraph& graph) {
    if (graph.getNumEdges() == 0) {
        return {"", 0}; // Return an empty pair if there are no edges
    }
    int maxDegree = std::numeric_limits<int>::lowest(); // Initialize to the lowest possible value
    std::string maxNodeName = ""; // Initialize an empty string for the node name
    std::pair<std::string, int> maxEdge; // Initialize a pair to hold the maximum edge information
    for (int i = 0; i < graph.getNumNodes(); ++i) {
        int degree = graph.degreeOfNode(i); // Get the degree of each node
        if (degree > maxDegree) {
            maxDegree = degree; // Update the maximum degree
            maxNodeName = graph.getNodeName(i); // Get the name of the node corresponding to the maximum degree
            maxEdge = {maxNodeName, maxDegree}; // Update the pair with the maximum edge information
        }
    }
    return maxEdge; // Return the maximum degree
}

std::pair<std::string,int> weighed_graph_metrics::minEdgeDegree(const WeightedEdgeGraph& graph) {
    if (graph.getNumEdges() == 0) {
        return {"", 0}; // Return 0 if there are no edges
    }
    
    int minDegree = std::numeric_limits<int>::max(); // Initialize to the maximum possible value
    std::string minNodeName = ""; // Initialize an empty string for the node name
    std::pair<std::string, int> minEdge; // Initialize a pair to hold the minimum edge information
    for (int i = 0; i < graph.getNumNodes(); ++i) {
        int degree = graph.degreeOfNode(i); // Get the degree of each node
        if (degree < minDegree) {
            minDegree = degree; // Update the minimum degree
            minNodeName = graph.getNodeName(i); // Get the name of the node corresponding to the minimum degree
            minEdge = {minNodeName, minDegree}; // Update the pair with the minimum edge information
        }
    }
    
    return minEdge; // Return the minimum degree
}

double weighed_graph_metrics::averageEdgeDegreeWeighted(const WeightedEdgeGraph& graph) {
    if (graph.getNumEdges() == 0) {
        return 0.0; // Avoid division by zero if there are no edges
    }
    
    double totalWeightedDegree = 0.0;
    for (int i = 0; i < graph.getNumNodes(); ++i) {
        totalWeightedDegree += graph.degreeOfNode(i); // Get the weighted degree of each node and sum them up
    }
    
    return totalWeightedDegree / graph.getNumNodes(); // Return the average weighted degree
}