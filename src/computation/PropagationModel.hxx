/**
 * @file PropagationModel.hxx
 * @ingroup Core
 * @brief Defines the PropagationModel class used for managing propagation dynamics for the computation of the perturbation in MASFENON.
 * @details The PropagationModel class provides methods for applying propagation logic to the perturbation computation.
 * @details The propagation class uses a scale function to determine the scaling of the propagation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 * @warning This class is an abstract class and should not be used directly. It is intended to be inherited by other classes that implement specific propagation models.
 * @todo Make it stateful. Related to issue #28 @link
 */
#pragma once
#include <armadillo>
#include <functional>
#include <vector>
#include "data_structures/WeightedEdgeGraph.hxx"

/**
 * @class PropagationModel
 * @brief Abstract class for managing propagation dynamics in MASFENON.
 * @details This class provides methods for applying propagation logic to the perturbation computation.
 * @warning This class is an abstract class and should not be used directly. It is intended to be inherited by other classes that implement specific propagation models.
 * @details The propagation class uses a scale function to determine the scaling of the propagation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 * @todo Make it stateful. Related to issue #28 @link
 */
class PropagationModel{
    protected:
        std::function<double(double)> scaleFunction; ///< The function to scale the propagation term. It takes a double value (time) and returns a double value.
#ifdef USE_CUDA
        float* d_interactionMatrix = nullptr;
        double* d_currentState = nullptr;
        double* d_nextState = nullptr;
        double* d_conservation = nullptr;
        int cudaNumNodes = 0;

        void initializeCudaPropagation(const arma::dmat& interactionMatrix);
        arma::Col<double> runCudaPropagationStep(const arma::Col<double>& input) const;
#endif
    public:
        /**
         * @brief Default destructor for the PropagationModel class. (needed even if the class is abstract)
         * @details Initializes the propagation model with a default scale function.
         */
        virtual ~PropagationModel();
#ifdef USE_CUDA
        virtual bool supportsCudaPipeline() const { return false; }
        void uploadCudaState(const arma::Col<double>& input);
        void setCudaConservation(const arma::Col<double>& conservation);
        void runCudaFusedStep(int dissipationMode, double dissipationParameter, double propagationScale,
                              double conservationScale, bool saturation, double saturationValue);
        arma::Col<double> downloadCudaState() const;
        const double* cudaStatePointer() const { return d_currentState; }
#endif
        //using the scale function as a parameter itself, dependency injection
        /**
         * @brief Propagate the input vector.
         * @param input The input vector to be processed.
         * @param time The current time.
         * @return The output vector after applying the propagation model.
         * @details This function is used to compute the final output of the propagation model.
         */
        virtual arma::Col<double> propagate(arma::Col<double> input, double time) = 0;
        /**
         * @brief Propagation term of the input vector.
         * @param input The input vector to be processed.
         * @param time The current time.
         * @return The propagation term vector.
         * @details This function is used to compute the propagation term of the input vector.
         */
        virtual arma::Col<double> propagationTerm(arma::Col<double> input, double time) = 0;

        //getters and setters
        /**
         * @brief Get the scale function used in the propagation model.
         * @return The scale function used in the propagation model.
         * @details This function is used to get the scale function used in the propagation model.
         */
        std::function<double(double)> getScaleFunction(){return this->scaleFunction;}
        /**
         * @brief Set the scale function used in the propagation model.
         * @param scaleFunction The scale function to be set.
         * @details This function is used to set the scale function used in the propagation model.
         */
        void setScaleFunction(std::function<double(double)> scaleFunction){this->scaleFunction = scaleFunction;}
};
