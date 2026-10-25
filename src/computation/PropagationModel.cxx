/**
 * @file PropagationModel.cxx 
 * @ingroup Core
 * @brief Implements the PropagationModel class used for managing propagation dynamics for the computation of the perturbation in MASFENON.
 * @details The PropagationModel class provides methods for applying propagation logic to the perturbation computation.
 * @details The propagation class uses a scale function to determine the scaling of the propagation term. The scale function can be set and modified as needed.
 */
#include "computation/PropagationModel.hxx"

#ifdef USE_CUDA
#include "computation/PropagationCUDA.cuh"
#include <cuda_runtime.h>

#include <stdexcept>
#include <vector>

void PropagationModel::initializeCudaPropagation(const arma::dmat& interactionMatrix) {
    cudaNumNodes = static_cast<int>(interactionMatrix.n_rows);
    if (interactionMatrix.n_cols != interactionMatrix.n_rows) {
        throw std::invalid_argument("CUDA propagation requires a square interaction matrix");
    }
    if (cudaNumNodes == 0) {
        return;
    }

    std::vector<float> fp32Matrix(cudaNumNodes * cudaNumNodes);
    for (int row = 0; row < cudaNumNodes; ++row) {
        for (int col = 0; col < cudaNumNodes; ++col) {
            fp32Matrix[row * cudaNumNodes + col] = static_cast<float>(interactionMatrix(row, col));
        }
    }

    cudaError_t error = cudaMalloc(&d_interactionMatrix, fp32Matrix.size() * sizeof(float));
    if (error == cudaSuccess) {
        error = cudaMalloc(&d_currentState, cudaNumNodes * sizeof(double));
    }
    if (error == cudaSuccess) {
        error = cudaMalloc(&d_nextState, cudaNumNodes * sizeof(double));
    }
    if (error == cudaSuccess) {
        error = cudaMalloc(&d_conservation, cudaNumNodes * sizeof(double));
    }
    if (error == cudaSuccess) {
        error = cudaMemcpy(d_interactionMatrix, fp32Matrix.data(), fp32Matrix.size() * sizeof(float), cudaMemcpyHostToDevice);
    }
    if (error != cudaSuccess) {
        cudaFree(d_nextState);
        cudaFree(d_currentState);
        cudaFree(d_conservation);
        cudaFree(d_interactionMatrix);
        d_nextState = nullptr;
        d_currentState = nullptr;
        d_interactionMatrix = nullptr;
        d_conservation = nullptr;
        cudaNumNodes = 0;
        throw std::runtime_error(cudaGetErrorString(error));
    }
}

void PropagationModel::uploadCudaState(const arma::Col<double>& input) {
    if (static_cast<int>(input.n_elem) != cudaNumNodes) throw std::invalid_argument("CUDA propagation input size does not match the interaction matrix");
    const cudaError_t error = cudaMemcpy(d_currentState, input.memptr(), cudaNumNodes * sizeof(double), cudaMemcpyHostToDevice);
    if (error != cudaSuccess) throw std::runtime_error(cudaGetErrorString(error));
}

void PropagationModel::setCudaConservation(const arma::Col<double>& conservation) {
    if (static_cast<int>(conservation.n_elem) != cudaNumNodes) throw std::invalid_argument("CUDA conservation vector size does not match the interaction matrix");
    const cudaError_t error = cudaMemcpy(d_conservation, conservation.memptr(), cudaNumNodes * sizeof(double), cudaMemcpyHostToDevice);
    if (error != cudaSuccess) throw std::runtime_error(cudaGetErrorString(error));
}

void PropagationModel::runCudaFusedStep(int dissipationMode, double dissipationParameter, double propagationScale,
                                        double conservationScale, bool saturation, double saturationValue) {
    runCudaFusedPropagation(d_interactionMatrix, d_conservation, d_currentState, d_nextState, cudaNumNodes,
                            dissipationMode, dissipationParameter, propagationScale, conservationScale,
                            saturation, saturationValue);
    std::swap(d_currentState, d_nextState);
}

arma::Col<double> PropagationModel::downloadCudaState() const {
    arma::Col<double> output(cudaNumNodes);
    const cudaError_t error = cudaMemcpy(output.memptr(), d_currentState, cudaNumNodes * sizeof(double), cudaMemcpyDeviceToHost);
    if (error != cudaSuccess) throw std::runtime_error(cudaGetErrorString(error));
    return output;
}

arma::Col<double> PropagationModel::runCudaPropagationStep(const arma::Col<double>& input) const {
    if (static_cast<int>(input.n_elem) != cudaNumNodes) {
        throw std::invalid_argument("CUDA propagation input size does not match the interaction matrix");
    }
    if (cudaNumNodes == 0) {
        return arma::Col<double>();
    }

    cudaError_t error = cudaMemcpy(d_currentState, input.memptr(), cudaNumNodes * sizeof(double), cudaMemcpyHostToDevice);
    if (error != cudaSuccess) {
        throw std::runtime_error(cudaGetErrorString(error));
    }
    runCudaPropagation(d_interactionMatrix, d_currentState, d_nextState, cudaNumNodes);

    arma::Col<double> output(cudaNumNodes);
    error = cudaMemcpy(output.memptr(), d_nextState, cudaNumNodes * sizeof(double), cudaMemcpyDeviceToHost);
    if (error != cudaSuccess) {
        throw std::runtime_error(cudaGetErrorString(error));
    }
    return output;
}

PropagationModel::~PropagationModel() {
    cudaFree(d_nextState);
    cudaFree(d_currentState);
    cudaFree(d_conservation);
    cudaFree(d_interactionMatrix);
}
#else
PropagationModel::~PropagationModel() = default;
#endif
