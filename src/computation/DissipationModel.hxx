/**
 * @file DissipationModel.hxx
 * @ingroup Core
 * @brief Defines the DissipationModel class used for managing dissipation dynamics for the computation of the perturbation in MASFENON. This class is an abstract class.
 * @warning This class is an abstract class and should not be used directly. It is intended to be inherited by other classes that implement specific dissipation models.
 * @details The DissipationModel class provides methods for applying dissipation logic to the perturbation computation 
 */
#pragma once
#include <armadillo>

class DissipationModel{
    protected:
        int numEl;
    public:
        //DissipationModel();
        virtual ~DissipationModel(){}
        virtual arma::Col<double> dissipate(arma::Col<double> input,double time) = 0;
        virtual arma::Col<double> dissipationTerm(arma::Col<double> input, double time) = 0;
        // arma::Col<double> dissipatePow2Self(arma::Col<double> input);
        // arma::Col<double> dissipateSelfScaled(arma::Col<double> input, double scale);
        // arma::Col<double> dissipateSelfScaled(arma::Col<double> input, arma::Col<double> scales);
        // arma::Col<double> dissipateSelfPeriodic(arma::Col<double> input, double period, double amplitude, double phase);
        // arma::Col<double> dissipateSelfPeriodic(arma::Col<double> input, arma::Col<double> periods, arma::Col<double> amplitudes, arma::Col<double> phases);
        // arma::Col<double> dissipateSelfPeriodicShift(arma::Col<double> input, double shiftY, double period, double amplitude, double phase);
        // arma::Col<double> dissipateSelfPeriodicShift(arma::Col<double> input, arma::Col<double> shiftY, arma::Col<double> periods, arma::Col<double> amplitudes, arma::Col<double> phases);
        // arma::Col<double> dissipateSelfRandom(arma::Col<double> input, double min, double max);
        int getNumEl(){return this->numEl;}
        void setNumEl(int numEl){this->numEl = numEl;}
};