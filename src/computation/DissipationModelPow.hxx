/**
 * @file DissipationModelPow.hxx
 * @ingroup Core
 * @brief Defines the DissipationModelPow class used for managing power dissipation dynamics for the computation of the perturbation in MASFENON.
 * @details The DissipationModelPow class inherits from the DissipationModel class and implements power dissipation logic.
 * @details The power dissipation model uses a power factor to compute the dissipation term.
 */
#pragma once
#include <armadillo>
#include "computation/DissipationModel.hxx"

/**
 * @class DissipationModelPow
 * @brief Class for managing power dissipation dynamics in MASFENON.
 * @details This class inherits from the DissipationModel class and implements power dissipation logic.
 * @details The power dissipation model uses a power factor to compute the dissipation term.
 * @implements DissipationModel
 */
class DissipationModelPow : public DissipationModel
{
    private:
        double power; ///< The power factor used in the power dissipation model.
    public:
        /**
         * @brief Default constructor for the DissipationModelPow class.
         * @details Initializes the power dissipation model with default values(zeros for everything)
         */
        DissipationModelPow();
        /**
         * @brief Constructor for the DissipationModelPow class.
         * @param power The power factor used in the power dissipation model.
         * @details Initializes the power dissipation model with the given value.
         */
        DissipationModelPow(double power);
        /**
         * @brief Destructor for the DissipationModelPow class.
         * @details Cleans up any resources used by the class.
         */
        ~DissipationModelPow();
        /**
         * @brief Applies the power dissipation model to the input vector.
         * @param input The input vector to be processed.
         * @param time The current time.
         * @return The output vector after applying the power dissipation model.
         * @details This function computes the output vector by applying the power dissipation model to the input vector.
         */
        arma::Col<double> dissipate(arma::Col<double> input, double time)override;
        /**
         * @brief Computes the dissipation term of the input vector.
         * @param input The input vector to be processed.
         * @param time The current time.
         * @return The dissipation term vector.
         * @details This function computes the dissipation term of the input vector by applying the power dissipation model.
         */
        arma::Col<double> dissipationTerm(arma::Col<double> input, double time)override;
        /**
         * @brief Gets the power factor used in the power dissipation model.
         * @return The power factor.
         * @details This function returns the power factor used in the power dissipation model.
         */
        double getPower(){return this->power;}
};