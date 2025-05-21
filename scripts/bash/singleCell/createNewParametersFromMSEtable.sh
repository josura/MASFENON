#!/bin/bash

# returning the new parameters ranges after computing them from the MSE table passed with command line argument
## The table has the following data(header is "MSE experiment")
## <MSE-value> dissipation_<dissValue>-propagation_<propaValue>-conservation_<consValue>
# The program takes parameters
# - The MSE file
# - The number of records to use for the generation of the new parameters(top n MSE)
# - The number of intervals

if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <mse_file> <top_n> <intervals>"
    exit 1
fi

mse_file="$1"
top_n="$2"
intervals="$3"
