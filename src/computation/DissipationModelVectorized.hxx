/**
 * @file DissipationModelVectorized.hxx
 * @ingroup Experimental
 * @brief Defines the DissipationModelVectorized class used for managing vectorized dissipation dynamics for the computation of the perturbation in MASFENON. This class is an abstract class.
 * @warning This class is an abstract class and should not be used directly. It is intended to be inherited by other classes that implement specific dissipation models.
 */
#pragma once
#include <armadillo>

class DissipationModelVectorized{
    protected:
        int numEl;
    public:
        //DissipationModelVectorized();
        virtual ~DissipationModelVectorized(){}
        virtual arma::Mat<double> dissipate(arma::Mat<double> input,double time) = 0;
        virtual arma::Mat<double> dissipationTerm(arma::Mat<double> input, double time) = 0;
        int getNumEl(){return this->numEl;}
        void setNumEl(int numEl){this->numEl = numEl;}
};