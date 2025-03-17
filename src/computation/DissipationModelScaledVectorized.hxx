#pragma once
#include <armadillo>
#include "computation/DissipationModelVectorized.hxx"

class DissipationModelScaledVectorized : public DissipationModelVectorized
{
    private:
        std::function<double(double)> scaleFunction;
    public:
        // default constructor uses scaleFunction = 0.5
        DissipationModelScaledVectorized();
        DissipationModelScaledVectorized(std::function<double(double)> scaleFunc);
        ~DissipationModelScaledVectorized()override;
        arma::Mat<double> dissipate(arma::Mat<double> input, double time)override;
        arma::Mat<double> dissipationTerm(arma::Mat<double> input, double time)override;
        double getScale(double time){return scaleFunction(time);}
};