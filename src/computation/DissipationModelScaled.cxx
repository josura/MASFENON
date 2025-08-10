/**
 * @file DissipationModelScaled.cxx
 * @ingroup Core
 * @brief Implements the DissipationModelScaled class used for managing scaled dissipation dynamics for the computation of the perturbation in MASFENON.
 * @details The DissipationModelScaled class inherits from the DissipationModel class and implements scaled dissipation logic.
 * @details The scaled dissipation model uses a scale function to compute the dissipation term.
 */
#include "computation/DissipationModelScaled.hxx"
#include <iostream>

DissipationModelScaled::DissipationModelScaled(){
    this->scaleFunction = [](double time)-> double{return 0.5;};
    this->numEl = 0;
    this->scaleFunctionVectorized = [](double time)-> arma::Col<double>{return arma::Mat<double>(0,0);}; // using a column vector with 0 elements as a placeholder
}

DissipationModelScaled::~DissipationModelScaled(){
}

DissipationModelScaled::DissipationModelScaled(std::function<double(double)> scaleFun){
    this->scaleFunction = scaleFun;
    this->numEl = 0;
    // initialize the vectorized scaled function to be used later when the number of elements is known (to be able to meet a condition when being initialized, like returning a size 0 Matrix)
    this->scaleFunctionVectorized = [](double time)-> arma::Col<double>{return arma::Mat<double>(0,0);};
}

DissipationModelScaled::DissipationModelScaled(std::function<arma::Col<double>(double)> scaleFun){
    this->scaleFunctionVectorized = scaleFun;
    this->numEl = this->scaleFunctionVectorized(0).n_elem; // initialize the number of elements based on the scale function
}

arma::Col<double> DissipationModelScaled::dissipate(arma::Col<double> input, double time){
    uint numElem = input.n_elem;
    // initializing the vectorized scale function if it was not initialized before (we have the number of elements now in the input)
    //if (this->scaleFunctionVectorized(0).n_elem == 0) {
    if (this->scaleFunctionVectorized(0).n_elem != numElem) {
        //this->scaleFunctionVectorized = [scaleFunction = scaleFunction,input](double time)-> arma::Mat<double>{
            //arma::Mat<double> scaleMatrix = arma::diagmat(arma::ones<arma::Col<double>>(input.n_elem) * scaleFunction(time));
        this->scaleFunctionVectorized = [scaleFunction = scaleFunction, numElem = numElem](double time)-> arma::Col<double>{
            //arma::Mat<double> scaleMatrix = arma::diagmat(arma::ones<arma::Col<double>>(input.n_elem) * 1.0); // using 1.0 as a placeholder, since we will multiply it later with the scale function value
            arma::Col<double> scaleValues = arma::ones<arma::Col<double>>(numElem) * scaleFunction(time);
            return scaleValues;
        };  
    }
    // return input - (this->scaleFunction(time)*input);
    return input - this->scaleFunctionVectorized(time) % input;
}

arma::Col<double> DissipationModelScaled::dissipationTerm(arma::Col<double> input, double time){
    return this->scaleFunction(time)*input;
}
