/**
 * @file ConservationModel.hxx
 * @ingroup Core
 * @brief Defines the ConservationModel class used for managing conservation dynamics for the computation of the perturbation in MASFENON.
 * @details The ConservationModel class provides methods for applying conservation logic to the perturbation computation. The class is not designed as an abstract class, but rather as a base class for specific conservation models.
 * @details The conservation class uses a scale function to determine the scaling of the conservation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 */
#pragma once
#include <armadillo>
#include <functional>

/**
 * @class ConservationModel
 * @brief Class for managing conservation dynamics in MASFENON.
 * @details This class provides methods for applying conservation logic to the perturbation computation.
 * @details The conservation class uses a scale function to determine the scaling of the conservation term. The scale function can be set and modified as needed.* 
 * @details To set the scale function, @see CustomFunctions.hxx
 * @todo Make it stateful. Related to issue #28 @link https://github.com/josura/MASFENON/issues/28 @endlink
 */
class ConservationModel{
    protected:
        std::function<double(double)> scaleFunction; ///< The function to scale the conservation term. It takes a double value (time) and returns a double value.>
        std::function<arma::Col<double>(double)> scaleFunctionVectorized; ///< The function to scale the conservation term for vectorized operations. It takes a double value (time) and returns a vector of double values (scaling values).
    public:
        /**
         * @brief Default constructor for the ConservationModel class.
         * @details Initializes the scale function to a default constant function that returns the value of 0.5.
         */
        ConservationModel();
        /**
         * @brief Constructor for the ConservationModel class.
         * @param scaleFunction The function to scale the conservation term. It takes a double value (time) and returns a double value.
         * @details Initializes the scale function to the provided value.
         */
        ConservationModel(std::function<double(double)> scaleFunction);
        /**
         * @brief Constructor for the Conservation Model class, passing a scaling function that uses vectorized operations.
         * @param scaleFunction The function to scale the conservation term. It takes a double value (time) and returns a Diagonal matrix of double values (scaling values for each node, outgoing edges are scaled with this).
         * @details Initializes the scale function to the provided value, allowing for vectorized operations on the input vector.
         */
        ConservationModel(std::function<arma::Col<double>(double)> scaleFunction);
        /**
         * @brief Destructor for the ConservationModel class.
         * @details Cleans up any resources used by the class.
         */
        virtual ~ConservationModel();
        //using the scale function as a parameter itself, dependency injection
        /**
         * @brief Applies the conservation model to the input vector.
         * @param input The input vector to be processed.
         * @param inputDissipated The dissipated input vector.
         * @param Wstar The matrix representing the conservation model.
         * @param time The current time.
         * @param q A vector of weights for the edges (default is an empty vector).
         * @return The output vector after applying the conservation model.
         * @details This function is used to compute the final output of the conservation model.
         * @details The output is computed as the product of the scale function, the matrix Wstar, and the input vector, taking into account the dissipated input.
         */
        virtual arma::Col<double> conservate(arma::Col<double> input, arma::Col<double> inputDissipated,arma::Mat<double> Wstar, double time, std::vector<double> q = std::vector<double>());
        /**
         * @brief Applies the conservation to the input vector and returns the conservation term.
         * @param input The input vector to be processed.
         * @param Wstar The matrix representing the conservation model.
         * @param time The current time.
         * @param q A vector of weights for the edges (default is an empty vector).
         * @return The conservation term vector.
         * @details This function is used to compute the conservation term for the input vector.
         * @details The conservation term is computed as the product of the scale function, the matrix Wstar, and the input vector.
         */
        virtual arma::Col<double> conservationTerm(arma::Col<double> input,arma::Mat<double> Wstar, double time, std::vector<double> q = std::vector<double>());

        //getters and setters
        /**
         * @brief Gets the scale function.
         * @return The scale function.
         * @details This function returns the scale function used in the conservation model.
         */
        std::function<double(double)> getScaleFunction(){return this->scaleFunction;}
        /**
         * @brief Sets the scale function.
         * @param scaleFunction The scale function to be set.
         * @details This function sets the scale function used in the conservation model.
         */
        void setScaleFunction(std::function<double(double)> scaleFunction){this->scaleFunction = scaleFunction;}
};