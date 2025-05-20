/**
 * @file DissipationModel.hxx
 * @ingroup Core
 * @brief Defines the DissipationModel class used for managing dissipation dynamics for the computation of the perturbation in MASFENON. This class is an abstract class.
 * @warning This class is an abstract class and should not be used directly. It is intended to be inherited by other classes that implement specific dissipation models.
 * @details The DissipationModel class provides methods for applying dissipation logic to the perturbation computation
 * @details The dissipation class uses a scale function to determine the scaling of the dissipation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 */
#pragma once
#include <armadillo>

/**
 * @class DissipationModel
 * @brief Abstract class for managing dissipation dynamics in MASFENON.
 * @details This class provides methods for applying dissipation logic to the perturbation computation.
 * @warning This class is an abstract class and should not be used directly. It is intended to be inherited by other classes that implement specific dissipation models.
 * @details The dissipation class uses a scale function to determine the scaling of the dissipation term. The scale function can be set and modified as needed.
 * @details To set the scale function, @see CustomFunctions.hxx
 * @todo Make it stateful. Related to issue #28 @link https://github.com/josura/MASFENON/issues/28 @endlink
 */
class DissipationModel{
    protected:
        int numEl; ///< The number of elements in the dissipation model. This is used to determine the size of the input and output vectors.
    public:
        /**
         * @brief Default destructor for the DissipationModel class. (needed even if the class is abstract)
         * @details Cleans up the resources used by the DissipationModel class.
         */
        virtual ~DissipationModel(){}
        /**
         * @brief Dissipate the input vector.
         * @param input The input vector to be processed.
         * @param time The current time.
         * @return The output vector after applying the dissipation model.
         * @details This function is used to compute the final output of the dissipation model.
         */
        virtual arma::Col<double> dissipate(arma::Col<double> input,double time) = 0;
        /**
         * @brief Dissipation term of the input vector.
         * @param input The input vector to be processed.
         * @param time The current time.
         * @return The dissipation term vector.
         * @details This function is used to compute the dissipation term of the input vector.
         */
        virtual arma::Col<double> dissipationTerm(arma::Col<double> input, double time) = 0;
        /**
         * @brief Get the number of elements in the dissipation model.
         * @return The number of elements in the dissipation model.
         * @details This function is used to get the number of elements in the dissipation model.
         */
        int getNumEl(){return this->numEl;}
        /**
         * @brief Set the number of elements in the dissipation model.
         * @param numEl The number of elements to be set.
         * @details This function is used to set the number of elements in the dissipation model.
         */
        void setNumEl(int numEl){this->numEl = numEl;}
};