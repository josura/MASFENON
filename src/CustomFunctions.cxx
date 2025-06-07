/**
 * @file CustomFunctions.cxx
 * @ingroup Core
 * @brief Implementation of utility functions for scaling and saturation in the MASFENON framework.
 * @details Provides lambda-based function generators for dissipation, conservation, and propagation scaling,
 * as well as saturation limiting to keep node values within desired bounds.
 */

 #include "CustomFunctions.hxx"

 /**
  * @brief Generates a constant dissipation scaling function.
  * @return A lambda function returning a constant dissipation factor.
  * @details The dissipation factor should be between 0 and 1 to avoid producing negative values in the system dynamics.
  */
 std::function<double(double)> getDissipationScalingFunction() {
    return [](double x) {
        // Constant dissipation factor (can be modified for dynamic behavior)
        return 0.5;
    };
}

/**
 * @brief Generates a constant dissipation scaling function with parameters.
 * @return A lambda function returning a constant dissipation factor.
 * @details The dissipation factor should be between 0 and 1 to avoid producing negative values in the system dynamics.
 */
std::function<double(double)> getDissipationScalingFunction(std::vector<double> parameters) {
    return [](double x) {
        // Constant dissipation factor (can be modified for dynamic behavior)
        // Parameters can be used to adjust the factor if needed
        return 0.5; // Example constant value
    };
}
 
/**
 * @brief Generates a constant conservation scaling function.
 * @return A lambda function returning a constant conservation factor.
 * @details The conservation factor should also lie between 0 and 1 to ensure values remain within logical bounds.
 */
std::function<double(double)> getConservationScalingFunction() {
    return [](double x) {
        // Constant conservation factor (can be modified dynamically)
        return 0.5;
    };
}

/**
 * @brief Generates a constant conservation scaling function with parameters.
 * @return A lambda function returning a constant conservation factor.
 * @details The conservation factor should also lie between 0 and 1 to ensure values remain within logical bounds.
 */
std::function<double(double)> getConservationScalingFunction(std::vector<double> parameters) {
    return [](double x) {
        // Constant conservation factor (can be modified dynamically)
        // Parameters can be used to adjust the factor if needed
        return 0.5; // Example constant value
    };
}

/**
 * @brief Generates a constant propagation scaling function.
 * @return A lambda function returning a constant propagation factor.
 * @details The propagation scaling can be tuned dynamically to reflect varying transmission/influence strength over time.
 */
std::function<double(double)> getPropagationScalingFunction() {
    return [](double x) {
        // Constant propagation factor
        return 0.5;
    };
}

/**
 * @brief Generates a constant propagation scaling function with parameters.
 * @return A lambda function returning a constant propagation factor.
 * @details The propagation scaling can be tuned dynamically to reflect varying transmission/influence strength over time.
 */
std::function<double(double)> getPropagationScalingFunction(std::vector<double> parameters) {
    return [](double x) {
        // Constant propagation factor (can be modified dynamically)
        // Parameters can be used to adjust the factor if needed
        return 0.5; // Example constant value
    };
}

/**
 * @brief Generates a saturation function with upper and lower bounds.
 * @return A lambda function that clamps a value between [-saturation, +saturation].
 * @details Used to prevent node values from exceeding defined logical limits in the augmented graph.
 */
std::function<double(double,double)> getSaturationFunction() {
    return [](double value, double saturation) -> double {
        if (value > saturation) return saturation;
        else if (value < -saturation) return -saturation;
        else return value;
    };
}
 