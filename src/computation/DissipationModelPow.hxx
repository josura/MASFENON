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