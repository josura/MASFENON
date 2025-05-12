/**
 * @file DissipationModelScaled.hxx
 * @ingroup Core
 * @brief Defines the DissipationModelScaled class used for managing scaled dissipation dynamics for the computation of the perturbation in MASFENON.
 * @details The DissipationModelScaled class inherits from the DissipationModel class and implements scaled dissipation logic.
 * @details The scaled dissipation model uses a scale function to compute the dissipation term.
 * @details The dissipation term is computed as the product of the input vector and the scale function.
 * @see DissipationModel.hxx
 * @see CustomFunctions.hxx
 */
#pragma once
#include <armadillo>
#include "computation/DissipationModel.hxx"

/**
 * @class DissipationModelScaled
 * @brief Class for managing scaled dissipation dynamics in MASFENON.
 * @details This class inherits from the DissipationModel class and implements scaled dissipation logic.
 * @details The scaled dissipation model uses a scale function to compute the dissipation term.
 * @details The dissipation term is computed as the product of the input vector and the scale function.
 * @details The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 * @implements DissipationModel
 */
class DissipationModelScaled : public DissipationModel
{
    private:
        std::function<double(double)> scaleFunction; ///< The function to scale the dissipation term. It takes a double value (time) and returns a double value.
    public:
        // default constructor uses scaleFunction = 0.5
        /**
         * @brief Default constructor for the DissipationModelScaled class.
         * @details Initializes the scale function to a default constant function that returns the value of 0.5.
         */
        DissipationModelScaled();
        /**
         * @brief Constructor for the DissipationModelScaled class.
         * @param scaleFunction The function to scale the dissipation term. It takes a double value (time) and returns a double value.
         * @details Initializes the scale function to the provided value.
         */
        DissipationModelScaled(std::function<double(double)> scaleFunc);
        /**
         * @brief Destructor for the DissipationModelScaled class.
         * @details Cleans up any resources used by the class.
         */
        ~DissipationModelScaled()override;
        /**
         * @brief Applies the scaled dissipation model to the input vector.
         * @param input The input vector to be processed.
         * @param time The current time.
         * @return The output vector after applying the scaled dissipation model.
         * @details This function computes the output vector by applying the scaled dissipation model to the input vector.
         */
        arma::Col<double> dissipate(arma::Col<double> input, double time)override;
        /**
         * @brief Computes the dissipation term of the input vector.
         * @param input The input vector to be processed.
         * @param time The current time.
         * @return The dissipation term vector.
         * @details This function computes the dissipation term of the input vector by applying the scaled dissipation model.
         */
        arma::Col<double> dissipationTerm(arma::Col<double> input, double time)override;
        /**
         * @brief Gets the scale function used in the scaled dissipation model.
         * @return The scale function.
         * @details This function returns the scale function used in the scaled dissipation model.
         */
        double getScale(double time){return scaleFunction(time);}
};