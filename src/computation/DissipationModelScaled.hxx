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

class DissipationModelScaled : public DissipationModel
{
    private:
        std::function<double(double)> scaleFunction;
    public:
        // default constructor uses scaleFunction = 0.5
        DissipationModelScaled();
        DissipationModelScaled(std::function<double(double)> scaleFunc);
        ~DissipationModelScaled()override;
        arma::Col<double> dissipate(arma::Col<double> input, double time)override;
        arma::Col<double> dissipationTerm(arma::Col<double> input, double time)override;
        double getScale(double time){return scaleFunction(time);}
};