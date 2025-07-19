/**
 * @file graphUtilities.hxx
 * @ingroup Core
 * @brief Contains utility functions for graph operations.
 * @details The functions are used to work with graphs, and finding metrics
 */
#pragma once
#include <vector>
#include <tuple>
#include <queue>
#include "data_structures/WeightedEdgeGraph.hxx"

/**
 * @namespace weighted_graph_metrics
 * @brief Contains utility functions for calculating metrics on weighted graphs.
 * @details The functions are used to calculate metrics such as the average weight of edges in a graph.
 * @note The functions are designed to work with the WeightedEdgeGraph class.
 */
namespace weighted_graph_metrics {
    /**
     * @enum DegreeMode
     * @brief Enum class to specify the mode of degree calculation.
     * @ingroup Core
     * @details This enum class is used to specify whether to calculate the in-degree, out-degree, or full degree of a node in a graph.
     * @note The in-degree is the number of edges directed towards a node, the out-degree is the number of edges directed away from a node, and the full degree is the total number of edges connected to a node (both in and out).
     */
    enum class DegreeMode {
        In,  /**< Define the mode for considering only the entering edges in the functions */
        Out, /**< Define the mode for considering only the exiting edges in the functions  */
        Full /**< Define the mode for considering all the edges */
    };
    /**
     * @brief Calculates the average weight of edges in a weighted graph.
     * @param graph The weighted edge graph to analyze.
     * @return The average weight of the edges in the graph.
     * @details This function iterates through all edges in the graph and computes the average weight.
     */
    double averageEdgeWeight(const WeightedEdgeGraph& graph);
    /**
     * @brief Computes the maximum edge weight in a weighted graph.
     * @param graph The weighted edge graph to analyze.
     * @return The maximum edge weight in the graph.
     * @details This function iterates through all edges in the graph and finds the maximum weight.
     */
    std::pair<std::string,double> maxEdgeWeight(const WeightedEdgeGraph& graph);
    /**
     * @brief Computes the minimum edge weight in a weighted graph.
     * @param graph The weighted edge graph to analyze.
     * @return The minimum edge weight in the graph.
     * @details This function iterates through all edges in the graph and finds the minimum weight.
     */
    std::pair<std::string,double> minEdgeWeight(const WeightedEdgeGraph& graph);
    /**
     * @brief Computes the average edge degree in a weighted graph.
     * @param graph The weighted edge graph to analyze.
     * @param mode The mode of degree calculation (In, Out, Full).
     * @return The average edge degree in the graph.
     * @details This function calculates the average degree of edges in the graph, which is defined as the average number of edges connected to any node.
     * @note The degree of a node is the number of edges connected to it. Depending on the mode, it can be the in-degree, out-degree, or full degree.
     * @return 0 if the graph has no edges.
     */
    double averageEdgeDegree(const WeightedEdgeGraph& graph, DegreeMode mode = DegreeMode::Full);
    /**
     * @brief Computes the maximum edge degree in a weighted graph.
     * @param graph The weighted edge graph to analyze.
     * @param mode The mode of degree calculation (In, Out, Full).
     * @return The maximum edge degree in the graph.
     * @details This function calculates the maximum degree of edges in the graph, which is defined as the maximum number of edges connected to any node.
     * @note The degree of a node is the number of edges connected to it. Depending on the mode, it can be the in-degree, out-degree, or full degree.
     * @return The pair <name,value>. 0 if the graph has no edges.
     */
    std::pair<std::string,int> maxEdgeDegree(const WeightedEdgeGraph& graph, DegreeMode mode = DegreeMode::Full);
    /**
     * @brief Computes the minimum edge degree in a weighted graph.
     * @param graph The weighted edge graph to analyze.
     * @param mode The mode of degree calculation (In, Out, Full).
     * @return The minimum edge degree in the graph.
     * @details This function calculates the minimum degree of edges in the graph, which is defined as the minimum number of edges connected to any node.
     * @note The degree of a node is the number of edges connected to it. Depending on the mode, it can be the in-degree, out-degree, or full degree.
     * @return The pair <name,value>. 0 if the graph has no edges.
     */
    std::pair<std::string,int> minEdgeDegree(const WeightedEdgeGraph& graph, DegreeMode mode = DegreeMode::Full);
    /**
     * @brief Computes the average edge degree (weighted) in a weighted graph.
     * @param graph The weighted edge graph to analyze.
     * @param mode The mode of degree calculation (In, Out, Full).
     * @return The average edge degree (weighted) in the graph.
     * @details This function calculates the average degree of edges in the graph, which is defined as the average sum of weights of edges connected to any node.
     * @note The degree of a node is the sum of weights of edges connected to it. Depending on the mode, it can be the in-degree, out-degree, or full degree.
     * @return 0 if the graph has no edges.
     */
    double averageEdgeDegreeWeighted(const WeightedEdgeGraph& graph, DegreeMode mode = DegreeMode::Full);
    /**
     * @brief Computes the maximum edge degree (weighted) in a weighted graph.
     * @param graph The weighted edge graph to analyze.
     * @param mode The mode of degree calculation (In, Out, Full).
     * @details This function calculates the maximum degree of edges in the graph, which is defined as the maximum sum of weights of edges connected to any node.
     * @note The degree of a node is the sum of weights of edges connected to it. Depending on the mode, it can be the in-degree, out-degree, or full degree.
     * @return The pair <name,value> node that has the maximum edge degree. 0 if the graph has no edges.
     */
    std::pair<std::string,double> maxEdgeDegreeWeighted(const WeightedEdgeGraph& graph, DegreeMode mode = DegreeMode::Full);
    /**
     * @brief Computes the minimum edge degree (weighted) in a weighted graph.
     * @param graph The weighted edge graph to analyze.
     * @param mode The mode of degree calculation (In, Out, Full).
     * @details This function calculates the minimum degree of edges in the graph, which is defined as the minimum sum of weights of edges connected to any node.
     * @note The degree of a node is the sum of weights of edges connected to it. Depending on the mode, it can be the in-degree, out-degree, or full degree.
     * @return The pair <name,value> node that has the minimum edge degree. 0 if the graph has no edges.
     * @note This function is similar to minEdgeDegree, but it considers the weights of the edges.
     */
    std::pair<std::string,double> minEdgeDegreeWeighted(const WeightedEdgeGraph& graph, DegreeMode mode = DegreeMode::Full);

    /**
     * @brief Computes the average strength centrality in the graph.
     * @param graph The weighted edge graph to analyze.
     * @param mode The mode of the strength calculation (In, Out, Full).
     * @return the average strength centrality measure in the graph
     * @details This function computes the average strength centrality, where the strength is computed as:
     * strength(v) = sum of weights of edges incident to v      
     */
    double averageStrengthCentrality(const WeightedEdgeGraph& graph, DegreeMode mode = DegreeMode::Full);

    /**
     * @brief Computes the maximum strength centrality in the graph.
     * @param graph The weighted edge graph to analyze.
     * @param mode The mode of the strength calculation (In, Out, Full).
     * @return the maximum strength centrality measure in the graph and a node that has that maximum centrality 
     * @details This function computes the maximum strength centrality, where the strength is computed as:
     * strength(v) = sum of weights of edges incident to v      
     */
    std::pair<std::string,double> maxStrengthCentrality(const WeightedEdgeGraph& graph, DegreeMode mode = DegreeMode::Full);

    /**
     * @brief Computes the minimum strength centrality in the graph.
     * @param graph The weighted edge graph to analyze.
     * @param mode The mode of strength calculation (In, Out, Full).
     * @return the minimum strength centrality measure in the graph and a node that has that minimum centrality 
     * @details This function computes the minimum strength centrality, where the strength is computed as:
     * strength(v) = sum of weights of edges incident to v      
     */
    std::pair<std::string,double> minStrengthCentrality(const WeightedEdgeGraph& graph, DegreeMode mode = DegreeMode::Full);

    /**
     * @brief Computes the local clustering coefficient for a node in a weighted edge graph.
     * @param graph The weighted edge graph to analyze.
     * @param v The node for which to compute the local clustering coefficient.
     * @return The local clustering coefficient for the node v.
     * @details The local clustering coefficient is computed as the Barrat's local clustering coefficient
     */
    double weightedLocalClustering(const WeightedEdgeGraph& graph, int v);

    /**
     * @brief Computes the global clustering coefficient for a weighted edge graph.
     * @param graph The weighted edge graph to analyze.
     * @return The global clustering coefficient for the graph.
     * @details The global clustering coefficient is computed as the average of the local clustering coefficients of all the nodes in the graph.
     */
    double weightedGlobalClustering(const WeightedEdgeGraph& graph);

    /**
     * @brief Computes the weigth of a path in a weighted edge graph.
     * @param graph The weighted edge graph to analyze.
     * @param path A vector of node indices representing the path.
     * @return The total weight of the path.
     * @details The weight of the path is computed as the sum of the weights of the edges in the path.
     * @note The path is represented as a vector of node indices, where each index corresponds to a node in the graph.
     * @throw std::out_of_range if any node index in the path is out of range of the graph's nodes.
     * @throw std::invalid_argument if the path is not valid (e.g., edges do not exist between consecutive nodes).
     */
    double weightedPathWeight(const WeightedEdgeGraph& graph, const std::vector<int>& path);

    /**
     * @brief Control if the graph contains a cycle.
     * @param graph The weighted edge graph to analyze.
     * @return true if the graph contains a cycle, false otherwise.
     * @details This function uses a depth-first search (DFS) algorithm to detect cycles in the graph.
     * @note The function assumes that the graph is directed.
     * @throw std::invalid_argument if the graph is empty.
     */
    bool hasCycle(const WeightedEdgeGraph& graph);

    /**
     * @brief Computes the unweighted shortest path with BFS from a source node to all other nodes in a weighted edge graph.
     * @param graph The weighted edge graph to analyze.
     * @param source The source node from which to compute the shortest paths.
     * @return A vector pairs containing the shortest path lengths and the corresponding paths from the source node to all other nodes.
     * @note The paths are represented as vectors of node indices.
     * @throw std::out_of_range if the source node index is out of range.
     * @throw std::invalid_argument if the graph is empty.
     * @details This function uses a breadth-first search (BFS) algorithm to compute the shortest path lengths in an unweighted graph.
     * @note The graph is treated as unweighted for the purpose of this function, meaning
     * that all edges are considered to have equal weight (1).
     */
    std::vector< 
        std::pair< 
            int,std::vector<int> 
        > 
    > allUnweightedShortestPathFromSourceBFS(const WeightedEdgeGraph& graph, int source);


    /**
     * @brief Computes the weighted shortest path with Dijkstra's algorithm from a source node to all other nodes in a weighted edge graph.
     * @param graph The weighted edge graph to analyze.
     * @param source The source node from which to compute the shortest paths.
     * @return A vector pairs containing the shortest path lengths and the corresponding paths from the source node to all other nodes.
     * @note The paths are represented as vectors of node indices.
     * @throw std::out_of_range if the source node index is out of range.
     * @throw std::invalid_argument if the graph is empty.
     * @details This function uses Dijkstra's algorithm to compute the shortest path lengths in a weighted graph.
     * @note The graph is treated as weighted, meaning that edges can have different weights.
     * @warning This function assumes that the graph does not contain negative weight cycles. And that the weights are non-negative.
     * @todo add control for negative weights
     */
    std::vector< 
        std::pair< 
            int,std::vector<int>
        >
    > allWeightedShortestPathFromSourceDijkstra(const WeightedEdgeGraph& graph, int source);

    /**
     * @brief Computes the weighted shortest path with Bellman-Ford algorithm from a source node to all other nodes in a weighted edge graph.
     * @param graph The weighted edge graph to analyze.
     * @param source The source node from which to compute the shortest paths.
     * @return A vector pairs containing the shortest path lengths and the corresponding paths from the source node to all other nodes.
     * @note The paths are represented as vectors of node indices.
     * @throw std::out_of_range if the source node index is out of range.
     * @throw std::invalid_argument if the graph is empty.
     * @details This function uses the Bellman-Ford algorithm to compute the shortest path lengths
     * in a weighted graph.
     * @note The graph is treated as weighted, meaning that edges can have different weights.
     */
    std::vector< 
        std::pair< 
            int,std::vector<int>
        >
    > allWeightedShortestPathFromSourceBellmanFord(const WeightedEdgeGraph& graph, int source);


}