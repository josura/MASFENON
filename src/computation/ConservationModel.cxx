/**
 * @file ConservationModel.cxx
 * @ingroup Core
 * @brief Implements the ConservationModel class used for managing conservation dynamics for the computation of the perturbation in MASFENON.
 */
#include "computation/ConservationModel.hxx"
#include "utils/armaUtilities.hxx"
#include "utils/mathUtilities.hxx"


ConservationModel::ConservationModel(){
    this->scaleFunction = [](double time)-> double{return 0.5;};
    // initialize the vectorized scaled function to be used later when the number of elements is known (to be able to meet a condition when being initialized, like returning a size 0 Matrix)
    this->scaleFunctionVectorized = [](double time)-> arma::Mat<double>{return arma::Mat<double>(0,0);};

}

ConservationModel::ConservationModel(std::function<double(double)> scaleFunction){
    this->scaleFunction = scaleFunction;
    // initialize the vectorized scaled function to be used later when the number of elements is known (to be able to meet a condition when being initialized, like returning a size 0 Matrix)
    this->scaleFunctionVectorized = [](double time)-> arma::Mat<double>{return arma::Mat<double>(0,0);};
}

ConservationModel::ConservationModel(std::function<arma::Mat<double>(double)> scaleFunction){
    this->scaleFunctionVectorized = scaleFunction;
}

ConservationModel::~ConservationModel(){}

arma::Col<double> ConservationModel::conservate(arma::Col<double> input, arma::Col<double> inputDissipated, arma::Mat<double> Wstar,double time, std::vector<double> q){
    // initializing the vectorized scale function if it was not initialized before (we have the number of elements now in the input)
    if (this->scaleFunctionVectorized(0).n_cols == 0) {
        // make a vectorized function that returns a diagonal matrix with the scale function values on the diagonal
        auto scaleFunction = this->scaleFunction; // capture the scale function
        this->scaleFunctionVectorized = [scaleFunction,input](double time)-> arma::Mat<double>{
            arma::Mat<double> scaleMatrix = arma::diagmat(arma::ones<arma::Col<double>>(input.n_elem) * scaleFunction(time));
            return scaleMatrix;
        };  
    }

    //if q is empty, then we assume that all the values in q are 1, that means all the weights of the edges are considered of the same importance and 
    // and the perturbation is completely passed down the network 
    if (q.size()) {
        if (q.size() == input.n_elem) {
            //convert q vector to arma vector
            arma::Col<double> qArma = vectorToArmaColumn(q);
            arma::Col<double> outputArma = inputDissipated -  (scaleFunctionVectorized(time) * Wstar * qArma) % input;
            return outputArma;
        } else{
            throw std::invalid_argument("q vector is not of the same size as input vector. abort");
        }
    }
    else {
        //since in the case of vector values with all q values equal to 1
        arma::Col<double> qOnes = arma::ones<arma::Col<double>>(input.n_elem);
        arma::Col<double> outputArma =  inputDissipated -  (scaleFunctionVectorized(time)* Wstar * qOnes) % input;
        return outputArma;
    }
}

arma::Col<double> ConservationModel::conservationTerm(arma::Col<double> input, arma::Mat<double> Wstar, double time, std::vector<double> q){
    if (q.size()) {
        if (q.size() == input.n_elem) {
            //convert q vector to arma vector
            arma::Col<double> qArma = vectorToArmaColumn(q);
            arma::Col<double> outputArma =  (scaleFunctionVectorized(time) * Wstar * qArma) % input;
            return outputArma;
        } else{
            throw std::invalid_argument("q is not of the same size as input vector. abort");
        }
    } else{
        arma::Col<double> qOnes = arma::ones<arma::Col<double>>(input.n_elem);
        arma::Col<double> outputArma =  scaleFunctionVectorized(time) *(Wstar * qOnes) % input;
        return outputArma;
    }
}


