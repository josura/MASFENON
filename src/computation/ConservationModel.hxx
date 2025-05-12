/**
 * @file ConservationModel.hxx
 * @ingroup Core
 * @brief Defines the ConservationModel class used for managing conservation dynamics for the computation of the perturbation in MASFENON.
 * @details The ConservationModel class provides methods for applying conservation logic to the perturbation computation. The class is not designed as an abstract class, but rather as a base class for specific conservation models.
 * @details The conservation class uses a scale function to determine the scaling of the conservation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 * @todo Make it stateful. Related to issue #28 @link https://github.com/josura/MASFENON/issues/28 @endlink
 */
#pragma once
#include <armadillo>
#include <functional>

class ConservationModel{
    protected:
        std::function<double(double)> scaleFunction; ///< The function to scale the conservation term. It takes a double value (time) and returns a double value.>
    public:
        ConservationModel();
        ConservationModel(std::function<double(double)> scaleFunction);
        virtual ~ConservationModel();
        //using the scale function as a parameter itself, dependency injection
        virtual arma::Col<double> conservate(arma::Col<double> input, arma::Col<double> inputDissipated,arma::Mat<double> Wstar, double time, std::vector<double> q = std::vector<double>());
        virtual arma::Col<double> conservationTerm(arma::Col<double> input,arma::Mat<double> Wstar, double time, std::vector<double> q = std::vector<double>());

        //getters and setters
        std::function<double(double)> getScaleFunction(){return this->scaleFunction;}
        void setScaleFunction(std::function<double(double)> scaleFunction){this->scaleFunction = scaleFunction;}
};