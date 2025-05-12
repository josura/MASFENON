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

/** */
class DissipationModelPeriodic : public DissipationModel
{
    private:
        arma::Col<double> phases;
        arma::Col<double> periods;
        arma::Col<double> amplitudes;
    public:
        DissipationModelPeriodic();
        DissipationModelPeriodic(int numEl,double phase, double period, double amplitude);
        DissipationModelPeriodic(arma::Col<double> phases, arma::Col<double> periods, arma::Col<double> amplitudes);
        ~DissipationModelPeriodic();
        arma::Col<double> dissipate(arma::Col<double> input, double time)override;
        arma::Col<double> dissipationTerm(arma::Col<double> input, double time)override;
        arma::Col<double> getPhases(){return this->phases;}
        arma::Col<double> getPeriods(){return this->periods;}
        arma::Col<double> getAmplitudes(){return this->amplitudes;}
};