/**
 * @file PropagationModel.hxx
 * @ingroup Core
 * @brief Defines the PropagationModel class used for managing propagation dynamics for the computation of the perturbation in MASFENON.
 * @details The PropagationModel class provides methods for applying propagation logic to the perturbation computation.
 * @details The propagation class uses a scale function to determine the scaling of the propagation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 * @warning This class is an abstract class and should not be used directly. It is intended to be inherited by other classes that implement specific propagation models.
 * @todo Make it stateful. Related to issue #28 @link
 */
#pragma once
#include <armadillo>
#include <functional>
#include <vector>
#include "data_structures/WeightedEdgeGraph.hxx"

class PropagationModel{
    protected:
        std::function<double(double)> scaleFunction;
    public:
        //PropagationModel();
        //PropagationModel(std::function<double(double)> scaleFunction);
        virtual ~PropagationModel(){}
        //using the scale function as a parameter itself, dependency injection
        virtual arma::Col<double> propagate(arma::Col<double> input, double time) = 0;
        virtual arma::Col<double> propagationTerm(arma::Col<double> input, double time) = 0;

        //getters and setters
        std::function<double(double)> getScaleFunction(){return this->scaleFunction;}
        void setScaleFunction(std::function<double(double)> scaleFunction){this->scaleFunction = scaleFunction;}
};