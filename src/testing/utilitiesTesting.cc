/**
 * @file utilitiesTesting.cc
 * @ingroup Testing
 * @brief Contains unit tests for the utilities functions in MASFENON.
 * @details The tests cover various functionalities such as file reading, dissipation scaling functions, and more.
 * @warning This file is intended for testing purposes only and should not be used in production code.
 * @todo Add more tests for other utility functions.
 * @todo Add more tests for getting dissipation scaling functions with different parameters. Especially controlling the behaviour with different number of parameters(throwing exception inside)
 * @see utilities.hxx
 * @see utilities.cxx
 */

#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>
#include "utils/utilities.hxx"

class utilitiesTesting : public ::testing::Test {
    protected:
        void SetUp() override {
            //initialize resources
            
            
        }
        void TearDown() override{
            //delete resources
        }
        // protected variables here
        std::vector<std::string> orderedNodeNames_t0 = {"a0","b0","c0","e0","d0","f0"}; 

  
};

TEST_F(utilitiesTesting, testWorks) {
    EXPECT_EQ(1, 1);
    
}

TEST_F(utilitiesTesting, dissipationScalingFunctionFromFileWorksFullParametersOrdered) {
    std::string fileName = "../data/testdata/testHeterogeneousTemporalGraphMultipleInteractions/parameters/dissipationParameters/t0.tsv";
    auto scaleFunction = dissipationScalingFunctionFromFile(fileName, orderedNodeNames_t0);
    std::vector<double> expectedValues_time0 = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    arma::Col<double> result = scaleFunction(0.0);
    EXPECT_EQ(result.n_elem, expectedValues_time0.size()) << "Result size does not match expected size";
    for (size_t i = 0; i < expectedValues_time0.size(); ++i) {
        EXPECT_DOUBLE_EQ(result(i), expectedValues_time0[i]) << "Mismatch at index " << i;
    }
}
    