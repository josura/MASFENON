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
# - SIMULATION_SCRIPT for generating the MSE sorted tables for each experiment:  /home/josura/Projects/ccc/c2c-sepia/scripts/python/temporalSingleCell/compute-MSE-metabolites.py
## This python code generates the MSE sorted tables, from the single cell data, this code is available in the c2c-sepia repository at https://github.com/josura/c2c-sepia
## The data used to fit the experiments is unavailable to the public at the moment
## IMPORTANT: This python code can be substituted with a code that generates some MSE sorted tables in <mse_experiment.tsv> files with the following schema:
### MSE	experiment
### <MSE-value> dissipation_<dissValue>-propagation_<propaValue>-conservation_<consValue>
# - COMPUTE_MSE_SCRIPT for running the simulation with custom parameters: /home/josura/Projects/ccc/MASFENON/scripts/bash/singleCell/simulation_custom_parameters.sh
## This bash script run the masfenon simulation with the selected parameters on the data defined in the configuration section, the parameter used are the following:
## dissipationMin dissipationMax dissipationSteps propagationMin propagationMax propagationSteps conservationMin conservationMax conservationStep
# - GENERATE_NEW_PARAMS_SCRIPT for generating the new parameters from one MSE table generated with the previous script

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
GENERATE_NEW_PARAMS_SCRIPT="/home/josura/Projects/ccc/MASFENON/scripts/bash/singleCell/createNewParametersFromMSEtable.sh"

VENV_FOLDER="/home/josura/Projects/ccc/c2c-sepia/scripts/python/temporalSingleCell/venv"


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

# Sourcing python environment for the python code generating the MSE tables
source "$VENV_FOLDER/bin/activate"
# Epoch loop
for ((epoch=1; epoch<=epochs; epoch++)); do
    echo "=== Epoch $epoch / $epochs ==="

    # Create a subfolder for this epoch's outputs
    epochOutput="$MSEfolder/epoch_$epoch"
    mkdir -p "$epochOutput"

    echo "[INFO] Running simulation (epoch $epoch)..."
    bash "$SIMULATION_SCRIPT" \
        "$minDissipationParam" "$maxDissipationParam" "$dissipationIntervals" \
        "$minPropagationParam" "$maxPropagationParam" "$propagationIntervals" \
        "$minConservationParam" "$maxConservationParam" "$conservationIntervals"

    echo "[INFO] Computing MSE for epoch $epoch..."
    python3 "$COMPUTE_MSE_SCRIPT" --outputFolder "$epochOutput"

    echo "[DONE] Epoch $epoch completed."
    echo
    # TODO update the parameters based on the MSE table generated, or use another script that generates the new parameters from the MSE table.
done

echo "=== All epochs completed. MSE results saved in: $MSEfolder ==="
