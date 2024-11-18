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