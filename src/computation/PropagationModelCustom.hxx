/**
 * @file PropagationModelCustom.hxx
 * @ingroup Core
 * @brief Defines the PropagationModelCustom class used for managing custom propagation dynamics for the computation of the perturbation in MASFENON.
 * @details The PropagationModelCustom class provides methods for applying custom propagation logic to the perturbation computation.
 * @details The propagation class uses a scale function to determine the scaling of the propagation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 * @note This class is a concrete implementation of the PropagationModel class and should be used directly.
 */
#pragma once
#include <armadillo>
#include "computation/PropagationModel.hxx"

/**
 * @class PropagationModelCustom
 * @brief Class for managing custom propagation dynamics in MASFENON.
 * @details This class provides methods for applying custom propagation logic to the perturbation computation.
 * @details The propagation class uses a scale function to determine the scaling of the propagation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 * @note This class is a concrete implementation of the PropagationModel class and should be used directly.
 * @implements PropagationModel
 */
class PropagationModelCustom : public PropagationModel
{
    private:
        std::function<double(double)> scaleFunction; ///< The function to scale the propagation term. It takes a double value (time) and returns a double value.
        std::function<arma::Col<double>(double)> scaleFunctionVectorized; ///< The function to scale the propagation term for vectorized operations. It takes a double value (time) and returns a vector of double values (scaling values).
        arma::dmat Wmat; ///< The weighted adjacency matrix of the graph, transposed and normalized by column, as an Armadillo matrix.
    public:
        /**
         * @brief Constructor for the PropagationModelCustom class, passing a graph.
         * @param graph The graph to be used for the propagation model.
         * @details Initializes the propagation model with a default scale function (constant function always returning 0.5)and the weighted adjacency matrix of the graph.
         */
        PropagationModelCustom(const WeightedEdgeGraph* graph);
        /**
         * @brief Constructor for the PropagationModelCustom class, passing a graph and a scale function.
         * @param graph The graph to be used for the propagation model.
         * @param scaleFunc The scale function to be used in the propagation model.
         * @details Initializes the propagation model with the specified scale function and the weighted adjacency matrix of the graph.
         */
        PropagationModelCustom(const WeightedEdgeGraph* graph,std::function<double(double)> scaleFunc);
        /**
         * @brief Constructor for the PropagationModelCustom class, passing a graph and a vectorized scale function.
         * @param graph The graph to be used for the propagation model.
         * @param scaleFunc The vectorized scale function to be used in the propagation model.
         * @details Initializes the propagation model with the specified vectorized scale function and the weighted adjacency matrix of the graph.
         */
        PropagationModelCustom(const WeightedEdgeGraph* graph,std::function<arma::Col<double>(double)> scaleFunc);
        /**
         * @brief Destructor for the PropagationModelCustom class.
         * @details Cleans up the resources used by the propagation model.
         */
        ~PropagationModelCustom()override;
        /**
         * @brief Propagate the input vector.
         * @param input The input vector to be processed.
         * @param time The current time.
         * @return The output vector after applying the propagation model.
         * @details This function is used to compute the final output of the propagation model.
         */
        arma::Col<double> propagate(arma::Col<double> input,double time)override; // add additional parameters, but remember to change the main accordingly
        /**
         * @brief Propagation term of the input vector.
         * @param input The input vector to be processed.
         * @param time The current time.
         * @return The propagation term vector.
         * @details This function is used to compute the propagation term of the input vector.
         */
        arma::Col<double> propagationTerm(arma::Col<double> input, double time)override;
        /**
         * @brief Get the scale function value at a certain time.
         * @return The value of the scale function.
         * @details This function is used to get the scale function value at a certain time.
         */
        double getScale(double time){return scaleFunction(time);}
};