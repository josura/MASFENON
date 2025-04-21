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
 * @details The dissipation scaling function is used to adjust the values of the nodes in the augmented graph based on the current state of the system. The dissipation factor should be between 0 and 1 to avoid producing negative values in the system dynamics.
 */
std::function<double(double)> getDissipationScalingFunction();

/**
 * @brief Generates a scaling function for conservation.
 * @return A theta function that takes a double value and returns a double value.
 * @details The conservation scaling function is used to adjust the values of the nodes in the augmented graph based on the current state of the system. The conservation factor should also lie between 0 and 1 to ensure values remain within logical bounds.
 */
std::function<double(double)> getConservationScalingFunction();

/**
 * @brief Generates a scaling function for propagation.
 * @return A omega function that takes a double value and returns a double value.
 * @details The propagation scaling function is used to adjust the values of the nodes in the augmented graph based on the current state of the system. The propagation scaling can be tuned dynamically to reflect varying transmission/influence strength over time.
 */
std::function<double(double)> getPropagationScalingFunction();

/**
 * @brief Generates a saturation function. Should clamps a value between [-saturation, +saturation].
 * @return A saturation function that takes two double values(input value and saturation value) and returns a double value. 
 * @details The saturation function is used to limit the values of the nodes in the augmented graph. Used to prevent node values from exceeding defined logical limits in the augmented graph.
 */
std::function<double(double,double)> getSaturationFunction();