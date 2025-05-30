/**
 * @file DissipationModelRandom.cxx
 * @ingroup Core
 * @brief Implements the DissipationModelRandom class used for managing random dissipation dynamics for the computation of the perturbation in MASFENON.
 * @details The DissipationModelRandom class inherits from the DissipationModel class and implements random dissipation logic.
 * @details The random dissipation model uses a range of values to compute the dissipation term.
 */
#include "computation/DissipationModelRandom.hxx"
#include <cstddef>

DissipationModelRandom::DissipationModelRandom(){
    this->rangeMin = 0;
    this->rangeMax = 1;
}

DissipationModelRandom::DissipationModelRandom(double rangeMin, double rangeMax){
    this->rangeMin = rangeMin;
    this->rangeMax = rangeMax;
}

DissipationModelRandom::~DissipationModelRandom(){
}

arma::Col<double> DissipationModelRandom::dissipate(arma::Col<double> input, double time){
    arma::Col<double> output = arma::Col<double>(input.n_rows);
    for(size_t i = 0; i < input.n_rows; i++){
        output(i) = input(i) - input(i)*(this->rangeMin + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(this->rangeMax-this->rangeMin))));
    }
    return output;
}

arma::Col<double> DissipationModelRandom::dissipationTerm(arma::Col<double> input, double time){
    arma::Col<double> output = arma::Col<double>(input.n_rows);
    for(size_t i = 0; i < input.n_rows; i++){
        output(i) = input(i)*(this->rangeMin + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(this->rangeMax-this->rangeMin))));
    }
    return output;
}