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


VENV_FOLDER="/home/josura/Projects/ccc/c2c-sepia/scripts/python/temporalSingleCell/venv"

# Sourcing python environment for the python code generating the MSE tables
source "$VENV_FOLDER/bin/activate"


timepointToFit="6h"
echo "[INFO] Timepoint to fit set to $timepointToFit"


epochs=""
OutputFolder=""

# propagation, dissipation, conservation parameters
minPropagationParam=""
maxPropagationParam=""
propagationIntervals=""
minDissipationParam=""
maxDissipationParam=""
dissipationIntervals=""
minConservationParam=""
maxConservationParam=""
conservationIntervals=""

# Paths to scripts
SIMULATION_SCRIPT="/home/josura/Projects/ccc/MASFENON/scripts/bash/singleCell/simulation_custom_parameters.sh"
COMPUTE_MSE_SCRIPT="/home/josura/Projects/ccc/c2c-sepia/scripts/python/temporalSingleCell/compute-MSE-metabolites.py"
GENERATE_NEW_PARAMS_SCRIPT="/home/josura/Projects/ccc/MASFENON/scripts/bash/singleCell/createNewParametersFromMSEtable.sh"



# ------------------------------
# Input Argument Parsing Section
# ------------------------------

if [ "$#" -eq 5 ]; then
    # Manual range mode
    mode="manual"
    echo "[INFO] Using manual parameter ranges: [$minParam, $maxParam] with $intervals intervals"
    epochs=$1
    OutputFolder=$2
    # Reading the command line parameters
    minParam=$3
    maxParam=$4
    intervals=$5
    # propagation, dissipation, conservation parameters, from the command line parameters
    minPropagationParam=$minParam
    maxPropagationParam=$maxParam
    propagationIntervals=$intervals
    minDissipationParam=$minParam
    maxDissipationParam=$maxParam
    dissipationIntervals=$intervals
    minConservationParam=$minParam
    maxConservationParam=$maxParam
    conservationIntervals=$intervals
    
elif [ "$#" -eq 2 ]; then
    # Resume mode from file
    mode="resume computation"
    echo "[INFO] RESUME COMPUTATION. Computing ranges from simulation output folder $2 for epochs $1"
    epochs=$1
    OutputFolder=$2
    # TODO also make the intervals be passed as command line argument
    # For now the intervals are set to 3 as a default value
    intervals=4
    echo "[INFO] Using default intervals: $intervals"
    # Create a subfolder for this epoch's outputs
    epochOutput="$OutputFolder/epoch_initial"
    mkdir -p "$epochOutput"
    # Infer the new parameters from the simulated data
    echo "[INFO] Computing MSE for initial parameter selection from simulation output folder"
    
    python3 $COMPUTE_MSE_SCRIPT $epochOutput

    echo "[DONE] Computed MSE for initial parameters."
    echo
    # update the parameters by running the generation of the new parameters script
    bash "$GENERATE_NEW_PARAMS_SCRIPT" \
        "$epochOutput/mse_$timepointToFit.tsv" \
        5 \
        "$intervals" \
        "$epochOutput/newParams_mse_$timepointToFit.txt"
    # Read updated parameters
    while IFS= read -r line; do
        key=$(echo "$line" | cut -d':' -f1)
        min=$(echo "$line" | grep -o 'min=[^,]*' | cut -d'=' -f2)
        max=$(echo "$line" | grep -o 'max=[^,]*' | cut -d'=' -f2)
        steps=$(echo "$line" | grep -o 'intervals=[^ ]*' | cut -d'=' -f2)

        case "$key" in
            dissipation)
                minDissipationParam="$min"
                maxDissipationParam="$max"
                dissipationIntervals="$steps"
                ;;
            propagation)
                minPropagationParam="$min"
                maxPropagationParam="$max"
                propagationIntervals="$steps"
                ;;
            conservation)
                minConservationParam="$min"
                maxConservationParam="$max"
                conservationIntervals="$steps"
                ;;
        esac
    done < "$epochOutput/newParams_mse_${timepointToFit}.txt"
    echo "[DONE] Computed initial parameters"
    
else
    echo "Usage:"
    echo "  $0 <epochs> <OutputFolder> <minParam> <maxParam> <intervals>      # manual range mode"
    echo "  $0 <epochs> <OutputFolder>                                        # resume from previous simulations"
    exit 1
fi



# Epoch loop
for ((epoch=1; epoch<=epochs; epoch++)); do
    echo "=== Epoch $epoch / $epochs ==="
    echo "=== [EPOCH] Parameters: ==="
    echo "=== [EPOCH] Dissipation: [$minDissipationParam,$maxDissipationParam] $dissipationIntervals intervals ==="
    echo "=== [EPOCH] Propagation: [$minPropagationParam,$maxPropagationParam] $propagationIntervals intervals ==="
    echo "=== [EPOCH] Conservation: [$minConservationParam,$maxConservationParam] $conservationIntervals intervals ==="

    # Create a subfolder for this epoch's outputs
    epochOutput="$OutputFolder/epoch_$epoch"
    mkdir -p "$epochOutput"

    echo "[INFO] Running simulation (epoch $epoch)..."
    # TODO make the simulation output folder be passed via command argument in case
    bash "$SIMULATION_SCRIPT" \
        "$minDissipationParam" "$maxDissipationParam" "$dissipationIntervals" \
        "$minPropagationParam" "$maxPropagationParam" "$propagationIntervals" \
        "$minConservationParam" "$maxConservationParam" "$conservationIntervals"

    echo "[INFO] Computing MSE for epoch $epoch..."
    # TODO also make the intervals be passed as command line argument
    python3 $COMPUTE_MSE_SCRIPT $epochOutput

    echo "[DONE] Epoch $epoch completed."
    echo
    # update the parameters by running the generation of the new parameters script
    bash "$GENERATE_NEW_PARAMS_SCRIPT" \
        "$epochOutput/mse_$timepointToFit.tsv" \
        5 \
        "$intervals" \
        "$epochOutput/newParams_mse_$timepointToFit.txt"
    # Read updated parameters
    while IFS= read -r line; do
        key=$(echo "$line" | cut -d':' -f1)
        min=$(echo "$line" | grep -o 'min=[^,]*' | cut -d'=' -f2)
        max=$(echo "$line" | grep -o 'max=[^,]*' | cut -d'=' -f2)
        steps=$(echo "$line" | grep -o 'intervals=[^ ]*' | cut -d'=' -f2)

        case "$key" in
            dissipation)
                minDissipationParam="$min"
                maxDissipationParam="$max"
                dissipationIntervals="$steps"
                ;;
            propagation)
                minPropagationParam="$min"
                maxPropagationParam="$max"
                propagationIntervals="$steps"
                ;;
            conservation)
                minConservationParam="$min"
                maxConservationParam="$max"
                conservationIntervals="$steps"
                ;;
        esac
    done < "$epochOutput/newParams_mse_${timepointToFit}.txt"
done

echo "=== All epochs completed. MSE results saved in: $OutputFolder ==="
