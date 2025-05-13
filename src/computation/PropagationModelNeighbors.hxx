/**
 * @file PropagationModelNeighbors.hxx
 * @ingroup Core
 * @brief Defines the PropagationModelNeighbors class used for managing propagation dynamics for the computation of the perturbation in MASFENON.
 * @details The PropagationModelNeighbors class provides methods for applying propagation logic to the perturbation computation.
 * @details The propagation class uses a scale function to determine the scaling of the propagation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 * @details The propagation function propagates the values in each network, considering each node's neighbors and the weights of the edges connecting them.
 */
#pragma once
#include <armadillo>
#include "computation/PropagationModel.hxx"

class PropagationModelNeighbors : public PropagationModel
{
    private:
        std::function<double(double)> scaleFunction;
        arma::dmat Wmat;
    public:
        PropagationModelNeighbors(const WeightedEdgeGraph* graph);
        PropagationModelNeighbors(const WeightedEdgeGraph* graph,std::function<double(double)> scaleFunc);
        ~PropagationModelNeighbors()override;
        arma::Col<double> propagate(arma::Col<double> input,double time)override;
        arma::Col<double> propagationTerm(arma::Col<double> input, double time)override;
        double getScale(double time){return scaleFunction(time);}
};