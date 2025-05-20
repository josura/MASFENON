#! /bin/bash
# This bash script will fit the framework with the data that is used for the single cell experiments. In this case, it will fit the simulation to better represent a single type of cell metabolite layer (AT1) in our scRNA-seq data.
# From the MSE table, the fitting will 
# The script takes the following command line parameters:
## - The number of epochs:
## - The MSE output folder where to save the MSE results:
## - The initial scaling parameters <min,max,intervals> to start the simulation (they will be the same for each scaling parameter, meaning they will start from the same minimum and maximum, with the same interval) 
# Every epoch generates three MSE files, representing the three timepoint that we have available for our data (6h,7h,10h)
# TODO make the script more generic for different kind of data.
# the bash/python script used to generate the MSE ranking for each timepoint are :
# - for generating the MSE sorted tables for each experiment:  /home/josura/Projects/ccc/c2c-sepia/scripts/python/temporalSingleCell/compute-MSE-metabolites.py
## This python code generates the MSE sorted tables, from the single cell data, this code is available in the c2c-sepia repository at https://github.com/josura/c2c-sepia
## The data used to fit the experiments is unavailable to the public at the moment
## IMPORTANT: This python code can be substituted with a code that generates some MSE sorted tables in <mse_experiment.tsv> files with the following schema:
### MSE	experiment
### <MSE-value> dissipation_<dissValue>-propagation_<propaValue>-conservation_<consValue>
# - for running the simulation with custom parameters: /home/josura/Projects/ccc/MASFENON/scripts/bash/singleCell/simulation_custom_parameters.sh
## This bash script run the masfenon simulation with the selected parameters on the data defined in the configuration section, the parameter used are the following:
## dissipationMin dissipationMax dissipationSteps propagationMin propagationMax propagationSteps conservationMin conservationMax conservationStep

# Input parameters
if [ "$#" -ne 5 ]; then
    echo "Usage: $0 <epochs> <MSEfolder> <minScaleParameter> <maxScaleParameter> <intervalParameter>"
    exit 1
fi

epochs=$1
MSEfolder=$2
minParam=$3
maxParam=$4
intervals=$5

# Paths to scripts
SIMULATION_SCRIPT="/home/josura/Projects/ccc/MASFENON/scripts/bash/singleCell/simulation_custom_parameters.sh"
COMPUTE_MSE_SCRIPT="/home/josura/Projects/ccc/c2c-sepia/scripts/python/temporalSingleCell/compute-MSE-metabolites.py"


# propagation, dissipation, conservation parameters
minPropagationParam=$minParam
maxPropagationParam=$maxParam
propagationIntervals=$intervals
minDissipationParam=$minParam
maxDissipationParam=$maxParam
dissipationIntervals=$intervals
minConservationParam=$minParam
maxConservationParam=$maxParam
conservationIntervals=$intervals

