/**
 * @file ConservationModelVectorized.hxx
 * @ingroup Experimental
 * @brief Defines the ConservationModelVectorized class for vectorized conservation computations.
 * @details The ConservationModelVectorized class provides methods for applying conservation logic to the perturbation computation.
 * @details The conservation class uses a scale function to determine the scaling of the conservation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 */
#pragma once
#include <armadillo>
#include <functional>

/**
 * @class ConservationModelVectorized
 * @brief Class for managing vectorized conservation dynamics in MASFENON, working with vectors of values for the single nodes.
 * @details This class provides methods for applying vectorized conservation logic to the perturbation computation.
 * @details The conservation class uses a scale function to determine the scaling of the conservation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 * @details The conservation model is based on the vectorized approach, allowing for efficient computations with large datasets.
 */
class ConservationModelVectorized{
    protected:
        std::function<double(double)> scaleFunction; ///< The function to scale the conservation term. It takes a double value (time) and returns a double value.
    public:
        /**
         * @brief Default constructor for the ConservationModelVectorized class.
         * @details Initializes the scale function to a default constant function that returns the value of 0.5.
         */
        ConservationModelVectorized();
        /**
         * @brief Constructor for the ConservationModelVectorized class.
         * @param scaleFunction The function to scale the conservation term. It takes a double value (time) and returns a double value.
         * @details Initializes the scale function to the provided value.
         */
        ConservationModelVectorized(std::function<double(double)> scaleFunction);
        /**
         * @brief Destructor for the ConservationModelVectorized class.
         * @details Cleans up any resources used by the class.
         */
        virtual ~ConservationModelVectorized();
        //using the scale function as a parameter itself, dependency injection
        /**
         * @brief Applies the conservation model to the input matrix. (representing a vector of values for the single nodes)
         * @param input The input matrix to be processed.
         * @param inputDissipated The dissipated input matrix.
         * @param Wstar The matrix representing the conservation model.
         * @param time The current time.
         * @param q A vector of weights for the edges (default is an empty vector).
         * @return The output matrix after applying the conservation model.
         * @details This function is used to compute the final output of the conservation model.
         * @details The output is computed as the product of the scale function, the matrix Wstar, and the input matrix, taking into account the dissipated input.
         */         
        virtual arma::Mat<double> conservate(arma::Mat<double> input, arma::Mat<double> inputDissipated,arma::Mat<double> Wstar, double time, std::vector<double> q = std::vector<double>());
        /**
         * @brief Applies the conservation to the input matrix and returns the conservation term.
         * @param input The input matrix to be processed.
         * @param Wstar The matrix representing the conservation model.
         * @param time The current time.
         * @param q A vector of weights for the edges (default is an empty vector).
         * @return The conservation term matrix.
         * @details This function is used to compute the conservation term for the input matrix.
         * @details The conservation term is computed as the product of the scale function, the matrix Wstar, and the input matrix.
         */
        virtual arma::Mat<double> conservationTerm(arma::Mat<double> input,arma::Mat<double> Wstar, double time, std::vector<double> q = std::vector<double>());

        //getters and setters
        /**
         * @brief Get the scale function used in the conservation model.
         * @return The scale function used in the conservation model.
         * @details This function is used to get the scale function used in the conservation model.
         */
        std::function<double(double)> getScaleFunction(){return this->scaleFunction;}
        /**
         * @brief Set the scale function used in the conservation model.
         * @param scaleFunction The scale function to be set.
         * @details This function is used to set the scale function used in the conservation model.
         */
        void setScaleFunction(std::function<double(double)> scaleFunction){this->scaleFunction = scaleFunction;}
};