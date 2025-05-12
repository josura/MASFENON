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
}

DissipationModelScaled::~DissipationModelScaled(){
}

DissipationModelScaled::DissipationModelScaled(std::function<double(double)> scaleFun){
    this->scaleFunction = scaleFun;
    this->numEl = 0;
}

arma::Col<double> DissipationModelScaled::dissipate(arma::Col<double> input, double time){
    return input - (this->scaleFunction(time)*input);
}

arma::Col<double> DissipationModelScaled::dissipationTerm(arma::Col<double> input, double time){
    return this->scaleFunction(time)*input;
}
