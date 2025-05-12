/**
 * @file DissipationModelPow.cxx
 * @ingroup Core
 * @brief Implements the DissipationModelPow class used for managing power dissipation dynamics for the computation of the perturbation in MASFENON.
 * @details The DissipationModelPow class inherits from the DissipationModel class and implements power dissipation logic.
 */
#include "computation/DissipationModelPow.hxx"

DissipationModelPow::DissipationModelPow(){
    this->power = 2;
    this->numEl = 0;
}

DissipationModelPow::DissipationModelPow(double power){
    this->power = power;
    this->numEl = 0;
}

DissipationModelPow::~DissipationModelPow(){
}

arma::Col<double> DissipationModelPow::dissipate(arma::Col<double> input, double time){
    return input - pow(input,this->power);
}

arma::Col<double> DissipationModelPow::dissipationTerm(arma::Col<double> input, double time){
    return pow(input,this->power);
}
