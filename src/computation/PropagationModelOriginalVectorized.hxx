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
        arma::Col<double> propagate(arma::Col<double> input,double time)override;
        arma::Col<double> propagationTerm(arma::Col<double> input, double time)override;
        double getScale(double time){return scaleFunction(time);}
};