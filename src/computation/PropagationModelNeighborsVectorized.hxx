#pragma once
#include <armadillo>
#include "computation/PropagationModelVectorized.hxx"

class PropagationModelNeighborsVectorized : public PropagationModelVectorized
{
    private:
        std::function<double(double)> scaleFunction;
        arma::dmat Wmat;
    public:
        PropagationModelNeighborsVectorized(const WeightedEdgeGraph* graph);
        PropagationModelNeighborsVectorized(const WeightedEdgeGraph* graph,std::function<double(double)> scaleFunc);
        ~PropagationModelNeighborsVectorized()override;
        arma::Col<double> propagate(arma::Col<double> input,double time)override;
        arma::Col<double> propagationTerm(arma::Col<double> input, double time)override;
        double getScale(double time){return scaleFunction(time);}
};