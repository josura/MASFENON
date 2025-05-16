/**
 * @file PropagationModelNeighborsVectorized.hxx
 * @ingroup Experimental
 * @brief Defines the PropagationModelNeighborsVectorized class used for managing vectorized propagation dynamics for the computation of the perturbation in MASFENON. This class is an implemntation of the PropagationModelVectorized class.
 * @warning This class is not yet implemented.
 * @details The neighbors propagation class uses a scale function to determine the scaling of the propagation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 */
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