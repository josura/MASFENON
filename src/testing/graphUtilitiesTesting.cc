#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "utils/graphUtilities.hxx"

class GraphUtilitiesTesting : public ::testing::Test {
    protected:
        void SetUp() override {
            // Setup code if needed
        }

        void TearDown() override {
            // Cleanup code if needed
        }

        // member variables for the test class
        WeightedEdgeGraph *graph1; 
        WeightedEdgeGraph *graph2;
        WeightedEdgeGraph *graph3;
};

