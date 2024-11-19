#pragma once
#include <armadillo>
#include <functional>

class ConservationModelVectorized{
    protected:
        std::function<double(double)> scaleFunction;
    public:
        ConservationModelVectorized();
        ConservationModelVectorized(std::function<double(double)> scaleFunction);
        virtual ~ConservationModelVectorized();
        //using the scale function as a parameter itself, dependency injection
        virtual arma::Mat<double> conservate(arma::Mat<double> input, arma::Mat<double> inputDissipated,arma::Mat<double> Wstar, double time, std::vector<double> q = std::vector<double>());
        virtual arma::Mat<double> conservationTerm(arma::Mat<double> input,arma::Mat<double> Wstar, double time, std::vector<double> q = std::vector<double>());

        //getters and setters
        std::function<double(double)> getScaleFunction(){return this->scaleFunction;}
        void setScaleFunction(std::function<double(double)> scaleFunction){this->scaleFunction = scaleFunction;}
};