/**
 * @file DissipationModelScaledVectorized.hxx
 * @ingroup Experimental
 * @brief Defines the DissipationModelScaledVectorized class used for managing vectorized scaled dissipation dynamics for the computation of the perturbation in MASFENON. This class is an abstract class.
 * @warning This class is an abstract class and should not be used directly. It is intended to be inherited by other classes that implement specific scaled dissipation models.
 * @details The scaled dissipation class uses a scale function to determine the scaling of the dissipation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 * @todo Make it stateful. Related to issue #28 @link
 */
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