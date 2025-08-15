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
        std::vector<std::string> orderedNodeNames_t1 = {"a1","b1","c1","d1","e1","f1"};
        std::vector<std::string> orderedNodeNames_t2 = {"a2","b2","c2","d2","e2","f2"};
        std::vector<std::string> orderedNodeNames_t3 = {"a3","b3","c3","d3","e3","f3"}; 

  
};

TEST_F(utilitiesTesting, testWorks) {
    EXPECT_EQ(1, 1);
    
}

TEST_F(utilitiesTesting, dissipationScalingFunctionFromFileWorksFullParametersOrdered) {
    std::string fileName = "../data/testdata/testHeterogeneousTemporalGraphMultipleInteractions/parameters/dissipationParameters/t0.tsv";
    auto scaleFunction = dissipationScalingFunctionFromFile(fileName, orderedNodeNames_t0);
    // the custom function will return the following for t0
    // - first parameter when t<=5, which is 0.0 for all nodes
    // - second parameter when 5<t<=6, which is (0,-1,0,-1,0,-1) for the nodes in the order of orderedNodeNames_t0
    // - third parameter when 6<t<=10, which is (1,2,3,4,5,6) for the nodes in the order of orderedNodeNames_t0
    std::vector<double> expectedValues_time0 = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    arma::Col<double> result_time0 = scaleFunction(0.0);
    EXPECT_EQ(result_time0.n_elem, expectedValues_time0.size()) << "Result size does not match expected size";
    for (size_t i = 0; i < expectedValues_time0.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time0(i), expectedValues_time0[i]) << "Mismatch at index " << i;
    }
    arma::Col<double> result_time5 = scaleFunction(5.0);
    EXPECT_EQ(result_time5.n_elem, expectedValues_time0.size()) << "Result size does not match expected size";
    for (size_t i = 0; i < expectedValues_time0.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5(i), expectedValues_time0[i]) << "Mismatch at index " << i;
    }
    
    std::vector<double> expectedValues_time5_1 = {0.0, -1.0, 0.0, -1.0, 0.0, -1.0};
    arma::Col<double> result_time5_1 = scaleFunction(5.1);
    EXPECT_EQ(result_time5_1.n_elem, expectedValues_time5_1.size()) << "Result size does not match expected size";
    for (size_t i = 0; i < expectedValues_time5_1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5_1(i), expectedValues_time5_1[i]) << "Mismatch at index " << i;
    }
    arma::Col<double> result_time6 = scaleFunction(6);
    EXPECT_EQ(result_time6.n_elem, expectedValues_time5_1.size()) << "Result size does not match expected size";
    for (size_t i = 0; i < expectedValues_time5_1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time6(i), expectedValues_time5_1[i]) << "Mismatch at index " << i;
    }
    std::vector<double> expectedValues_time6_1 = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    arma::Col<double> result_time6_1 = scaleFunction(6.1);
    EXPECT_EQ(result_time6_1.n_elem, expectedValues_time6_1.size()) << "Result size does not match expected size";
    for (size_t i = 0; i < expectedValues_time6_1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time6_1(i), expectedValues_time6_1[i]) << "Mismatch at index " << i;
    }
    arma::Col<double> result_time10 = scaleFunction(10);
    EXPECT_EQ(result_time10.n_elem, expectedValues_time6_1.size()) << "Result size does not match expected size";
    for (size_t i = 0; i < expectedValues_time6_1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time10(i), expectedValues_time6_1[i]) << "Mismatch at index " << i;
    }
}

TEST_F(utilitiesTesting, dissipationScalingFunctionFromFileWorksPartialParametersUnorderedPartial) {
    std::string fileName = "../data/testdata/testHeterogeneousTemporalGraphMultipleInteractions/parameters/dissipationParametersUnorderedPartial/t0.tsv";
    auto scaleFunction = dissipationScalingFunctionFromFile(fileName, orderedNodeNames_t0);
    // the custom function will return the following for t0
    // - first parameter when t<=5, which is 0.0 for the nodes specified in the file (a0, b0, c0) and 0.5(default) for the others (d0, e0, f0)
    // - second parameter when 5<t<=6, which is (0,-1,0,0.5,0.5,0.5) for the nodes in the order of orderedNodeNames_t0
    // - third parameter when 6<t<=10, which is (1,2,3,0.5,0.5,0.5) for the nodes in the order of orderedNodeNames_t0
    std::vector<double> expectedValues_time0 = {0.0, 0.0, 0.0, 0.5, 0.5, 0.5};
    arma::Col<double> result_time0 = scaleFunction(0.0);
    EXPECT_EQ(result_time0.n_elem, expectedValues_time0.size()) << "Result size does not match expected size";
    for (size_t i = 0; i < expectedValues_time0.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time0(i), expectedValues_time0[i]) << "Mismatch at index " << i;
    }
    arma::Col<double> result_time5 = scaleFunction(5.0);
    EXPECT_EQ(result_time5.n_elem, expectedValues_time0.size()) << "Result size does not match expected size";
    for (size_t i = 0; i < expectedValues_time0.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5(i), expectedValues_time0[i]) << "Mismatch at index " << i;
    }
    std::vector<double> expectedValues_time5_1 = {0.0, -1.0, 0.0, 0.5, 0.5, 0.5};
    arma::Col<double> result_time5_1 = scaleFunction(5.1);
    EXPECT_EQ(result_time5_1.n_elem, expectedValues_time5_1.size()) << "Result size does not match expected size";
    for (size_t i = 0; i < expectedValues_time5_1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5_1(i), expectedValues_time5_1[i]) << "Mismatch at index " << i;
    }
    arma::Col<double> result_time6 = scaleFunction(6);
    EXPECT_EQ(result_time6.n_elem, expectedValues_time5_1.size()) << "Result size does not match expected size";
    for (size_t i = 0; i < expectedValues_time5_1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time6(i), expectedValues_time5_1[i]) << "Mismatch at index " << i;
    }
    std::vector<double> expectedValues_time6_1 = {1.0, 2.0, 3.0, 0.5, 0.5, 0.5};
    arma::Col<double> result_time6_1 = scaleFunction(6.1);
    EXPECT_EQ(result_time6_1.n_elem, expectedValues_time6_1.size()) << "Result size does not match expected size";
    for (size_t i = 0; i < expectedValues_time6_1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time6_1(i), expectedValues_time6_1[i]) << "Mismatch at index " << i;
    }
    arma::Col<double> result_time10 = scaleFunction(10);
    EXPECT_EQ(result_time10.n_elem, expectedValues_time6_1.size()) << "Result size does not match expected size";
    for (size_t i = 0; i < expectedValues_time6_1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time10(i), expectedValues_time6_1[i]) << "Mismatch at index " << i;
    }
}


TEST_F(utilitiesTesting, dissipationScalingFunctionFromFileInvalidHeader) {
    std::string fileName = "../data/testdata/testHeterogeneousTemporalGraphMultipleInteractions/parameters/invalidHeaderExample.tsv";
    // std::string fileName = "/home/josura/Projects/ccc/MASFENON/data/testdata/testHeterogeneousTemporalGraphMultipleInteractions/parameters/invalidHeaderExample.tsv";
    EXPECT_THROW(dissipationScalingFunctionFromFile(fileName, orderedNodeNames_t0), std::invalid_argument);
}

TEST_F(utilitiesTesting, dissipationScalingFunctionFromFolderAllFiles){
    std::string folderPath = "../data/testdata/testHeterogeneousTemporalGraphMultipleInteractions/parameters/dissipationParameters";
    std::map<std::string, std::vector<std::string>> typeToOrderedNodeNames = {
        {"t0", orderedNodeNames_t0},
        {"t1", orderedNodeNames_t1},
        {"t2", orderedNodeNames_t2},
        {"t3", orderedNodeNames_t3}
    };
    auto scalingFunctions = dissipationScalingFunctionsFromFolder(folderPath, typeToOrderedNodeNames);
    // Check if the scaling functions are correctly loaded for each type
    EXPECT_TRUE(scalingFunctions.find("t0") != scalingFunctions.end()) << "Scaling function for t0 not found";
    EXPECT_TRUE(scalingFunctions.find("t1") != scalingFunctions.end()) << "Scaling function for t1 not found";
    EXPECT_TRUE(scalingFunctions.find("t2") != scalingFunctions.end()) << "Scaling function for t2 not found";
    EXPECT_TRUE(scalingFunctions.find("t3") != scalingFunctions.end()) << "Scaling function for t3 not found";
    // Check if the scaling functions return the expected values for t0
    auto scaleFunction_t0 = scalingFunctions["t0"];
    std::vector<double> expectedValues_time0 = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    arma::Col<double> result_time0 = scaleFunction_t0(0.0);
    EXPECT_EQ(result_time0.n_elem, expectedValues_time0.size()) << "Result size does not match expected size for t0 at time 0";
    for (size_t i = 0; i < expectedValues_time0.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time0(i), expectedValues_time0[i]) << "Mismatch at index " << i << " for t0 at time 0";
    }
    arma::Col<double> result_time5 = scaleFunction_t0(5.0);
    EXPECT_EQ(result_time5.n_elem, expectedValues_time0.size()) << "Result size does not match expected size for t0 at time 5";
    for (size_t i = 0; i < expectedValues_time0.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5(i), expectedValues_time0[i]) << "Mismatch at index " << i << " for t0 at time 5";
    }
    std::vector<double> expectedValues_time5_1 = {0.0, -1.0, 0.0, -1.0, 0.0, -1.0};
    arma::Col<double> result_time5_1 = scaleFunction_t0(5.1);
    EXPECT_EQ(result_time5_1.n_elem, expectedValues_time5_1.size()) << "Result size does not match expected size for t0 at time 5.1";
    for (size_t i = 0; i < expectedValues_time5_1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5_1(i), expectedValues_time5_1[i]) << "Mismatch at index " << i << " for t0 at time 5.1";
    }
    arma::Col<double> result_time6 = scaleFunction_t0(6);
    EXPECT_EQ(result_time6.n_elem, expectedValues_time5_1.size()) << "Result size does not match expected size for t0 at time 6";
    for (size_t i = 0; i < expectedValues_time5_1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time6(i), expectedValues_time5_1[i]) << "Mismatch at index " << i << " for t0 at time 6";
    }
    std::vector<double> expectedValues_time6_1 = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    arma::Col<double> result_time6_1 = scaleFunction_t0(6.1);
    EXPECT_EQ(result_time6_1.n_elem, expectedValues_time6_1.size()) << "Result size does not match expected size for t0 at time 6.1";
    for (size_t i = 0; i < expectedValues_time6_1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time6_1(i), expectedValues_time6_1[i]) << "Mismatch at index " << i << " for t0 at time 6.1";
    }
    arma::Col<double> result_time10 = scaleFunction_t0(10);
    EXPECT_EQ(result_time10.n_elem, expectedValues_time6_1.size()) << "Result size does not match expected size for t0 at time 10";
    for (size_t i = 0; i < expectedValues_time6_1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time10(i), expectedValues_time6_1[i]) << "Mismatch at index " << i << " for t0 at time 10";
    }
    // Check if the scaling functions return the expected values for t1, the same logic applies, but the middle values for the scaling function are also 0.0, bigger than 6 is 1,2,3,4,5,6
    auto scaleFunction_t1 = scalingFunctions["t1"];
    std::vector<double> expectedValues_time1 = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    arma::Col<double> result_time1 = scaleFunction_t1(0.0);
    EXPECT_EQ(result_time1.n_elem, expectedValues_time1.size()) << "Result size does not match expected size for t1 at time 0";
    for (size_t i = 0; i < expectedValues_time1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time1(i), expectedValues_time1[i]) << "Mismatch at index " << i << " for t1 at time 0";
    }
    arma::Col<double> result_time5_t1 = scaleFunction_t1(5.0);
    EXPECT_EQ(result_time5_t1.n_elem, expectedValues_time1.size()) << "Result size does not match expected size for t1 at time 5";
    for (size_t i = 0; i < expectedValues_time1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5_t1(i), expectedValues_time1[i]) << "Mismatch at index " << i << " for t1 at time 5";
    }
    std::vector<double> expectedValues_time5_1_t1 = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    arma::Col<double> result_time5_1_t1 = scaleFunction_t1(5.1);
    EXPECT_EQ(result_time5_1_t1.n_elem, expectedValues_time5_1_t1.size()) << "Result size does not match expected size for t1 at time 5.1";
    for (size_t i = 0; i < expectedValues_time5_1_t1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5_1_t1(i), expectedValues_time5_1_t1[i]) << "Mismatch at index " << i << " for t1 at time 5.1";
    }
    arma::Col<double> result_time6_t1 = scaleFunction_t1(6);
    EXPECT_EQ(result_time6_t1.n_elem, expectedValues_time5_1_t1.size()) << "Result size does not match expected size for t1 at time 6";
    for (size_t i = 0; i < expectedValues_time5_1_t1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time6_t1(i), expectedValues_time5_1_t1[i]) << "Mismatch at index " << i << " for t1 at time 6";
    }
    std::vector<double> expectedValues_time6_1_t1 = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    arma::Col<double> result_time6_1_t1 = scaleFunction_t1(6.1);
    EXPECT_EQ(result_time6_1_t1.n_elem, expectedValues_time6_1_t1.size()) << "Result size does not match expected size for t1 at time 6.1";
    for (size_t i = 0; i < expectedValues_time6_1_t1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time6_1_t1(i), expectedValues_time6_1_t1[i]) << "Mismatch at index " << i << " for t1 at time 6.1";
    }
    arma::Col<double> result_time10_t1 = scaleFunction_t1(10);
    EXPECT_EQ(result_time10_t1.n_elem, expectedValues_time6_1_t1.size()) << "Result size does not match expected size for t1 at time 10";
    for (size_t i = 0; i < expectedValues_time6_1_t1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time10_t1(i), expectedValues_time6_1_t1[i]) << "Mismatch at index " << i << " for t1 at time 10";
    }
    // Check if the scaling functions return the expected values for t2, the same logic applies, but the middle values for the scaling function are also 0.0, bigger than 6 is 6,5,4,3,2,1
    auto scaleFunction_t2 = scalingFunctions["t2"];
    std::vector<double> expectedValues_time2 = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    arma::Col<double> result_time2 = scaleFunction_t2(0.0);
    EXPECT_EQ(result_time2.n_elem, expectedValues_time2.size()) << "Result size does not match expected size for t2 at time 0";
    for (size_t i = 0; i < expectedValues_time2.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time2(i), expectedValues_time2[i]) << "Mismatch at index " << i << " for t2 at time 0";
    }
    arma::Col<double> result_time5_t2 = scaleFunction_t2(5.0);
    EXPECT_EQ(result_time5_t2.n_elem, expectedValues_time2.size()) << "Result size does not match expected size for t2 at time 5";
    for (size_t i = 0; i < expectedValues_time2.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5_t2(i), expectedValues_time2[i]) << "Mismatch at index " << i << " for t2 at time 5";
    }
    std::vector<double> expectedValues_time5_1_t2 = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    arma::Col<double> result_time5_1_t2 = scaleFunction_t2(5.1);
    EXPECT_EQ(result_time5_1_t2.n_elem, expectedValues_time5_1_t2.size()) << "Result size does not match expected size for t2 at time 5.1";
    for (size_t i = 0; i < expectedValues_time5_1_t2.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5_1_t2(i), expectedValues_time5_1_t2[i]) << "Mismatch at index " << i << " for t2 at time 5.1";
    }
    arma::Col<double> result_time6_t2 = scaleFunction_t2(6);
    EXPECT_EQ(result_time6_t2.n_elem, expectedValues_time5_1_t2.size()) << "Result size does not match expected size for t2 at time 6";
    for (size_t i = 0; i < expectedValues_time5_1_t2.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time6_t2(i), expectedValues_time5_1_t2[i]) << "Mismatch at index " << i << " for t2 at time 6";
    }
    std::vector<double> expectedValues_time6_1_t2 = {6.0, 5.0, 4.0, 3.0, 2.0, 1.0};
    arma::Col<double> result_time6_1_t2 = scaleFunction_t2(6.1);
    EXPECT_EQ(result_time6_1_t2.n_elem, expectedValues_time6_1_t2.size()) << "Result size does not match expected size for t2 at time 6.1";
    for (size_t i = 0; i < expectedValues_time6_1_t2.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time6_1_t2(i), expectedValues_time6_1_t2[i]) << "Mismatch at index " << i << " for t2 at time 6.1";
    }
    arma::Col<double> result_time10_t2 = scaleFunction_t2(10);
    EXPECT_EQ(result_time10_t2.n_elem, expectedValues_time6_1_t2.size()) << "Result size does not match expected size for t2 at time 10";
    for (size_t i = 0; i < expectedValues_time6_1_t2.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time10_t2(i), expectedValues_time6_1_t2[i]) << "Mismatch at index " << i << " for t2 at time 10";
    }
    // Check if the scaling functions return the expected values for t3, the same logic applies, but the middle values for the scaling function are also 0.0, bigger than 6 is 6,5,4,3,2,1
    auto scaleFunction_t3 = scalingFunctions["t3"];
    std::vector<double> expectedValues_time3 = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    arma::Col<double> result_time3 = scaleFunction_t3(0.0);
    EXPECT_EQ(result_time3.n_elem, expectedValues_time3.size()) << "Result size does not match expected size for t3 at time 0";
    for (size_t i = 0; i < expectedValues_time3.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time3(i), expectedValues_time3[i]) << "Mismatch at index " << i << " for t3 at time 0";
    }
    arma::Col<double> result_time5_t3 = scaleFunction_t3(5.0);
    EXPECT_EQ(result_time5_t3.n_elem, expectedValues_time3.size()) << "Result size does not match expected size for t3 at time 5";
    for (size_t i = 0; i < expectedValues_time3.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5_t3(i), expectedValues_time3[i]) << "Mismatch at index " << i << " for t3 at time 5";
    }
    std::vector<double> expectedValues_time5_1_t3 = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    arma::Col<double> result_time5_1_t3 = scaleFunction_t3(5.1);
    EXPECT_EQ(result_time5_1_t3.n_elem, expectedValues_time5_1_t3.size()) << "Result size does not match expected size for t3 at time 5.1";
    for (size_t i = 0; i < expectedValues_time5_1_t3.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5_1_t3(i), expectedValues_time5_1_t3[i]) << "Mismatch at index " << i << " for t3 at time 5.1";
    }
    arma::Col<double> result_time6_t3 = scaleFunction_t3(6);
    EXPECT_EQ(result_time6_t3.n_elem, expectedValues_time5_1_t3.size()) << "Result size does not match expected size for t3 at time 6";
    for (size_t i = 0; i < expectedValues_time5_1_t3.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time6_t3(i), expectedValues_time5_1_t3[i]) << "Mismatch at index " << i << " for t3 at time 6";
    }
    std::vector<double> expectedValues_time6_1_t3 = {6.0, 5.0, 4.0, 3.0, 2.0, 1.0};
    arma::Col<double> result_time6_1_t3 = scaleFunction_t3(6.1);
    EXPECT_EQ(result_time6_1_t3.n_elem, expectedValues_time6_1_t3.size()) << "Result size does not match expected size for t3 at time 6.1";
    for (size_t i = 0; i < expectedValues_time6_1_t3.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time6_1_t3(i), expectedValues_time6_1_t3[i]) << "Mismatch at index " << i << " for t3 at time 6.1";
    }
    arma::Col<double> result_time10_t3 = scaleFunction_t3(10);
    EXPECT_EQ(result_time10_t3.n_elem, expectedValues_time6_1_t3.size()) << "Result size does not match expected size for t3 at time 10";
    for (size_t i = 0; i < expectedValues_time6_1_t3.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time10_t3(i), expectedValues_time6_1_t3[i]) << "Mismatch at index " << i << " for t3 at time 10";
    }
}

TEST_F(utilitiesTesting, dissipationScalingFunctionFromFolderPartialFiles){
    std::string folderPath = "../data/testdata/testHeterogeneousTemporalGraphMultipleInteractions/parameters/dissipationParametersUnorderedPartial";
    std::map<std::string, std::vector<std::string>> typeToOrderedNodeNames = {
        {"t0", orderedNodeNames_t0},
        {"t1", orderedNodeNames_t1},
        {"t2", orderedNodeNames_t2},
        {"t3", orderedNodeNames_t3}
    };
    auto scalingFunctions = dissipationScalingFunctionsFromFolder(folderPath, typeToOrderedNodeNames);
    // t0 and t1 are defined, t2 and t3 are not defined, so they should have the default values
    // t0 is the partial file, so it should have the default values for the nodes not defined in the file
    // t1 is the full file, so it should have the values for all nodes, so same tests as the one before
    EXPECT_TRUE(scalingFunctions.find("t0") != scalingFunctions.end()) << "Scaling function for t0 not found";
    EXPECT_TRUE(scalingFunctions.find("t1") != scalingFunctions.end()) << "Scaling function for t1 not found";
    EXPECT_TRUE(scalingFunctions.find("t2") != scalingFunctions.end()) << "Scaling function for t2 not found";
    EXPECT_TRUE(scalingFunctions.find("t3") != scalingFunctions.end()) << "Scaling function for t3 not found";
    // Check if the scaling functions return the expected values for t0
    auto scaleFunction_t0 = scalingFunctions["t0"];
    std::vector<double> expectedValues_time0 = {0.0, 0.0, 0.0, 0.5, 0.5, 0.5};
    arma::Col<double> result_time0 = scaleFunction_t0(0.0);
    EXPECT_EQ(result_time0.n_elem, expectedValues_time0.size()) << "Result size does not match expected size for t0 at time 0";
    for (size_t i = 0; i < expectedValues_time0.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time0(i), expectedValues_time0[i]) << "Mismatch at index " << i << " for t0 at time 0";
    }
    arma::Col<double> result_time5 = scaleFunction_t0(5.0);
    EXPECT_EQ(result_time5.n_elem, expectedValues_time0.size()) << "Result size does not match expected size for t0 at time 5";
    for (size_t i = 0; i < expectedValues_time0.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5(i), expectedValues_time0[i]) << "Mismatch at index " << i << " for t0 at time 5";
    }
    std::vector<double> expectedValues_time5_1 = {0.0, -1.0, 0.0, 0.5, 0.5, 0.5};
    arma::Col<double> result_time5_1 = scaleFunction_t0(5.1);
    EXPECT_EQ(result_time5_1.n_elem, expectedValues_time5_1.size()) << "Result size does not match expected size for t0 at time 5.1";
    for (size_t i = 0; i < expectedValues_time5_1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5_1(i), expectedValues_time5_1[i]) << "Mismatch at index " << i << " for t0 at time 5.1";
    }
    arma::Col<double> result_time6 = scaleFunction_t0(6);
    EXPECT_EQ(result_time6.n_elem, expectedValues_time5_1.size()) << "Result size does not match expected size for t0 at time 6";
    for (size_t i = 0; i < expectedValues_time5_1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time6(i), expectedValues_time5_1[i]) << "Mismatch at index " << i << " for t0 at time 6";
    }
    std::vector<double> expectedValues_time6_1 = {1.0, 2.0, 3.0, 0.5, 0.5, 0.5};
    arma::Col<double> result_time6_1 = scaleFunction_t0(6.1);
    EXPECT_EQ(result_time6_1.n_elem, expectedValues_time6_1.size()) << "Result size does not match expected size for t0 at time 6.1";
    for (size_t i = 0; i < expectedValues_time6_1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time6_1(i), expectedValues_time6_1[i]) << "Mismatch at index " << i << " for t0 at time 6.1";
    }
    arma::Col<double> result_time10 = scaleFunction_t0(10);
    EXPECT_EQ(result_time10.n_elem, expectedValues_time6_1.size()) << "Result size does not match expected size for t0 at time 10";
    for (size_t i = 0; i < expectedValues_time6_1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time10(i), expectedValues_time6_1[i]) << "Mismatch at index " << i << " for t0 at time 10";
    }
    // Check if the scaling functions return the expected values for t1, the same logic applies, but the middle values for the scaling function are also 0.0, bigger than 6 is 1,2,3,4,5,6
    auto scaleFunction_t1 = scalingFunctions["t1"];
    std::vector<double> expectedValues_time1 = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    arma::Col<double> result_time1 = scaleFunction_t1(0.0);
    EXPECT_EQ(result_time1.n_elem, expectedValues_time1.size()) << "Result size does not match expected size for t1 at time 0";
    for (size_t i = 0; i < expectedValues_time1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time1(i), expectedValues_time1[i]) << "Mismatch at index " << i << " for t1 at time 0";
    }
    arma::Col<double> result_time5_t1 = scaleFunction_t1(5.0);
    EXPECT_EQ(result_time5_t1.n_elem, expectedValues_time1.size()) << "Result size does not match expected size for t1 at time 5";
    for (size_t i = 0; i < expectedValues_time1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5_t1(i), expectedValues_time1[i]) << "Mismatch at index " << i << " for t1 at time 5";
    }
    std::vector<double> expectedValues_time5_1_t1 = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    arma::Col<double> result_time5_1_t1 = scaleFunction_t1(5.1);
    EXPECT_EQ(result_time5_1_t1.n_elem, expectedValues_time5_1_t1.size()) << "Result size does not match expected size for t1 at time 5.1";
    for (size_t i = 0; i < expectedValues_time5_1_t1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5_1_t1(i), expectedValues_time5_1_t1[i]) << "Mismatch at index " << i << " for t1 at time 5.1";
    }
    arma::Col<double> result_time6_t1 = scaleFunction_t1(6);
    EXPECT_EQ(result_time6_t1.n_elem, expectedValues_time5_1_t1.size()) << "Result size does not match expected size for t1 at time 6";
    for (size_t i = 0; i < expectedValues_time5_1_t1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time6_t1(i), expectedValues_time5_1_t1[i]) << "Mismatch at index " << i << " for t1 at time 6";
    }
    std::vector<double> expectedValues_time6_1_t1 = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    arma::Col<double> result_time6_1_t1 = scaleFunction_t1(6.1);
    EXPECT_EQ(result_time6_1_t1.n_elem, expectedValues_time6_1_t1.size()) << "Result size does not match expected size for t1 at time 6.1";
    for (size_t i = 0; i < expectedValues_time6_1_t1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time6_1_t1(i), expectedValues_time6_1_t1[i]) << "Mismatch at index " << i << " for t1 at time 6.1";
    }
    arma::Col<double> result_time10_t1 = scaleFunction_t1(10);
    EXPECT_EQ(result_time10_t1.n_elem, expectedValues_time6_1_t1.size()) << "Result size does not match expected size for t1 at time 10";
    for (size_t i = 0; i < expectedValues_time6_1_t1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time10_t1(i), expectedValues_time6_1_t1[i]) << "Mismatch at index " << i << " for t1 at time 10";
    }
    // Check if the scaling functions return the expected values for t2, everything is 0.5 (default value))
    auto scaleFunction_t2 = scalingFunctions["t2"];
    std::vector<double> expectedValues_time2 = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    arma::Col<double> result_time2 = scaleFunction_t2(0.0);
    EXPECT_EQ(result_time2.n_elem, expectedValues_time2.size()) << "Result size does not match expected size for t2 at time 0";
    for (size_t i = 0; i < expectedValues_time2.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time2(i), expectedValues_time2[i]) << "Mismatch at index " << i << " for t2 at time 0";
    }
    arma::Col<double> result_time5_t2 = scaleFunction_t2(5.0);
    EXPECT_EQ(result_time5_t2.n_elem, expectedValues_time2.size()) << "Result size does not match expected size for t2 at time 5";
    for (size_t i = 0; i < expectedValues_time2.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5_t2(i), expectedValues_time2[i]) << "Mismatch at index " << i << " for t2 at time 5";
    }
    std::vector<double> expectedValues_time5_1_t2 = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    arma::Col<double> result_time5_1_t2 = scaleFunction_t2(5.1);
    EXPECT_EQ(result_time5_1_t2.n_elem, expectedValues_time5_1_t2.size()) << "Result size does not match expected size for t2 at time 5.1";
    for (size_t i = 0; i < expectedValues_time5_1_t2.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5_1_t2(i), expectedValues_time5_1_t2[i]) << "Mismatch at index " << i << " for t2 at time 5.1";
    }
    arma::Col<double> result_time6_t2 = scaleFunction_t2(6);
    EXPECT_EQ(result_time6_t2.n_elem, expectedValues_time5_1_t2.size()) << "Result size does not match expected size for t2 at time 6";
    for (size_t i = 0; i < expectedValues_time5_1_t2.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time6_t2(i), expectedValues_time5_1_t2[i]) << "Mismatch at index " << i << " for t2 at time 6";
    }
    std::vector<double> expectedValues_time6_1_t2 = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    arma::Col<double> result_time6_1_t2 = scaleFunction_t2(6.1);
    EXPECT_EQ(result_time6_1_t2.n_elem, expectedValues_time6_1_t2.size()) << "Result size does not match expected size for t2 at time 6.1";
    for (size_t i = 0; i < expectedValues_time6_1_t2.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time6_1_t2(i), expectedValues_time6_1_t2[i]) << "Mismatch at index " << i << " for t2 at time 6.1";
    }
    arma::Col<double> result_time10_t2 = scaleFunction_t2(10);
    EXPECT_EQ(result_time10_t2.n_elem, expectedValues_time6_1_t2.size()) << "Result size does not match expected size for t2 at time 10";
    for (size_t i = 0; i < expectedValues_time6_1_t2.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time10_t2(i), expectedValues_time6_1_t2[i]) << "Mismatch at index " << i << " for t2 at time 10";
    }
    // Check if the scaling functions return the expected values for t3, everything is 0.5 (default value))
    auto scaleFunction_t3 = scalingFunctions["t3"];
    std::vector<double> expectedValues_time3 = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    arma::Col<double> result_time3 = scaleFunction_t3(0.0);
    EXPECT_EQ(result_time3.n_elem, expectedValues_time3.size()) << "Result size does not match expected size for t3 at time 0";
    for (size_t i = 0; i < expectedValues_time3.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time3(i), expectedValues_time3[i]) << "Mismatch at index " << i << " for t3 at time 0";
    }
    arma::Col<double> result_time5_t3 = scaleFunction_t3(5.0);
    EXPECT_EQ(result_time5_t3.n_elem, expectedValues_time3.size()) << "Result size does not match expected size for t3 at time 5";
    for (size_t i = 0; i < expectedValues_time3.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5_t3(i), expectedValues_time3[i]) << "Mismatch at index " << i << " for t3 at time 5";
    }
    std::vector<double> expectedValues_time5_1_t3 = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    arma::Col<double> result_time5_1_t3 = scaleFunction_t3(5.1);
    EXPECT_EQ(result_time5_1_t3.n_elem, expectedValues_time5_1_t3.size()) << "Result size does not match expected size for t3 at time 5.1";
    for (size_t i = 0; i < expectedValues_time5_1_t3.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5_1_t3(i), expectedValues_time5_1_t3[i]) << "Mismatch at index " << i << " for t3 at time 5.1";
    }
    arma::Col<double> result_time6_t3 = scaleFunction_t3(6);
    EXPECT_EQ(result_time6_t3.n_elem, expectedValues_time5_1_t3.size()) << "Result size does not match expected size for t3 at time 6";
    for (size_t i = 0; i < expectedValues_time5_1_t3.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time6_t3(i), expectedValues_time5_1_t3[i]) << "Mismatch at index " << i << " for t3 at time 6";
    }
    std::vector<double> expectedValues_time6_1_t3 = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    arma::Col<double> result_time6_1_t3 = scaleFunction_t3(6.1);
    EXPECT_EQ(result_time6_1_t3.n_elem, expectedValues_time6_1_t3.size()) << "Result size does not match expected size for t3 at time 6.1";
    for (size_t i = 0; i < expectedValues_time6_1_t3.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time6_1_t3(i), expectedValues_time6_1_t3[i]) << "Mismatch at index " << i << " for t3 at time 6.1";
    }
    arma::Col<double> result_time10_t3 = scaleFunction_t3(10);
    EXPECT_EQ(result_time10_t3.n_elem, expectedValues_time6_1_t3.size()) << "Result size does not match expected size for t3 at time 10";
    for (size_t i = 0; i < expectedValues_time6_1_t3.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time10_t3(i), expectedValues_time6_1_t3[i]) << "Mismatch at index " << i << " for t3 at time 10";
    }
    
}

TEST_F(utilitiesTesting, dissipationScalingFunctionFromFolderZeroFiles){
    std::string folderWithNoTSV = "../docs/manual_markdown";
    std::map<std::string, std::vector<std::string>> typeToOrderedNodeNames = {
        {"t0", orderedNodeNames_t0},
        {"t1", orderedNodeNames_t1},
        {"t2", orderedNodeNames_t2},
        {"t3", orderedNodeNames_t3}
    };
    auto scalingFunctions = dissipationScalingFunctionsFromFolder(folderWithNoTSV, typeToOrderedNodeNames);
    // t0, t1, t2 and t3 are not defined, so they should have the default values
    EXPECT_TRUE(scalingFunctions.find("t0") != scalingFunctions.end()) << "Scaling function for t0 not found";
    EXPECT_TRUE(scalingFunctions.find("t1") != scalingFunctions.end()) << "Scaling function for t1 not found";
    EXPECT_TRUE(scalingFunctions.find("t2") != scalingFunctions.end()) << "Scaling function for t2 not found";
    EXPECT_TRUE(scalingFunctions.find("t3") != scalingFunctions.end()) << "Scaling function for t3 not found";
    // Check if the scaling functions return the expected values for t0
    auto scaleFunction_t0 = scalingFunctions["t0"];
    std::vector<double> expectedValues_time0 = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    arma::Col<double> result_time0 = scaleFunction_t0(0.0);
    EXPECT_EQ(result_time0.n_elem, expectedValues_time0.size()) << "Result size does not match expected size for t0 at time 0";
    for (size_t i = 0; i < expectedValues_time0.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time0(i), expectedValues_time0[i]) << "Mismatch at index " << i << " for t0 at time 0";
    }
    arma::Col<double> result_time5 = scaleFunction_t0(5.0);
    EXPECT_EQ(result_time5.n_elem, expectedValues_time0.size()) << "Result size does not match expected size for t0 at time 5";
    for (size_t i = 0; i < expectedValues_time0.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5(i), expectedValues_time0[i]) << "Mismatch at index " << i << " for t0 at time 5";
    }
    std::vector<double> expectedValues_time10 = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    arma::Col<double> result_time10 = scaleFunction_t0(10);
    EXPECT_EQ(result_time10.n_elem, expectedValues_time10.size()) << "Result size does not match expected size for t0 at time 10";
    for (size_t i = 0; i < expectedValues_time10.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time10(i), expectedValues_time10[i]) << "Mismatch at index " << i << " for t0 at time 10";
    }
    // Check if the scaling functions return the expected values for t1, everything is 0.5 (default value))
    auto scaleFunction_t1 = scalingFunctions["t1"];
    std::vector<double> expectedValues_time1 = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    arma::Col<double> result_time1 = scaleFunction_t1(0.0);
    EXPECT_EQ(result_time1.n_elem, expectedValues_time1.size()) << "Result size does not match expected size for t1 at time 0";
    for (size_t i = 0; i < expectedValues_time1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time1(i), expectedValues_time1[i]) << "Mismatch at index " << i << " for t1 at time 0";
    }
    arma::Col<double> result_time5_t1 = scaleFunction_t1(5.0);
    EXPECT_EQ(result_time5_t1.n_elem, expectedValues_time1.size()) << "Result size does not match expected size for t1 at time 5";
    for (size_t i = 0; i < expectedValues_time1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5_t1(i), expectedValues_time1[i]) << "Mismatch at index " << i << " for t1 at time 5";
    }
    std::vector<double> expectedValues_time5_1_t1 = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    arma::Col<double> result_time5_1_t1 = scaleFunction_t1(5.1);
    EXPECT_EQ(result_time5_1_t1.n_elem, expectedValues_time5_1_t1.size()) << "Result size does not match expected size for t1 at time 5.1";
    for (size_t i = 0; i < expectedValues_time5_1_t1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5_1_t1(i), expectedValues_time5_1_t1[i]) << "Mismatch at index " << i << " for t1 at time 5.1";
    }
    arma::Col<double> result_time_10_t1 = scaleFunction_t1(10);
    EXPECT_EQ(result_time_10_t1.n_elem, expectedValues_time5_1_t1.size()) << "Result size does not match expected size for t1 at time 10";
    for (size_t i = 0; i < expectedValues_time5_1_t1.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time_10_t1(i), expectedValues_time5_1_t1[i]) << "Mismatch at index " << i << " for t1 at time 10";
    }
    // Check if the scaling functions return the expected values for t2, everything is 0.5 (default value))
    auto scaleFunction_t2 = scalingFunctions["t2"];
    std::vector<double> expectedValues_time2 = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    arma::Col<double> result_time2 = scaleFunction_t2(0.0);
    EXPECT_EQ(result_time2.n_elem, expectedValues_time2.size()) << "Result size does not match expected size for t2 at time 0";
    for (size_t i = 0; i < expectedValues_time2.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time2(i), expectedValues_time2[i]) << "Mismatch at index " << i << " for t2 at time 0";
    }
    arma::Col<double> result_time5_t2 = scaleFunction_t2(5.0);
    EXPECT_EQ(result_time5_t2.n_elem, expectedValues_time2.size()) << "Result size does not match expected size for t2 at time 5";
    for (size_t i = 0; i < expectedValues_time2.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5_t2(i), expectedValues_time2[i]) << "Mismatch at index " << i << " for t2 at time 5";
    }
    std::vector<double> expectedValues_time10_t2 = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    arma::Col<double> result_time10_t2 = scaleFunction_t2(10);
    EXPECT_EQ(result_time10_t2.n_elem, expectedValues_time10_t2.size()) << "Result size does not match expected size for t2 at time 10";
    for (size_t i = 0; i < expectedValues_time10_t2.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time10_t2(i), expectedValues_time10_t2[i]) << "Mismatch at index " << i << " for t2 at time 10";
    }
    // Check if the scaling functions return the expected values for t3, everything is 0.5 (default value))
    auto scaleFunction_t3 = scalingFunctions["t3"];
    std::vector<double> expectedValues_time3 = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    arma::Col<double> result_time3 = scaleFunction_t3(0.0);
    EXPECT_EQ(result_time3.n_elem, expectedValues_time3.size()) << "Result size does not match expected size for t3 at time 0";
    for (size_t i = 0; i < expectedValues_time3.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time3(i), expectedValues_time3[i]) << "Mismatch at index " << i << " for t3 at time 0";
    }
    arma::Col<double> result_time5_t3 = scaleFunction_t3(5.0);
    EXPECT_EQ(result_time5_t3.n_elem, expectedValues_time3.size()) << "Result size does not match expected size for t3 at time 5";
    for (size_t i = 0; i < expectedValues_time3.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time5_t3(i), expectedValues_time3[i]) << "Mismatch at index " << i << " for t3 at time 5";
    }
    std::vector<double> expectedValues_time10_t3 = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    arma::Col<double> result_time10_t3 = scaleFunction_t3(10);
    EXPECT_EQ(result_time10_t3.n_elem, expectedValues_time10_t3.size()) << "Result size does not match expected size for t3 at time 10";
    for (size_t i = 0; i < expectedValues_time10_t3.size(); ++i) {
        EXPECT_DOUBLE_EQ(result_time10_t3(i), expectedValues_time10_t3[i]) << "Mismatch at index " << i << " for t3 at time 10";
    }

}