/**
 * @file PropagationModelCustom.hxx
 * @ingroup Core
 * @brief Defines the PropagationModelCustom class used for managing custom propagation dynamics for the computation of the perturbation in MASFENON.
 * @details The PropagationModelCustom class provides methods for applying custom propagation logic to the perturbation computation.
 * @details The propagation class uses a scale function to determine the scaling of the propagation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 * @note This class is a concrete implementation of the PropagationModel class and should be used directly.
 */
#pragma once
#include <armadillo>
#include "computation/PropagationModel.hxx"

class PropagationModelCustom : public PropagationModel
{
    private:
        std::function<double(double)> scaleFunction;
        arma::dmat Wmat;
    public:
        PropagationModelCustom(const WeightedEdgeGraph* graph);
        PropagationModelCustom(const WeightedEdgeGraph* graph,std::function<double(double)> scaleFunc);
        ~PropagationModelCustom()override;
        arma::Col<double> propagate(arma::Col<double> input,double time)override; // add additional parameters, but remember to change the main accordingly
        arma::Col<double> propagationTerm(arma::Col<double> input, double time)override;
        double getScale(double time){return scaleFunction(time);}
};