/**
 * @file PropagationModelNeighborsVectorized.hxx
 * @ingroup Experimental
 * @brief Defines the PropagationModelNeighborsVectorized class used for managing vectorized propagation dynamics for the computation of the perturbation in MASFENON. This class is an implemntation of the PropagationModelVectorized class.
 */
#pragma once
#include <armadillo>
#include "computation/PropagationModelVectorized.hxx"

/**
 * @class PropagationModelNeighborsVectorized
 * @brief Propagation model implementation for neighbors vectorized propagation dynamics.
 * @details This class provides methods for applying neighbors vectorized propagation logic to the perturbation computation.
 * @warning This class is not yet implemented.
 * @details The neighbors propagation class uses a scale function to determine the scaling of the propagation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 * @todo Make it stateful. Related to issue #28
 */
class PropagationModelNeighborsVectorized : public PropagationModelVectorized
{
    private:
        std::function<double(double)> scaleFunction; ///< The scale function used to determine the scaling of the propagation term. This function can be set and modified as needed.
        arma::dmat Wmat; ///< The adjacency matrix of the graph. This is used to compute the propagation term.
    public:
        /**
         * @brief Default constructor for the PropagationModelNeighborsVectorized class.
         * @details This constructor initializes the scale function to a default value of 0.5.
         */
        PropagationModelNeighborsVectorized(const WeightedEdgeGraph* graph);
        /**
         * @brief Constructor for the PropagationModelNeighborsVectorized class with a custom scale function.
         * @param graph The graph to be used in the propagation model.
         * @param scaleFunc The custom scale function to be used in the propagation model.
         * @details This constructor initializes the scale function to the provided custom value.
         */
        PropagationModelNeighborsVectorized(const WeightedEdgeGraph* graph,std::function<double(double)> scaleFunc);
        /**
         * @brief Default destructor for the PropagationModelNeighborsVectorized class.
         * @details Cleans up the resources used by the PropagationModelNeighborsVectorized class.
         */
        ~PropagationModelNeighborsVectorized()override;
        /**
         * @brief Propagate the input vector.
         * @param input The input vector to be processed.
         * @param time The current time.
         * @return The output vector after applying the propagation model.
         * @details This function is used to compute the final output of the propagation model. In this case, the values are propagated to the neighbors of the nodes.
         */
        arma::Mat<double> propagate(arma::Mat<double> input,double time)override;
        /**
         * @brief Propagation term of the input vector.
         * @param input The input vector to be processed.
         * @param time The current time.
         * @return The propagation term vector.
         * @details This function is used to compute the propagation term of the input vector. In this case, the values are propagated to the neighbors of the nodes.
         */
        arma::Mat<double> propagationTerm(arma::Mat<double> input, double time)override;
        /**
         * @brief Get the scale function value at a given time.
         * @param time The time at which to evaluate the scale function.
         * @return The value of the scale function at the given time.
         * @details This function is used to get the value of the scale function at a given time.
         */
        double getScale(double time){return scaleFunction(time);}
};