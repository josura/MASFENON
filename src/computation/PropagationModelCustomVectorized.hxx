#pragma once
#include <armadillo>
#include "computation/PropagationModelVectorized.hxx"

class PropagationModelCustomVectorized : public PropagationModelVectorized
{
    private:
        std::function<double(double)> scaleFunction;
        arma::dmat Wmat;
    public:
        PropagationModelCustomVectorized(const WeightedEdgeGraph* graph);
        PropagationModelCustomVectorized(const WeightedEdgeGraph* graph,std::function<double(double)> scaleFunc);
        ~PropagationModelCustomVectorized()override;
        arma::Mat<double> propagate(arma::Mat<double> input,double time)override; // add additional parameters, but remember to change the main accordingly
        arma::Mat<double> propagationTerm(arma::Mat<double> input, double time)override;
        double getScale(double time){return scaleFunction(time);}
};