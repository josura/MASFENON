/**
 * @file DissipationModelScaledVectorized.hxx
 * @ingroup Experimental
 * @brief Defines the DissipationModelScaledVectorized class used for managing vectorized scaled dissipation dynamics for the computation of the perturbation in MASFENON. This class is an abstract class.
 * @warning This class is not yet implemented.
 * @details The scaled dissipation class uses a scale function to determine the scaling of the dissipation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 * @todo Make it stateful. Related to issue #28
 */
#pragma once
#include <armadillo>
#include "computation/DissipationModelVectorized.hxx"

/**
 * @class DissipationModelScaledVectorized
 * @brief Dissipation model implementation for scaled vectorized dissipation dynamics.
 * @details This class provides methods for applying scaled vectorized dissipation logic to the perturbation computation.
 * @warning This class is not yet implemented.
 * @details The scaled dissipation class uses a scale function to determine the scaling of the dissipation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 * @todo Make it stateful. Related to issue #28
 * @todo Implement the dissipation model logic and vectorization. Related to issue #5
 */
class DissipationModelScaledVectorized : public DissipationModelVectorized
{
    private:
        std::function<double(double)> scaleFunction; ///< The scale function used to determine the scaling of the dissipation term. This function can be set and modified as needed.
    public:
        // default constructor uses scaleFunction = 0.5
        /**
         * @brief Default constructor for the DissipationModelScaledVectorized class.
         * @details This constructor initializes the scale function to a default value of 0.5.
         */
        DissipationModelScaledVectorized();
        /**
         * @brief Constructor for the DissipationModelScaledVectorized class with a custom scale function.
         * @param scaleFunc The custom scale function to be used in the dissipation model.
         * @details This constructor initializes the scale function to the provided custom value.
         */
        DissipationModelScaledVectorized(std::function<double(double)> scaleFunc);
        /**
         * @brief Default destructor for the DissipationModelScaledVectorized class.
         * @details Cleans up the resources used by the DissipationModelScaledVectorized class.
         */
        ~DissipationModelScaledVectorized()override;
        /**
         * @brief Dissipate the input vector.
         * @param input The input vector to be processed.
         * @param time The current time.
         * @return The output vector after applying the dissipation model.
         * @details This function is used to compute the final output of the dissipation model.
         */
        arma::Mat<double> dissipate(arma::Mat<double> input, double time)override;
        /**
         * @brief Dissipation term of the input vector.
         * @param input The input vector to be processed.
         * @param time The current time.
         * @return The dissipation term vector.
         * @details This function is used to compute the dissipation term of the input vector.
         */
        arma::Mat<double> dissipationTerm(arma::Mat<double> input, double time)override;
        /**
         * @brief Get the scale value at a given time.
         * @param time The current time.
         * @return The scale value at the given time.
         * @details This function is used to get the scale value at a given time.
         */
        double getScale(double time){return scaleFunction(time);}
};