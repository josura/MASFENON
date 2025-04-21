/** 
 * @file CustomFunctions.hxx 
 * @ingroup Core
 * @brief Contains utility functions for generating scaling and saturation functions used in MASFENON propagation logic.
 * @details The functions are used to create scaling and saturation functions for dissipation, conservation, and propagation.
 * The scaling functions are used to adjust the values of the nodes in the augmented graph based on the current state of the system.
*/
#pragma once
#include <functional>
#include "utils/mathUtilities.hxx"

/**
 * @brief Generates a scaling function for dissipation.
 * @return A lambda function that takes a double value and returns a double value.
 */
std::function<double(double)> getDissipationScalingFunction();

/**
 * @brief Generates a scaling function for conservation.
 * @return A theta function that takes a double value and returns a double value.
 */
std::function<double(double)> getConservationScalingFunction();

/**
 * @brief Generates a scaling function for propagation.
 * @return A omega function that takes a double value and returns a double value.
 */
std::function<double(double)> getPropagationScalingFunction();

/**
 * @brief Generates a saturation function.
 * @return A saturation function that takes two double values and returns a double value.
 * @details The saturation function is used to limit the values of the nodes in the augmented graph.
 */
std::function<double(double,double)> getSaturationFunction();