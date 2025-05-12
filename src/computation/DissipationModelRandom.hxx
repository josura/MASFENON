/**
 * @file DissipationModelRandom.hxx
 * @ingroup Core
 * @brief Defines the DissipationModelRandom class used for managing random dissipation dynamics for the computation of the perturbation in MASFENON.
 * @details The DissipationModelRandom class inherits from the DissipationModel class and implements random dissipation logic.
 * @details The random dissipation model uses a range of values to compute the dissipation term.
 * @details The dissipation term is computed as a random value between the given range, for every single element.
 */
#pragma once
#include <armadillo>
#include "computation/DissipationModel.hxx"

/**
 * @class DissipationModelRandom
 * @brief Class for managing random dissipation dynamics in MASFENON.
 * @details This class inherits from the DissipationModel class and implements random dissipation logic.
 * @details The random dissipation model uses a range of values to compute the dissipation term.
 * @details The dissipation term is computed as a random value between the given range, for every single element.
 * @implements DissipationModel
 */
class DissipationModelRandom : public DissipationModel
{
    private:
        double rangeMin; ///< The minimum value of the range used in the random dissipation model.
        double rangeMax; ///< The maximum value of the range used in the random dissipation model.
    public:
        /**
         * @brief Default constructor for the DissipationModelRandom class.
         * @details Initializes the random dissipation model with default values( range will be [0,1])
         */
        DissipationModelRandom();
        /**
         * @brief Constructor for the DissipationModelRandom class.
         * @param rangeMin The minimum value of the range used in the random dissipation model.
         * @param rangeMax The maximum value of the range used in the random dissipation model.
         * @details Initializes the random dissipation model with the given values.
         */
        DissipationModelRandom(double rangeMin, double rangeMax);
        /**
         * @brief Destructor for the DissipationModelRandom class.
         * @details Cleans up any resources used by the class.
         */
        ~DissipationModelRandom();
        /**
         * @brief Applies the random dissipation model to the input vector.
         * @param input The input vector to be processed.
         * @param time The current time.
         * @return The output vector after applying the random dissipation model.
         * @details This function computes the output vector by applying the random dissipation model to the input vector.
         */
        arma::Col<double> dissipate(arma::Col<double> input, double time)override;
        /**
         * @brief Computes the dissipation term of the input vector.
         * @param input The input vector to be processed.
         * @param time The current time.
         * @return The dissipation term vector.
         * @details This function computes the dissipation term of the input vector by applying the random dissipation model.
         */
        arma::Col<double> dissipationTerm(arma::Col<double> input, double time)override;
};