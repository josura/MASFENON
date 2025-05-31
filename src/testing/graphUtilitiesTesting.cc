#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "utils/graphUtilities.hxx"

class GraphUtilitiesTesting : public ::testing::Test {
    protected:
        void SetUp() override {
            // Initialize graphs for testing
            graph1 = new WeightedEdgeGraph(); // Empty graph
            graph2 = new WeightedEdgeGraph(5); // Graph with 5 nodes, no edges
            graph3 = new WeightedEdgeGraph(nodeNames); // Graph with 5 nodes
            graph4 = new WeightedEdgeGraph(nodeNames, nodeValues); // Graph with 5 nodes and values

            // Add edges to graph3
            graph3->addEdge("node1", "node2", 1.0);
            graph3->addEdge("node2", "node3", 2.0);
            graph3->addEdge("node3", "node4", 3.0);
            graph3->addEdge("node4", "node5", 4.0);
            graph3->addEdge("node5", "node1", 5.0);
            graph3->addEdge("node1", "node3", 1.5);
            graph3->addEdge("node2", "node4", 2.5);
            graph3->addEdge("node3", "node5", 3.5);
            graph3->addEdge("node4", "node1", 4.5);
            graph3->addEdge("node5", "node2", 5.5);
            // Add edges to graph4
            graph4->addEdge("node1", "node2", 1.0);
            graph4->addEdge("node2", "node3", 2.0);
            graph4->addEdge("node3", "node4", 3.0);
            graph4->addEdge("node4", "node5", 4.0);
            graph4->addEdge("node5", "node1", 5.0);
            graph4->addEdge("node1", "node3", 1.5);
            graph4->addEdge("node2", "node4", 2.5);
            graph4->addEdge("node3", "node5", 3.5);
            graph4->addEdge("node4", "node1", 4.5);
            graph4->addEdge("node5", "node2", 5.5);
        }

        void TearDown() override {
            // Cleanup code if needed
            delete graph1;
            delete graph2;
            delete graph3;
            delete graph4;
        }

        // member variables for the test class
        WeightedEdgeGraph *graph1; // graph with zero edges and nodes
        WeightedEdgeGraph *graph2; // graph with 5 nodes and no edges
        WeightedEdgeGraph *graph3; // graph with 5 nodes, no values and 10 edges
        WeightedEdgeGraph *graph4; // graph with 5 nodes and values and 10 edges

        std::vector<std::string> nodeNames{"node1","node2","node3","node4","node5"};
        std::vector<double> nodeValues{0.3,4.1,3.8,8.2,9.5};
};

TEST_F(GraphUtilitiesTesting, testAverageEdgeWeight) {
    // Test average edge weight for an empty graph
    EXPECT_DOUBLE_EQ(weighed_graph_metrics::averageEdgeWeight(*graph1), 0.0);

    // Test average edge weight for a graph with no edges
    EXPECT_DOUBLE_EQ(weighed_graph_metrics::averageEdgeWeight(*graph2), 0.0);

    // Test average edge weight for a graph with edges
    double expectedAverage = (1.0 + 2.0 + 3.0 + 4.0 + 5.0 + 1.5 + 2.5 + 3.5 + 4.5 + 5.5) / 10;
    EXPECT_DOUBLE_EQ(weighed_graph_metrics::averageEdgeWeight(*graph3), expectedAverage);
    
    // Test average edge weight for a graph with values
    EXPECT_DOUBLE_EQ(weighed_graph_metrics::averageEdgeWeight(*graph4), expectedAverage);
}

TEST_F(GraphUtilitiesTesting, testMaxEdgeWeight) {
    // Test max edge weight for an empty graph
    auto maxEdge1 = weighed_graph_metrics::maxEdgeWeight(*graph1);
    EXPECT_EQ(maxEdge1.first, "");
    EXPECT_DOUBLE_EQ(maxEdge1.second, 0.0);

    // Test max edge weight for a graph with no edges
    auto maxEdge2 = weighed_graph_metrics::maxEdgeWeight(*graph2);
    EXPECT_EQ(maxEdge2.first, "");
    EXPECT_DOUBLE_EQ(maxEdge2.second, 0.0);

    // Test max edge weight for a graph with edges
    auto maxEdge3 = weighed_graph_metrics::maxEdgeWeight(*graph3);
    EXPECT_EQ(maxEdge3.first, "node5");
    EXPECT_DOUBLE_EQ(maxEdge3.second, 5.5);
    
    // Test max edge weight for a graph with values
    auto maxEdge4 = weighed_graph_metrics::maxEdgeWeight(*graph4);
    EXPECT_EQ(maxEdge4.first, "node5");
    EXPECT_DOUBLE_EQ(maxEdge4.second, 5.5);
}