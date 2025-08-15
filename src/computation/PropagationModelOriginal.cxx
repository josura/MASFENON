/**
 * @file PropagationModelOriginal.cxx
 * @ingroup Core
 * @brief Implements the methods of the PropagationModelOriginal class used for managing propagation dynamics for the computation of the perturbation in MASFENON.
 * @details The PropagationModelOriginal class provides methods for applying propagation logic to the perturbation computation.
 */
#include "computation/PropagationModelOriginal.hxx"
#include <armadillo>
#include <iostream>

PropagationModelOriginal::PropagationModelOriginal(const WeightedEdgeGraph* graph){
    this->scaleFunction = [](double time)-> double{return 0.5;};
    int numElements = graph->getNumNodes();
    this->scaleFunctionVectorized = [numElements](double time)-> arma::Col<double>{return arma::ones<arma::Col<double>>(numElements) * 0.5;};

    //pseudoinverse initialization
    std::vector<double> normalizationFactors(graph->getNumNodes(),0);
    for (int i = 0; i < graph->getNumNodes(); i++) {
        for(int j = 0; j < graph->getNumNodes();j++){
            normalizationFactors[i] += std::abs(graph->getEdgeWeight(i,j)); 
        }
    }
    arma::Mat<double> WtransArma = graph->adjMatrix.transpose().normalizeByVectorColumn(normalizationFactors).asArmadilloMatrix();
    
    arma::Mat<double> IdentityArma = arma::eye(graph->getNumNodes(),graph->getNumNodes());
    arma::Mat<double> temp = IdentityArma - WtransArma;
    
    //control determinant and invertibility, print warning if not invertible
    if(arma::det(temp) == 0){
        Logger::getInstance().printWarning(" PropagationModelOriginal::PropagationModelOriginal(const WeightedEdgeGraph* graph): The graph is not invertible, the pseudoinverse could lead to faulty results");
    }
    pseudoinverse = arma::pinv(temp);
}

PropagationModelOriginal::~PropagationModelOriginal(){
}

PropagationModelOriginal::PropagationModelOriginal(const WeightedEdgeGraph* graph,std::function<double(double)> scaleFunc):scaleFunction(scaleFunc){
    //using a vectorized scale function that returns the scale function value for all elements
    int numElements = graph->getNumNodes();
    this->scaleFunctionVectorized = [scaleFunc, numElements](double time)-> arma::Col<double>{
        return arma::ones<arma::Col<double>>(numElements) * scaleFunc(time);
    };
    //pseudoinverse initialization
    std::vector<double> normalizationFactors(graph->getNumNodes(),0);
    for (int i = 0; i < graph->getNumNodes(); i++) {
        for(int j = 0; j < graph->getNumNodes();j++){
            normalizationFactors[i] += std::abs(graph->getEdgeWeight(i,j)); 
        }
    }
    arma::Mat<double> WtransArma = graph->adjMatrix.transpose().normalizeByVectorColumn(normalizationFactors).asArmadilloMatrix();
    
    arma::Mat<double> IdentityArma = arma::eye(graph->getNumNodes(),graph->getNumNodes());
    
    pseudoinverse = arma::pinv(IdentityArma - WtransArma);
}

arma::Col<double> PropagationModelOriginal::propagate(arma::Col<double> input, double time){
    return ( pseudoinverse * input * this->scaleFunction(time));
}

arma::Col<double> PropagationModelOriginal::propagationTerm(arma::Col<double> input, double time){
    //a propagation term doesn't exist in this case since it is a resolution of the system of equations
    return pseudoinverse * input * this->scaleFunction(time);
}
