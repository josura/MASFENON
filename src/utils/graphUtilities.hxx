/**
 * @file graphUtilities.hxx
 * @ingroup Core
 * @brief Contains utility functions for graph operations.
 * @details The functions are used to work with graphs, and finding metrics
 */
#pragma once
#include <vector>
#include "data_structures/WeightedEdgeGraph.hxx"

/**
 * @namespace weighed_graph_metrics
 * @brief Contains utility functions for calculating metrics on weighted graphs.
 * @details The functions are used to calculate metrics such as the average weight of edges in a graph.
 * @note The functions are designed to work with the WeightedEdgeGraph class.
 */
namespace weighed_graph_metrics {
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
    double maxEdgeWeight(const WeightedEdgeGraph& graph);
    /**
     * @brief Computes the minimum edge weight in a weighted graph.
     * @param graph The weighted edge graph to analyze.
     * @return The minimum edge weight in the graph.
     * @details This function iterates through all edges in the graph and finds the minimum weight.
     */
    double minEdgeWeight(const WeightedEdgeGraph& graph);
    /**
     * @brief Computes the average edge degree in a weighted graph.
     * @param graph The weighted edge graph to analyze.
     * @return The average edge degree in the graph.
     * @details This function calculates the average degree of edges in the graph, which is defined as the average number of edges connected to any node.
     * @note The degree of a node is the number of edges connected to it(in and out).
     * @return 0 if the graph has no edges.
     */
    double averageEdgeDegree(const WeightedEdgeGraph& graph);
    /**
     * @brief Computes the maximum edge degree in a weighted graph.
     * @param graph The weighted edge graph to analyze.
     * @return The maximum edge degree in the graph.
     * @details This function calculates the maximum degree of edges in the graph, which is defined as the maximum number of edges connected to any node.
     * @note The degree of a node is the number of edges connected to it(in and out).
     * @return 0 if the graph has no edges.
     */
    int maxEdgeDegree(const WeightedEdgeGraph& graph);
    /**
     * @brief Computes the minimum edge degree in a weighted graph.
     * @param graph The weighted edge graph to analyze.
     * @return The minimum edge degree in the graph.
     * @details This function calculates the minimum degree of edges in the graph, which is defined as the minimum number of edges connected to any node.
     * @note The degree of a node is the number of edges connected to it(in and out).
     * @return 0 if the graph has no edges.
     */
    int minEdgeDegree(const WeightedEdgeGraph& graph);
    /**
     * @brief Computes the average edge degree (weighted) in a weighted graph.
     * @param graph The weighted edge graph to analyze.
     * @return The average edge degree (weighted) in the graph.
     * @details This function calculates the average degree of edges in the graph, which is defined as the average sum of weights of edges connected to any node.
     * @note The degree of a node is the sum of weights of edges connected to it(in and out).
     * @return 0 if the graph has no edges.
     */
    double averageEdgeDegreeWeighted(const WeightedEdgeGraph& graph);
    /**
     * @brief Computes the maximum edge degree (weighted) in a weighted graph.
     * @param graph The weighted edge graph to analyze.
     * @return The maximum edge degree (weighted) in the graph.
     * @details This function calculates the maximum degree of edges in the graph, which is defined as the maximum sum of weights of edges connected to any node.
     * @note The degree of a node is the sum of weights of edges connected to it(in and out).
     * @return 0 if the graph has no edges.
     */
    int maxEdgeDegreeWeighted(const WeightedEdgeGraph& graph);
    /**
     * @brief Computes the minimum edge degree (weighted) in a weighted graph.
     * @param graph The weighted edge graph to analyze.
     * @return The minimum edge degree (weighted) in the graph.
     * @details This function calculates the minimum degree of edges in the graph, which is defined as the minimum sum of weights of edges connected to any node.
     * @note The degree of a node is the sum of weights of edges connected to it(in and out).
     * @return 0 if the graph has no edges.
     * @note This function is similar to minEdgeDegree, but it considers the weights of the edges.
     */
    int minEdgeDegreeWeighted(const WeightedEdgeGraph& graph);
    /**
     * @brief Computes the maximum edge degree (weighted) in a weighted graph.
     * @param graph The weighted edge graph to analyze.
     * @return The maximum edge degree (weighted) in the graph.
     * @details This function calculates the maximum degree of edges in the graph, which is defined as the maximum sum of weights of edges connected to any node.
     * @note The degree of a node is the sum of weights of edges connected to it(in and out).
     * @return 0 if the graph has no edges.
     */
    double maxEdgeDegreeWeighted(const WeightedEdgeGraph& graph);
}