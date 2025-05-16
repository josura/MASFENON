/**
 * @file PropagationModelOriginalVectorized.hxx
 * @ingroup Experimental
 * @brief Defines the PropagationModelOriginalVectorized class used for managing vectorized propagation dynamics for the computation of the perturbation in MASFENON. This class is an implemntation of the PropagationModelVectorized class.
 */
#pragma once
#include <armadillo>
#include "computation/PropagationModelVectorized.hxx"

/**
 * @class PropagationModelOriginalVectorized
 * @brief Propagation model implementation for original vectorized propagation dynamics.
 * @details This class provides methods for applying original vectorized propagation logic to the perturbation computation.
 * @warning This class is not yet implemented.
 * @details The original propagation class uses a scale function to determine the scaling of the propagation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 */
class PropagationModelOriginalVectorized : public PropagationModelVectorized
{
    private:
        std::function<double(double)> scaleFunction; ///< The scale function used to determine the scaling of the propagation term. This function can be set and modified as needed.
        arma::dmat pseudoinverse; ///< The pseudoinverse of the adjacency matrix. This is used to compute the propagation term.
    public:
        /**
         * @brief Default constructor for the PropagationModelOriginalVectorized class.
         * @details This constructor initializes the scale function to a default value of 0.5.
         * @details This function computes the pseudoinverse of the adjacency matrix.
         */
        PropagationModelOriginalVectorized(const WeightedEdgeGraph* graph);
        /**
         * @brief Constructor for the PropagationModelOriginalVectorized class with a custom scale function.
         * @param graph The graph to be used in the propagation model.
         * @param scaleFunc The custom scale function to be used in the propagation model.
         * @details This constructor initializes the scale function to the provided custom value.
         * @details This function computes the pseudoinverse of the adjacency matrix.
         */
        PropagationModelOriginalVectorized(const WeightedEdgeGraph* graph,std::function<double(double)> scaleFunc);
        /**
         * @brief Default destructor for the PropagationModelOriginalVectorized class.
         * @details Cleans up the resources used by the PropagationModelOriginalVectorized class.
         */
        ~PropagationModelOriginalVectorized()override;
        /**
         * @brief Propagate the input vector.
         * @param input The input vector to be processed.
         * @param time The current time.
         * @return The output vector after applying the propagation model.
         * @details This function is used to compute the final output of the propagation model.
         */
        arma::Mat<double> propagate(arma::Mat<double> input,double time)override;
        /**
         * @brief Propagation term of the input vector.
         * @param input The input vector to be processed.
         * @param time The current time.
         * @return The propagation term vector.
         * @details This function is used to compute the propagation term of the input vector.
         */
        arma::Mat<double> propagationTerm(arma::Mat<double> input, double time)override;
        /**
         * @brief Get the scale function used in the propagation model.
         * @return The scale function used in the propagation model.
         * @details This function is used to get the scale function used in the propagation model.
         */
        double getScale(double time){return scaleFunction(time);}
};