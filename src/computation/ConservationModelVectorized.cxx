#include "computation/ConservationModelVectorized.hxx"
#include "utils/armaUtilities.hxx"
#include "utils/mathUtilities.hxx"


ConservationModelVectorized::ConservationModelVectorized(){
    this->scaleFunction = [](double time)-> double{return 0.5;};
}

ConservationModelVectorized::ConservationModelVectorized(std::function<double(double)> scaleFunction){
    this->scaleFunction = scaleFunction;
}

ConservationModelVectorized::~ConservationModelVectorized(){}

arma::Mat<double> ConservationModelVectorized::conservate(arma::Mat<double> input, arma::Mat<double> inputDissipated, arma::Mat<double> Wstar,double time, std::vector<double> q){
    //if q is empty, then we assume that all the values in q are 1, that means all the weights of the edges are considered of the same importance and 
    // and the perturbation is completely passed down the network 
    if (q.size()) {
        if (q.size() == input.n_rows) {
            //convert q vector to arma vector
            arma::Mat<double> qArmaMatrix = vectorToArmaMatrixRepeatColumns(q, Wstar.n_cols);
            // TODO repeat qarma column of the input matrix
            arma::Mat<double> outputArma = inputDissipated -  (scaleFunction(time) * Wstar * qArmaMatrix) % input;
            return outputArma;
        } else{
            throw std::invalid_argument("q vector is not of the same size as input vector. abort");
        }
    }
    else {
        //since in the case of vector values with all q values equal to 1
        arma::Mat<double> qOnesMatrix = arma::ones<arma::Mat<double>>(input.n_rows, input.n_cols);
        arma::Mat<double> outputArma =  inputDissipated -  (scaleFunction(time)* Wstar * qOnesMatrix) % input;
        return outputArma;
    }
}

arma::Mat<double> ConservationModelVectorized::conservationTerm(arma::Mat<double> input, arma::Mat<double> Wstar, double time, std::vector<double> q){
    if (q.size()) {
        if (q.size() == input.n_rows) {
            //convert q vector to arma vector
            arma::Mat<double> qArmaMatrix = vectorToArmaMatrixRepeatColumns(q, Wstar.n_cols);
            arma::Mat<double> outputArma =  (scaleFunction(time) * Wstar * qArmaMatrix) % input;
            return outputArma;
        } else{
            throw std::invalid_argument("q is not of the same size as input vector. abort");
        }
    } else{
        arma::Mat<double> qOnesMatrix = arma::ones<arma::Mat<double>>(input.n_rows, input.n_cols);
        arma::Mat<double> outputArma =  scaleFunction(time) *(Wstar * qOnesMatrix) % input;
        return outputArma;
    }
}


