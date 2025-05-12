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

class DissipationModelRandom : public DissipationModel
{
    private:
        double rangeMin;
        double rangeMax;
    public:
        DissipationModelRandom();
        DissipationModelRandom(double rangeMin, double rangeMax);
        ~DissipationModelRandom();
        arma::Col<double> dissipate(arma::Col<double> input, double time)override;
        arma::Col<double> dissipationTerm(arma::Col<double> input, double time)override;
};