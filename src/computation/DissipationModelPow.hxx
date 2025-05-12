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

class DissipationModelPow : public DissipationModel
{
    private:
        double power;
    public:
        DissipationModelPow();
        DissipationModelPow(double power);
        ~DissipationModelPow();
        arma::Col<double> dissipate(arma::Col<double> input, double time)override;
        arma::Col<double> dissipationTerm(arma::Col<double> input, double time)override;
        double getPower(){return this->power;}
};