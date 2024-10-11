#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>
#include "computation/DissipationModel.hxx"
#include "computation/DissipationModelPow.hxx"
#include "computation/DissipationModelPeriodic.hxx"
#include "computation/DissipationModelRandom.hxx"
#include "computation/DissipationModelScaled.hxx"

class DissipationModelTesting : public ::testing::Test {
    protected:
        void SetUp() override {
            c0 = new DissipationModelScaled([](double time)->double{return 0;});
            c1 = new DissipationModelPow(2);
        }
        void TearDown() override{
            //delete c0;
            delete c1;
        }
        DissipationModel* c0;       //testing default constructor
        DissipationModel* c1;       //testing general constructor
  
};

TEST_F(DissipationModelTesting, constructorWorksDefault) {
    EXPECT_EQ(c0->getNumEl(),0);
}

TEST_F(DissipationModelTesting, constructorWorksGeneral) {
    EXPECT_EQ(c1->getNumEl(),0);
}
