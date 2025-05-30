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
}