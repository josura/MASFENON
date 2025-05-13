/**
 * @file ConservationModelVectorized.hxx
 * @ingroup Experimental
 * @brief Defines the ConservationModelVectorized class for vectorized conservation computations.
 * @details The ConservationModelVectorized class provides methods for applying conservation logic to the perturbation computation.
 * @details The conservation class uses a scale function to determine the scaling of the conservation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 */
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