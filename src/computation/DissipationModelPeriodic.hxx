/**
 * @file DissipationModelPeriodic.hxx
 * @ingroup Core
 * @brief Defines the DissipationModelPeriodic class used for managing periodic dissipation dynamics for the computation of the perturbation in MASFENON.
 * @details The DissipationModelPeriodic class inherits from the DissipationModel class and implements periodic dissipation logic.
 * @details The periodic dissipation model uses a set of phases, periods, and amplitudes to compute the dissipation term.
 * @details The dissipation term is computed as a sum of sine functions with the given phases, periods, and amplitudes.
 * @see DissipationModel.hxx
 */
#pragma once
#include <armadillo>
#include "computation/DissipationModel.hxx"

/**
 * @class DissipationModelPeriodic
 * @brief Class for managing periodic dissipation dynamics in MASFENON.
 * @details This class inherits from the DissipationModel class and implements periodic dissipation logic.
 * @details The periodic dissipation model uses a set of phases, periods, and amplitudes to compute the dissipation term.
 * @details The dissipation term is computed as a sum of sine functions with the given phases, periods, and amplitudes.
 * @implements DissipationModel
 */
class DissipationModelPeriodic : public DissipationModel
{
    private:
        arma::Col<double> phases; ///< The phases of the sine functions used in the periodic dissipation model.
        arma::Col<double> periods; ///< The periods of the sine functions used in the periodic dissipation model.
        arma::Col<double> amplitudes; ///< The amplitudes of the sine functions used in the periodic dissipation model.
    public:
        /**
         * @brief Default constructor for the DissipationModelPeriodic class.
         * @details Initializes the periodic dissipation model with default values(zeros for everything)
         */
        DissipationModelPeriodic();
        /**
         * @brief Constructor for the DissipationModelPeriodic class.
         * @param numEl The number of elements in the dissipation model.
         * @param phase The phase of the sine function used in the periodic dissipation model.
         * @param period The period of the sine function used in the periodic dissipation model.
         * @param amplitude The amplitude of the sine function used in the periodic dissipation model.
         * @details Initializes the periodic dissipation model with the given values.
         */
        DissipationModelPeriodic(int numEl,double phase, double period, double amplitude);
        /**
         * @brief Constructor for the DissipationModelPeriodic class.
         * @param phases The phases of the sine functions used in the periodic dissipation model. As an Armadillo column vector.
         * @param periods The periods of the sine functions used in the periodic dissipation model. As an Armadillo column vector.
         * @param amplitudes The amplitudes of the sine functions used in the periodic dissipation model. As an Armadillo column vector.
         * @details Initializes the periodic dissipation model with the given values.
         */
        DissipationModelPeriodic(arma::Col<double> phases, arma::Col<double> periods, arma::Col<double> amplitudes);
        /**
         * @brief Destructor for the DissipationModelPeriodic class.
         * @details Cleans up any resources used by the class.
         */
        ~DissipationModelPeriodic();
        arma::Col<double> dissipate(arma::Col<double> input, double time)override;
        arma::Col<double> dissipationTerm(arma::Col<double> input, double time)override;
        arma::Col<double> getPhases(){return this->phases;}
        arma::Col<double> getPeriods(){return this->periods;}
        arma::Col<double> getAmplitudes(){return this->amplitudes;}
};