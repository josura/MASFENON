#pragma once
#include <functional>
#include "utils/mathUtilities.hxx"

//add scaling function as lambda function for dissipation
std::function<double(double)> getDissipationScalingFunction();

//add scaling function as lambda function for conservation
std::function<double(double)> getConservationScalingFunction();

//add scaling function as lambda function for propagation
std::function<double(double)> getPropagationScalingFunction();

// saturation function
std::function<double(double,double)> getSaturationFunction();