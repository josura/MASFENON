#ifndef PROPAGATION_CUDA_CUH
#define PROPAGATION_CUDA_CUH

// d_A is row-major FP32 interaction data; d_x and d_y are FP64 state vectors.
// extern "C" prevents C++ name mangling so C++ callers can link the launcher.
extern "C" {
    void runCudaPropagation(const float* d_A, const double* d_x, double* d_y, int num_nodes);
    void runCudaFusedPropagation(const float* d_A, const double* d_conservation, double* d_current_state,
                                double* d_next_state, int num_nodes, int dissipation_mode,
                                double dissipation_parameter, double propagation_scale,
                                double conservation_scale, int apply_saturation, double saturation);
    void runCudaScaleCopy(const double* d_source, double* d_destination, double scale);
    void runCudaScaleCopyAsync(const double* d_source, double* d_destination, double scale, void* stream);
}

#endif // PROPAGATION_CUDA_CUH
