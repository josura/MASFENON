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
        arma::Mat<double> propagate(arma::Mat<double> input,double time)override;
        arma::Mat<double> propagationTerm(arma::Mat<double> input, double time)override;
        double getScale(double time){return scaleFunction(time);}
};