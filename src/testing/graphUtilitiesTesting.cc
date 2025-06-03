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

TEST_F(GraphUtilitiesTesting, testMinEdgeWeight) {
    // Test min edge weight for an empty graph
    auto minEdge1 = weighed_graph_metrics::minEdgeWeight(*graph1);
    EXPECT_EQ(minEdge1.first, "");
    EXPECT_DOUBLE_EQ(minEdge1.second, 0.0);

    // Test min edge weight for a graph with no edges
    auto minEdge2 = weighed_graph_metrics::minEdgeWeight(*graph2);
    EXPECT_EQ(minEdge2.first, "");
    EXPECT_DOUBLE_EQ(minEdge2.second, 0.0);

    // Test min edge weight for a graph with edges
    auto minEdge3 = weighed_graph_metrics::minEdgeWeight(*graph3);
    EXPECT_EQ(minEdge3.first, "node1");
    EXPECT_DOUBLE_EQ(minEdge3.second, 1.0);
    
    // Test min edge weight for a graph with values
    auto minEdge4 = weighed_graph_metrics::minEdgeWeight(*graph4);
    EXPECT_EQ(minEdge4.first, "node1");
    EXPECT_DOUBLE_EQ(minEdge4.second, 1.0);
}

TEST_F(GraphUtilitiesTesting, testAverageEdgeDegreeFull) {
    // Test average edge degree for an empty graph
    EXPECT_DOUBLE_EQ(weighed_graph_metrics::averageEdgeDegree(*graph1, weighed_graph_metrics::DegreeMode::Full), 0.0);

    // Test average edge degree for a graph with no edges
    EXPECT_DOUBLE_EQ(weighed_graph_metrics::averageEdgeDegree(*graph2, weighed_graph_metrics::DegreeMode::Full), 0.0);

    // Test average edge degree for a graph with edges
    double expectedAverageDegree = (1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1) / 5; // Each node has degree of 2 on average
    EXPECT_DOUBLE_EQ(weighed_graph_metrics::averageEdgeDegree(*graph3, weighed_graph_metrics::DegreeMode::Full), expectedAverageDegree);
    
    // Test average edge degree for a graph with values
    EXPECT_DOUBLE_EQ(weighed_graph_metrics::averageEdgeDegree(*graph4, weighed_graph_metrics::DegreeMode::Full), expectedAverageDegree);
}

TEST_F(GraphUtilitiesTesting, testAverageEdgeDegreeIn) {
    // Test average edge degree for an empty graph
    EXPECT_DOUBLE_EQ(weighed_graph_metrics::averageEdgeDegree(*graph1, weighed_graph_metrics::DegreeMode::In), 0.0);

    // Test average edge degree for a graph with no edges
    EXPECT_DOUBLE_EQ(weighed_graph_metrics::averageEdgeDegree(*graph2, weighed_graph_metrics::DegreeMode::In), 0.0);

    // Test average edge degree for a graph with edges
    double expectedAverageDegree = (1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1) / 5; // Each node has in-degree of 2 on average
    EXPECT_DOUBLE_EQ(weighed_graph_metrics::averageEdgeDegree(*graph3, weighed_graph_metrics::DegreeMode::In), expectedAverageDegree);
    
    // Test average edge degree for a graph with values
    EXPECT_DOUBLE_EQ(weighed_graph_metrics::averageEdgeDegree(*graph4, weighed_graph_metrics::DegreeMode::In), expectedAverageDegree);
}

TEST_F(GraphUtilitiesTesting, testAverageEdgeDegreeOut) {
    // Test average edge degree for an empty graph
    EXPECT_DOUBLE_EQ(weighed_graph_metrics::averageEdgeDegree(*graph1, weighed_graph_metrics::DegreeMode::Out), 0.0);

    // Test average edge degree for a graph with no edges
    EXPECT_DOUBLE_EQ(weighed_graph_metrics::averageEdgeDegree(*graph2, weighed_graph_metrics::DegreeMode::Out), 0.0);

    // Test average edge degree for a graph with edges
    double expectedAverageDegree = (1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1) / 5; // Each node has out-degree of 2 on average
    EXPECT_DOUBLE_EQ(weighed_graph_metrics::averageEdgeDegree(*graph3, weighed_graph_metrics::DegreeMode::Out), expectedAverageDegree);
    
    // Test average edge degree for a graph with values
    EXPECT_DOUBLE_EQ(weighed_graph_metrics::averageEdgeDegree(*graph4, weighed_graph_metrics::DegreeMode::Out), expectedAverageDegree);
}
// TODO: Also control the node name that are returned in the maxEdgeDegree functions (only one node is returned, not all of them)
TEST_F(GraphUtilitiesTesting, testMaxEdgeDegreeFull) {
    // Test max edge degree for an empty graph
    EXPECT_EQ(weighed_graph_metrics::maxEdgeDegree(*graph1, weighed_graph_metrics::DegreeMode::Full).second, 0);

    // Test max edge degree for a graph with no edges
    EXPECT_EQ(weighed_graph_metrics::maxEdgeDegree(*graph2, weighed_graph_metrics::DegreeMode::Full).second, 0);

    // Test max edge degree for a graph with edges
    EXPECT_EQ(weighed_graph_metrics::maxEdgeDegree(*graph3, weighed_graph_metrics::DegreeMode::Full).second, 4);
    
    // Test max edge degree for a graph with values
    EXPECT_EQ(weighed_graph_metrics::maxEdgeDegree(*graph4, weighed_graph_metrics::DegreeMode::Full).second, 4);
}

TEST_F(GraphUtilitiesTesting, testMaxEdgeDegreeIn) {
    // Test max edge degree for an empty graph
    EXPECT_EQ(weighed_graph_metrics::maxEdgeDegree(*graph1, weighed_graph_metrics::DegreeMode::In).second, 0);

    // Test max edge degree for a graph with no edges
    EXPECT_EQ(weighed_graph_metrics::maxEdgeDegree(*graph2, weighed_graph_metrics::DegreeMode::In).second, 0);

    // Test max edge degree for a graph with edges
    EXPECT_EQ(weighed_graph_metrics::maxEdgeDegree(*graph3, weighed_graph_metrics::DegreeMode::In).second, 2);
    
    // Test max edge degree for a graph with values
    EXPECT_EQ(weighed_graph_metrics::maxEdgeDegree(*graph4, weighed_graph_metrics::DegreeMode::In).second, 2);
}

TEST_F(GraphUtilitiesTesting, testMaxEdgeDegreeOut) {
    // Test max edge degree for an empty graph
    EXPECT_EQ(weighed_graph_metrics::maxEdgeDegree(*graph1, weighed_graph_metrics::DegreeMode::Out).second, 0);

    // Test max edge degree for a graph with no edges
    EXPECT_EQ(weighed_graph_metrics::maxEdgeDegree(*graph2, weighed_graph_metrics::DegreeMode::Out).second, 0);

    // Test max edge degree for a graph with edges
    EXPECT_EQ(weighed_graph_metrics::maxEdgeDegree(*graph3, weighed_graph_metrics::DegreeMode::Out).second, 2);
    
    // Test max edge degree for a graph with values
    EXPECT_EQ(weighed_graph_metrics::maxEdgeDegree(*graph4, weighed_graph_metrics::DegreeMode::Out).second, 2);
}

TEST_F(GraphUtilitiesTesting, testMinEdgeDegreeFull) {
    // Test min edge degree for an empty graph
    EXPECT_EQ(weighed_graph_metrics::minEdgeDegree(*graph1, weighed_graph_metrics::DegreeMode::Full).second, 0);

    // Test min edge degree for a graph with no edges
    EXPECT_EQ(weighed_graph_metrics::minEdgeDegree(*graph2, weighed_graph_metrics::DegreeMode::Full).second, 0);

    // Test min edge degree for a graph with edges
    EXPECT_EQ(weighed_graph_metrics::minEdgeDegree(*graph3, weighed_graph_metrics::DegreeMode::Full).second, 4);
    
    // Test min edge degree for a graph with values
    EXPECT_EQ(weighed_graph_metrics::minEdgeDegree(*graph4, weighed_graph_metrics::DegreeMode::Full).second, 4);
}

TEST_F(GraphUtilitiesTesting, testMinEdgeDegreeIn) {
    // Test min edge degree for an empty graph
    EXPECT_EQ(weighed_graph_metrics::minEdgeDegree(*graph1, weighed_graph_metrics::DegreeMode::In).second, 0);

    // Test min edge degree for a graph with no edges
    EXPECT_EQ(weighed_graph_metrics::minEdgeDegree(*graph2, weighed_graph_metrics::DegreeMode::In).second, 0);

    // Test min edge degree for a graph with edges
    EXPECT_EQ(weighed_graph_metrics::minEdgeDegree(*graph3, weighed_graph_metrics::DegreeMode::In).second, 2);
    
    // Test min edge degree for a graph with values
    EXPECT_EQ(weighed_graph_metrics::minEdgeDegree(*graph4, weighed_graph_metrics::DegreeMode::In).second, 2);
}

TEST_F(GraphUtilitiesTesting, testMinEdgeDegreeOut) {
    // Test min edge degree for an empty graph
    EXPECT_EQ(weighed_graph_metrics::minEdgeDegree(*graph1, weighed_graph_metrics::DegreeMode::Out).second, 0);

    // Test min edge degree for a graph with no edges
    EXPECT_EQ(weighed_graph_metrics::minEdgeDegree(*graph2, weighed_graph_metrics::DegreeMode::Out).second, 0);

    // Test min edge degree for a graph with edges
    EXPECT_EQ(weighed_graph_metrics::minEdgeDegree(*graph3, weighed_graph_metrics::DegreeMode::Out).second, 2);
    
    // Test min edge degree for a graph with values
    EXPECT_EQ(weighed_graph_metrics::minEdgeDegree(*graph4, weighed_graph_metrics::DegreeMode::Out).second, 2);
}

TEST_F(GraphUtilitiesTesting, testAverageEdgeDegreeWeightedFull) {
    // Test average edge degree for an empty graph
    EXPECT_DOUBLE_EQ(weighed_graph_metrics::averageEdgeDegreeWeighted(*graph1, weighed_graph_metrics::DegreeMode::Full), 0.0);

    // Test average edge degree for a graph with no edges
    EXPECT_DOUBLE_EQ(weighed_graph_metrics::averageEdgeDegreeWeighted(*graph2, weighed_graph_metrics::DegreeMode::Full), 0.0);

    // Test average edge degree for a graph with edges
    double expectedAverageDegree = (1.0 + 2.0 + 3.0 + 4.0 + 5.0 + 1.5 + 2.5 + 3.5 + 4.5 + 5.5)*2 / 5;  // Each node has weighted full degree of 13 on average (every edge contributes to the degree of two nodes)
    EXPECT_DOUBLE_EQ(weighed_graph_metrics::averageEdgeDegreeWeighted(*graph3, weighed_graph_metrics::DegreeMode::Full), expectedAverageDegree);
    
    // Test average edge degree for a graph with values
    EXPECT_DOUBLE_EQ(weighed_graph_metrics::averageEdgeDegreeWeighted(*graph4, weighed_graph_metrics::DegreeMode::Full), expectedAverageDegree);
}

TEST_F(GraphUtilitiesTesting, testMaxEdgeDegreeWeightedFull) {
    // Test max edge degree for an empty graph
    auto maxEdge1 = weighed_graph_metrics::maxEdgeDegreeWeighted(*graph1, weighed_graph_metrics::DegreeMode::Full);
    EXPECT_EQ(maxEdge1.first, "");
    EXPECT_DOUBLE_EQ(maxEdge1.second, 0.0);

    // Test max edge degree for a graph with no edges
    auto maxEdge2 = weighed_graph_metrics::maxEdgeDegreeWeighted(*graph2, weighed_graph_metrics::DegreeMode::Full);
    EXPECT_EQ(maxEdge2.first, "");
    EXPECT_DOUBLE_EQ(maxEdge2.second, 0.0);

    // Test max edge degree for a graph with edges
    auto maxEdge3 = weighed_graph_metrics::maxEdgeDegreeWeighted(*graph3, weighed_graph_metrics::DegreeMode::Full);
    EXPECT_EQ(maxEdge3.first, "node5");
    // The node with the maximum degree is "node5" with a weighted full degree of 18
    double expectedMaxDegree1 = (4.0 + 3.5 + 5.0 + 5.5); 
    EXPECT_DOUBLE_EQ(maxEdge3.second, expectedMaxDegree1); 
    
    // Test max edge degree for a graph with values
    auto maxEdge4 = weighed_graph_metrics::maxEdgeDegreeWeighted(*graph4, weighed_graph_metrics::DegreeMode::Full);
    EXPECT_EQ(maxEdge4.first, "node5");
    // The node with the maximum degree is "node5" with a weighted full degree of 18
    double expectedMaxDegree2 = (4.0 + 3.5 + 5.0 + 5.5); 
    EXPECT_DOUBLE_EQ(maxEdge4.second, expectedMaxDegree2);
}

TEST_F(GraphUtilitiesTesting, testMaxEdgeDegreeWeightedIn) {
    // Test max edge degree for an empty graph
    auto maxEdge1 = weighed_graph_metrics::maxEdgeDegreeWeighted(*graph1, weighed_graph_metrics::DegreeMode::In);
    EXPECT_EQ(maxEdge1.first, "");
    EXPECT_DOUBLE_EQ(maxEdge1.second, 0.0);

    // Test max edge degree for a graph with no edges
    auto maxEdge2 = weighed_graph_metrics::maxEdgeDegreeWeighted(*graph2, weighed_graph_metrics::DegreeMode::In);
    EXPECT_EQ(maxEdge2.first, "");
    EXPECT_DOUBLE_EQ(maxEdge2.second, 0.0);

    // Test max edge degree for a graph with edges
    auto maxEdge3 = weighed_graph_metrics::maxEdgeDegreeWeighted(*graph3, weighed_graph_metrics::DegreeMode::In);
    EXPECT_EQ(maxEdge3.first, "node1");
    // The node with the maximum in-degree is "node1" with a weighted in-degree of 9.5
    double expectedMaxDegree1 = (5.0 + 4.5);
    EXPECT_DOUBLE_EQ(maxEdge3.second, expectedMaxDegree1);

    // Test max edge degree for a graph with values
    auto maxEdge4 = weighed_graph_metrics::maxEdgeDegreeWeighted(*graph4, weighed_graph_metrics::DegreeMode::In);
    EXPECT_EQ(maxEdge4.first, "node1");
    // The node with the maximum in-degree is "node1" with a weighted in-degree of 9.5
    double expectedMaxDegree2 = (5.0 + 4.5);
    EXPECT_DOUBLE_EQ(maxEdge4.second, expectedMaxDegree2);
}

TEST_F(GraphUtilitiesTesting, testMaxEdgeDegreeWeightedOut) {
    // Test max edge degree for an empty graph
    auto maxEdge1 = weighed_graph_metrics::maxEdgeDegreeWeighted(*graph1, weighed_graph_metrics::DegreeMode::Out);
    EXPECT_EQ(maxEdge1.first, "");
    EXPECT_DOUBLE_EQ(maxEdge1.second, 0.0);

    // Test max edge degree for a graph with no edges
    auto maxEdge2 = weighed_graph_metrics::maxEdgeDegreeWeighted(*graph2, weighed_graph_metrics::DegreeMode::Out);
    EXPECT_EQ(maxEdge2.first, "");
    EXPECT_DOUBLE_EQ(maxEdge2.second, 0.0);

    // Test max edge degree for a graph with edges
    auto maxEdge3 = weighed_graph_metrics::maxEdgeDegreeWeighted(*graph3, weighed_graph_metrics::DegreeMode::Out);
    EXPECT_EQ(maxEdge3.first, "node5");
    // The node with the maximum out-degree is "node5" with a weighted out-degree of 10.5
    double expectedMaxDegree1 = (5.0 + 5.5);
    EXPECT_DOUBLE_EQ(maxEdge3.second, expectedMaxDegree1);

    // Test max edge degree for a graph with values
    auto maxEdge4 = weighed_graph_metrics::maxEdgeDegreeWeighted(*graph4, weighed_graph_metrics::DegreeMode::Out);
    EXPECT_EQ(maxEdge4.first, "node5");
    // The node with the maximum out-degree is "node1" with a weighted out-degree of 10.5
    double expectedMaxDegree2 = (5.0 + 5.5);
    EXPECT_DOUBLE_EQ(maxEdge4.second, expectedMaxDegree2);
}

TEST_F(GraphUtilitiesTesting, testMinEdgeDegreeWeightedFull) {
    // Test min edge degree for an empty graph
    auto minEdge1 = weighed_graph_metrics::minEdgeDegreeWeighted(*graph1, weighed_graph_metrics::DegreeMode::Full);
    EXPECT_EQ(minEdge1.first, "");
    EXPECT_DOUBLE_EQ(minEdge1.second, 0.0);

    // Test min edge degree for a graph with no edges
    auto minEdge2 = weighed_graph_metrics::minEdgeDegreeWeighted(*graph2, weighed_graph_metrics::DegreeMode::Full);
    EXPECT_EQ(minEdge2.first, "");
    EXPECT_DOUBLE_EQ(minEdge2.second, 0.0);

    // Test min edge degree for a graph with edges
    auto minEdge3 = weighed_graph_metrics::minEdgeDegreeWeighted(*graph3, weighed_graph_metrics::DegreeMode::Full);
    EXPECT_EQ(minEdge3.first, "node3");
    // The node with the minimum full degree is "node3" with a weighted full degree of 10
    double expectedMinDegree1 = (2.0 + 1.5 + 3.0 + 3.5);
    EXPECT_DOUBLE_EQ(minEdge3.second, expectedMinDegree1);

    // Test min edge degree for a graph with values
    auto minEdge4 = weighed_graph_metrics::minEdgeDegreeWeighted(*graph4, weighed_graph_metrics::DegreeMode::Full);
    EXPECT_EQ(minEdge4.first, "node3");
    // The node with the minimum full degree is "node3" with a weighted full degree of 10
    double expectedMinDegree2 = (2.0 + 1.5 + 3.0 + 3.5);
    EXPECT_DOUBLE_EQ(minEdge4.second, expectedMinDegree2);
}

TEST_F(GraphUtilitiesTesting, testMinEdgeDegreeWeightedIn) {
    // Test min edge degree for an empty graph
    auto minEdge1 = weighed_graph_metrics::minEdgeDegreeWeighted(*graph1, weighed_graph_metrics::DegreeMode::In);
    EXPECT_EQ(minEdge1.first, "");
    EXPECT_DOUBLE_EQ(minEdge1.second, 0.0);

    // Test min edge degree for a graph with no edges
    auto minEdge2 = weighed_graph_metrics::minEdgeDegreeWeighted(*graph2, weighed_graph_metrics::DegreeMode::In);
    EXPECT_EQ(minEdge2.first, "");
    EXPECT_DOUBLE_EQ(minEdge2.second, 0.0);

    // Test min edge degree for a graph with edges
    auto minEdge3 = weighed_graph_metrics::minEdgeDegreeWeighted(*graph3, weighed_graph_metrics::DegreeMode::In);
    EXPECT_EQ(minEdge3.first, "node3");
    // The node with the minimum in-degree is "node3" with a weighted in-degree of 3.5
    double expectedMinDegree1 = (2.0 + 1.5);
    EXPECT_DOUBLE_EQ(minEdge3.second, expectedMinDegree1);

    // Test min edge degree for a graph with values
    auto minEdge4 = weighed_graph_metrics::minEdgeDegreeWeighted(*graph4, weighed_graph_metrics::DegreeMode::In);
    EXPECT_EQ(minEdge4.first, "node3");
    // The node with the minimum in-degree is "node3" with a weighted in-degree of 3.5
    double expectedMinDegree2 = (2.0 + 1.5);
    EXPECT_DOUBLE_EQ(minEdge4.second, expectedMinDegree2);
}

TEST_F(GraphUtilitiesTesting, testMinEdgeDegreeWeightedOut) {
    // Test min edge degree for an empty graph
    auto minEdge1 = weighed_graph_metrics::minEdgeDegreeWeighted(*graph1, weighed_graph_metrics::DegreeMode::Out);
    EXPECT_EQ(minEdge1.first, "");
    EXPECT_DOUBLE_EQ(minEdge1.second, 0.0);

    // Test min edge degree for a graph with no edges
    auto minEdge2 = weighed_graph_metrics::minEdgeDegreeWeighted(*graph2, weighed_graph_metrics::DegreeMode::Out);
    EXPECT_EQ(minEdge2.first, "");
    EXPECT_DOUBLE_EQ(minEdge2.second, 0.0);

    // Test min edge degree for a graph with edges
    auto minEdge3 = weighed_graph_metrics::minEdgeDegreeWeighted(*graph3, weighed_graph_metrics::DegreeMode::Out);
    EXPECT_EQ(minEdge3.first, "node1");
    // The node with the minimum out-degree is "node1" with a weighted out-degree of 2.5
    double expectedMinDegree1 = (1.0 + 1.5);
    EXPECT_DOUBLE_EQ(minEdge3.second, expectedMinDegree1);

    // Test min edge degree for a graph with values
    auto minEdge4 = weighed_graph_metrics::minEdgeDegreeWeighted(*graph4, weighed_graph_metrics::DegreeMode::Out);
    EXPECT_EQ(minEdge4.first, "node1");
    // The node with the minimum out-degree is "node1" with a weighted out-degree of 2.5
    double expectedMinDegree2 = (1.0 + 1.5);
    EXPECT_DOUBLE_EQ(minEdge4.second, expectedMinDegree2);
}

