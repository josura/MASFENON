#include "PropagationCUDA.cuh"
#include <cuda_runtime.h>
#include <cstdio>

__global__ void mixedPrecisionPropagation(const float* d_A, const double* d_x, double* d_y, int num_nodes) {
    int row = blockIdx.x * blockDim.x + threadIdx.x;
    if (row < num_nodes) {
        double accumulator = 0.0;
        for (int col = 0; col < num_nodes; ++col) {
            const double weight = static_cast<double>(d_A[row * num_nodes + col]);
            accumulator += weight * d_x[col];
        }
        d_y[row] = accumulator;
    }
}

__global__ void fusedMixedPrecisionPropagation(const float* d_A, const double* d_conservation,
                                               const double* d_current_state, double* d_next_state,
                                               int num_nodes, int dissipation_mode,
                                               double dissipation_parameter, double propagation_scale,
                                               double conservation_scale, int apply_saturation,
                                               double saturation) {
    const int row = blockIdx.x * blockDim.x + threadIdx.x;
    if (row >= num_nodes) {
        return;
    }

    double propagated = 0.0;
    for (int col = 0; col < num_nodes; ++col) {
        double state = d_current_state[col];
        if (dissipation_mode == 1) {
            state -= pow(state, dissipation_parameter);
        } else if (dissipation_mode == 2) {
            state *= 1.0 - dissipation_parameter;
        }
        propagated += static_cast<double>(d_A[row * num_nodes + col]) * state;
    }

    double state = d_current_state[row];
    if (dissipation_mode == 1) {
        state -= pow(state, dissipation_parameter);
    } else if (dissipation_mode == 2) {
        state *= 1.0 - dissipation_parameter;
    }
    double output = state + propagation_scale * propagated - conservation_scale * d_conservation[row] * state;
    if (apply_saturation) {
        output = fmin(fmax(output, -saturation), saturation);
    }
    d_next_state[row] = output;
}

__global__ void scaleCopy(const double* d_source, double* d_destination, double scale) {
    *d_destination = *d_source * scale;
}

void runCudaPropagation(const float* d_A, const double* d_x, double* d_y, int num_nodes) {
    if (num_nodes <= 0) {
        return;
    }

    constexpr int threadsPerBlock = 256;
    const int blocksPerGrid = (num_nodes + threadsPerBlock - 1) / threadsPerBlock;
    mixedPrecisionPropagation<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_x, d_y, num_nodes);

    cudaError_t error = cudaGetLastError();
    if (error == cudaSuccess) {
        error = cudaDeviceSynchronize();
    }
    if (error != cudaSuccess) {
        std::fprintf(stderr, "CUDA propagation error: %s\n", cudaGetErrorString(error));
    }
}

void runCudaFusedPropagation(const float* d_A, const double* d_conservation, double* d_current_state,
                             double* d_next_state, int num_nodes, int dissipation_mode,
                             double dissipation_parameter, double propagation_scale,
                             double conservation_scale, int apply_saturation, double saturation) {
    if (num_nodes <= 0) {
        return;
    }
    constexpr int threadsPerBlock = 256;
    const int blocksPerGrid = (num_nodes + threadsPerBlock - 1) / threadsPerBlock;
    fusedMixedPrecisionPropagation<<<blocksPerGrid, threadsPerBlock>>>(
        d_A, d_conservation, d_current_state, d_next_state, num_nodes, dissipation_mode,
        dissipation_parameter, propagation_scale, conservation_scale, apply_saturation, saturation);
    const cudaError_t error = cudaGetLastError();
    if (error != cudaSuccess) {
        std::fprintf(stderr, "CUDA fused propagation error: %s\n", cudaGetErrorString(error));
    }
}

void runCudaScaleCopy(const double* d_source, double* d_destination, double scale) {
    runCudaScaleCopyAsync(d_source, d_destination, scale, nullptr);
}

void runCudaScaleCopyAsync(const double* d_source, double* d_destination, double scale, void* stream) {
    scaleCopy<<<1, 1, 0, static_cast<cudaStream_t>(stream)>>>(d_source, d_destination, scale);
    const cudaError_t error = cudaGetLastError();
    if (error != cudaSuccess) {
        std::fprintf(stderr, "CUDA scale-copy error: %s\n", cudaGetErrorString(error));
    }
}
