/**
 * @file PropagationModelVectorized.hxx
 * @ingroup Experimental
 * @brief Defines the PropagationModelVectorized class used for managing vectorized propagation dynamics for the computation of the perturbation in MASFENON. This class is an abstract class.
 * @warning This class is an abstract class and should not be used directly. It is intended to be inherited by other classes that implement specific propagation models.
 * @details The propagation class uses a scale function to determine the scaling of the propagation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 * @todo Make it stateful. Related to issue #28 @link
 */
#pragma once
#include <armadillo>
#include <functional>
#include <vector>
#include "data_structures/WeightedEdgeGraph.hxx"

/**
 * @class PropagationModelVectorized
 * @brief Abstract class for managing vectorized propagation dynamics in MASFENON.
 * @details This class provides methods for applying vectorized propagation logic to the perturbation computation.
 * @warning This class is an abstract class and should not be used directly. It is intended to be inherited by other classes that implement specific propagation models.
 * @details The propagation class uses a scale function to determine the scaling of the propagation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 * @todo Make it stateful. Related to issue #28 @link
 */
class PropagationModelVectorized{
    protected:
        std::function<double(double)> scaleFunction; ///< The scale function used to determine the scaling of the propagation term. This function can be set and modified as needed.
    public:
        /**
         * @brief Default destructor for the PropagationModelVectorized class. (needed even if the class is abstract)
         * @details Cleans up the resources used by the PropagationModelVectorized class.
         */
        virtual ~PropagationModelVectorized(){}
        //using the scale function as a parameter itself, dependency injection
        /**
         * @brief Propagate the input vector.
         * @param input The input vector to be processed.
         * @param time The current time.
         * @return The output vector after applying the propagation model.
         * @details This function is used to compute the final output of the propagation model.
         */
        virtual arma::Mat<double> propagate(arma::Mat<double> input, double time) = 0;
        /**
         * @brief Propagation term of the input vector.
         * @param input The input vector to be processed.
         * @param time The current time.
         * @return The propagation term vector.
         * @details This function is used to compute the propagation term of the input vector.
         */
        virtual arma::Mat<double> propagationTerm(arma::Mat<double> input, double time) = 0;

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