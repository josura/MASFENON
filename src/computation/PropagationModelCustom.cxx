/**
 * @file PropagationModelCustom.cxx
 * @ingroup Core
 * @brief Implements the PropagationModelCustom class used for managing custom propagation dynamics for the computation of the perturbation in MASFENON.
 * @details The PropagationModelCustom class provides methods for applying custom propagation logic to the perturbation computation.
 * @details The propagation class uses a scale function to determine the scaling of the propagation term. The scale function can be set and modified as needed.
 */
#include "computation/PropagationModelCustom.hxx"
#include <armadillo>
#include <iostream>

PropagationModelCustom::PropagationModelCustom(const WeightedEdgeGraph* graph){
    this->scaleFunction = [](double time)-> double{return 0.5;};

    //getting normalization values for the adjacency matrix
    std::vector<double> normalizationFactors(graph->getNumNodes(),0);
    for (int i = 0; i < graph->getNumNodes(); i++) {
        for(int j = 0; j < graph->getNumNodes();j++){
            normalizationFactors[i] += std::abs(graph->getEdgeWeight(i,j)); 
        }
    }

    this->Wmat = graph->adjMatrix.transpose().normalizeByVectorColumn(normalizationFactors).asArmadilloMatrix();
}

PropagationModelCustom::~PropagationModelCustom(){
}

PropagationModelCustom::PropagationModelCustom(const WeightedEdgeGraph* graph, std::function<double(double)> scaleFun):scaleFunction(scaleFun){
    //getting normalization values for the adjacency matrix
    std::vector<double> normalizationFactors(graph->getNumNodes(),0);
    for (int i = 0; i < graph->getNumNodes(); i++) {
        for(int j = 0; j < graph->getNumNodes();j++){
            normalizationFactors[i] += std::abs(graph->getEdgeWeight(i,j)); 
        }
    }

    this->Wmat = graph->adjMatrix.transpose().normalizeByVectorColumn(normalizationFactors).asArmadilloMatrix();
}


arma::Col<double> PropagationModelCustom::propagate(arma::Col<double> input, double time){
    return input + (Wmat * input * this->scaleFunction(time));
}

arma::Col<double> PropagationModelCustom::propagationTerm(arma::Col<double> input, double time){
    return (Wmat * input * this->scaleFunction(time));
}
