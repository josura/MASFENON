#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>
#include "utils/utilities.hxx"
#include "utils/mathUtilities.hxx"

class utilitiesTesting : public ::testing::Test {
    protected:
        void SetUp() override {
            //initialize resources
            
            
        }
        void TearDown() override{
            //delete resources
        }
        // protected variables here

  
};

TEST_F(utilitiesTesting, testWorks) {
    EXPECT_EQ(1, 1);
    
}