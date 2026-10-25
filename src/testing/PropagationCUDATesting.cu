#include "computation/PropagationCUDA.cuh"

#include <cuda_runtime.h>
#include <gtest/gtest.h>

#include <vector>

namespace {

class PropagationCUDATest : public ::testing::Test {
protected:
    void TearDown() override {
        cudaFree(d_A);
        cudaFree(d_x);
        cudaFree(d_y);
    }

    float* d_A = nullptr;
    double* d_x = nullptr;
    double* d_y = nullptr;
};

TEST_F(PropagationCUDATest, MatchesDoublePrecisionAccumulationForFloatMatrix) {
    constexpr int numNodes = 3;
    const std::vector<float> matrix = {
        0.125F, -1.5F, 2.25F,
        3.5F, 0.75F, -0.25F,
        -2.0F, 1.125F, 0.5F,
    };
    const std::vector<double> state = {1.0 / 3.0, -2.75, 4.125};
    std::vector<double> actual(numNodes);
    std::vector<double> expected(numNodes, 0.0);

    for (int row = 0; row < numNodes; ++row) {
        for (int col = 0; col < numNodes; ++col) {
            expected[row] += static_cast<double>(matrix[row * numNodes + col]) * state[col];
        }
    }

    ASSERT_EQ(cudaSuccess, cudaMalloc(&d_A, matrix.size() * sizeof(float)));
    ASSERT_EQ(cudaSuccess, cudaMalloc(&d_x, state.size() * sizeof(double)));
    ASSERT_EQ(cudaSuccess, cudaMalloc(&d_y, actual.size() * sizeof(double)));
    ASSERT_EQ(cudaSuccess, cudaMemcpy(d_A, matrix.data(), matrix.size() * sizeof(float), cudaMemcpyHostToDevice));
    ASSERT_EQ(cudaSuccess, cudaMemcpy(d_x, state.data(), state.size() * sizeof(double), cudaMemcpyHostToDevice));

    runCudaPropagation(d_A, d_x, d_y, numNodes);

    ASSERT_EQ(cudaSuccess, cudaMemcpy(actual.data(), d_y, actual.size() * sizeof(double), cudaMemcpyDeviceToHost));
    for (int row = 0; row < numNodes; ++row) {
        EXPECT_NEAR(expected[row], actual[row], 1e-12);
    }
}

TEST_F(PropagationCUDATest, FusedStepsKeepStateOnDevice) {
    constexpr int numNodes = 2;
    const std::vector<float> matrix = {0.0F, 1.0F, 1.0F, 0.0F};
    const std::vector<double> initialState = {1.0, 2.0};
    const std::vector<double> conservation = {0.0, 0.0};
    std::vector<double> actual(numNodes);

    ASSERT_EQ(cudaSuccess, cudaMalloc(&d_A, matrix.size() * sizeof(float)));
    ASSERT_EQ(cudaSuccess, cudaMalloc(&d_x, initialState.size() * sizeof(double)));
    ASSERT_EQ(cudaSuccess, cudaMalloc(&d_y, actual.size() * sizeof(double)));
    double* d_conservation = nullptr;
    ASSERT_EQ(cudaSuccess, cudaMalloc(&d_conservation, conservation.size() * sizeof(double)));
    ASSERT_EQ(cudaSuccess, cudaMemcpy(d_A, matrix.data(), matrix.size() * sizeof(float), cudaMemcpyHostToDevice));
    ASSERT_EQ(cudaSuccess, cudaMemcpy(d_x, initialState.data(), initialState.size() * sizeof(double), cudaMemcpyHostToDevice));
    ASSERT_EQ(cudaSuccess, cudaMemcpy(d_conservation, conservation.data(), conservation.size() * sizeof(double), cudaMemcpyHostToDevice));

    runCudaFusedPropagation(d_A, d_conservation, d_x, d_y, numNodes, 2, 0.5, 0.5, 0.0, 0, 0.0);
    runCudaFusedPropagation(d_A, d_conservation, d_y, d_x, numNodes, 2, 0.5, 0.5, 0.0, 0, 0.0);

    ASSERT_EQ(cudaSuccess, cudaMemcpy(actual.data(), d_x, actual.size() * sizeof(double), cudaMemcpyDeviceToHost));
    EXPECT_DOUBLE_EQ(0.8125, actual[0]);
    EXPECT_DOUBLE_EQ(0.875, actual[1]);
    ASSERT_EQ(cudaSuccess, cudaFree(d_conservation));
}

} // namespace
