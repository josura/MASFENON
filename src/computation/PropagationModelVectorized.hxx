#pragma once
#include <armadillo>
#include <functional>
#include <vector>
#include "data_structures/WeightedEdgeGraph.hxx"

class PropagationModelVectorized{
    protected:
        std::function<double(double)> scaleFunction;
    public:
        virtual ~PropagationModelVectorized(){}
        //using the scale function as a parameter itself, dependency injection
        virtual arma::Mat<double> propagate(arma::Mat<double> input, double time) = 0;
        virtual arma::Mat<double> propagationTerm(arma::Mat<double> input, double time) = 0;

        //getters and setters
        std::function<double(double)> getScaleFunction(){return this->scaleFunction;}
        void setScaleFunction(std::function<double(double)> scaleFunction){this->scaleFunction = scaleFunction;}
};