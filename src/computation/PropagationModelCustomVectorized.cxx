#include "computation/PropagationModelCustomVectorized.hxx"
#include <armadillo>
#include <iostream>

PropagationModelCustomVectorized::PropagationModelCustomVectorized(const WeightedEdgeGraph* graph){
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

PropagationModelCustomVectorized::~PropagationModelCustomVectorized(){
}

PropagationModelCustomVectorized::PropagationModelCustomVectorized(const WeightedEdgeGraph* graph, std::function<double(double)> scaleFun):scaleFunction(scaleFun){
    //getting normalization values for the adjacency matrix
    std::vector<double> normalizationFactors(graph->getNumNodes(),0);
    for (int i = 0; i < graph->getNumNodes(); i++) {
        for(int j = 0; j < graph->getNumNodes();j++){
            normalizationFactors[i] += std::abs(graph->getEdgeWeight(i,j)); 
        }
    }

    this->Wmat = graph->adjMatrix.transpose().normalizeByVectorColumn(normalizationFactors).asArmadilloMatrix();
}


arma::Mat<double> PropagationModelCustomVectorized::propagate(arma::Mat<double> input, double time){
    return input + (Wmat * input * this->scaleFunction(time));
}

arma::Mat<double> PropagationModelCustomVectorized::propagationTerm(arma::Mat<double> input, double time){
    return (Wmat * input * this->scaleFunction(time));
}
