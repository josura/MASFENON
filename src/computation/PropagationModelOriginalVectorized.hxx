/**
 * @file PropagationModelOriginalVectorized.hxx
 * @ingroup Experimental
 * @brief Defines the PropagationModelOriginalVectorized class used for managing vectorized propagation dynamics for the computation of the perturbation in MASFENON. This class is an implemntation of the PropagationModelVectorized class.
 */
#pragma once
#include <armadillo>
#include "computation/PropagationModelVectorized.hxx"

class PropagationModelOriginalVectorized : public PropagationModelVectorized
{
    private:
        std::function<double(double)> scaleFunction;
        arma::dmat pseudoinverse;
    public:
        PropagationModelOriginalVectorized(const WeightedEdgeGraph* graph);
        PropagationModelOriginalVectorized(const WeightedEdgeGraph* graph,std::function<double(double)> scaleFunc);
        ~PropagationModelOriginalVectorized()override;
        arma::Mat<double> propagate(arma::Mat<double> input,double time)override;
        arma::Mat<double> propagationTerm(arma::Mat<double> input, double time)override;
        double getScale(double time){return scaleFunction(time);}
};