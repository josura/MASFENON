#!  /bin/bash
# Look into the simulation.conf file in this folder to see the environment variables used for the simulation.
if [ "$#" -ne 9 ]; then
    echo "Usage: $0 dissipationMin dissipationMax dissipationSteps propagationMin propagationMax propagationSteps conservationMin conservationMax conservationSteps"
    exit 1
fi

# Read command-line arguments
dissMin=$1; dissMax=$2; dissSteps=($3*1.0)
propMin=$4; propMax=$5; propSteps=($6*1.0)
consMin=$7; consMax=$8; consSteps=($9*1.0)

# read environment variables from config file, 
# WARNING: to change according to the target system
source /home/josura/Projects/ccc/MASFENON/scripts/bash/singleCell/simulation.conf

echo "graphsFolder set to $graphsFolder"
echo "initialPerturbationFolder set to $initialPerturbationFolder"
echo "typeInteractionsFolder set to $typeInteractionsFolder"
echo "nodesFolder set to $nodesFolder"
echo "allOutputsFolder set to $allOutputsFolder"

# Compute step sizes with awk
dissStep=$(awk -v min="$dissMin" -v max="$dissMax" -v steps="$dissSteps" 'BEGIN { if (steps <= 1) print 0; else print (max - min) / (steps - 1) }')
propStep=$(awk -v min="$propMin" -v max="$propMax" -v steps="$propSteps" 'BEGIN { if (steps <= 1) print 0; else print (max - min) / (steps - 1) }')
consStep=$(awk -v min="$consMin" -v max="$consMax" -v steps="$consSteps" 'BEGIN { if (steps <= 1) print 0; else print (max - min) / (steps - 1) }')

# Generate the value ranges
dissipationScaleFactors=($(seq -f "%.6f" $dissMin $dissStep $dissMax))
propagationScaleFactors=($(seq -f "%.6f" $propMin $propStep $propMax))
conservationScaleFactors=($(seq -f "%.6f" $consMin $consStep $consMax))


for dissipationScaleFactor in "${dissipationScaleFactors[@]}"; do
    for propagationScaleFactor in "${propagationScaleFactors[@]}"; do
        for conservationScaleFactor in "${conservationScaleFactors[@]}"; do
                outputFolder="$allOutputsFolder/dissipation_$dissipationScaleFactor-propagation_$propagationScaleFactor-conservation_$conservationScaleFactor"
                mkdir -p $outputFolder

                echo "Running simulation with dissipation: $dissipationScaleFactor, propagation: $propagationScaleFactor, conservation: $conservationScaleFactor"
                mpirun --mca pml ob1 --mca btl tcp,self --mca btl_tcp_if_include wlan0 -np 8 ./build/masfenon-MPI \
                        --graphsFilesFolder $graphsFolder \
                        --initialPerturbationPerTypeFolder $initialPerturbationFolder \
                        --typeInteractionFolder $typeInteractionsFolder \
                        --nodeDescriptionFolder $nodesFolder \
                        --dissipationModel scaled \
                        --dissipationModelParameters $dissipationScaleFactor \
                        --propagationModel neighbors \
                        --propagationModelParameters $propagationScaleFactor \
                        --conservationModel scaled \
                        --conservationModelParameters $conservationScaleFactor \
                        --intertypeIterations 10 \
                        --intratypeIterations 5 \
                        --timestep 1 \
                        --virtualNodesGranularity typeAndNode \
                        --saturation \
                        --saturationTerm 0.05 \
                        --outputFormat iterationMatrix \
                        --outputFolder $outputFolder
        done
    done
done
