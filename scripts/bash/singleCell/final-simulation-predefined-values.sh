#!  /bin/bash
set -euo pipefail
# Look into the simulation.conf file in this folder to see the environment variables used for the simulation.

# read environment variables from config file, 
# WARNING: to change according to the target system
source /home/josura/Projects/ccc/MASFENON/scripts/bash/singleCell/simulation.conf

echo "graphsFolder set to $graphsFolder"
echo "initialPerturbationFolder set to $initialPerturbationFolder"
echo "typeInteractionsFolder set to $typeInteractionsFolder"
echo "nodesFolder set to $nodesFolder"
echo "finalOutputFolder set to $finalOutputFolder"

# Hardcoded scale parameters for the single timepoints
# TODO fix this with the use of a single file or pass them via command line parameters
dissipationScaleFactor6h=0.280320
propagationScaleFactor6h=0.015737
conservationScaleFactor6h=0.350748

dissipationScaleFactor7h=0.161740
propagationScaleFactor7h=-0.005539
conservationScaleFactor7h=0.164609

dissipationScaleFactor10h=0.161740
propagationScaleFactor10h=-0.005539
conservationScaleFactor10h=0.164609


outputFolder="$finalOutputFolder/dissipation6h_$dissipationScaleFactor6h-propagation6h_$propagationScaleFactor6h-conservation6h_$conservationScaleFactor6h-dissipation7h_$dissipationScaleFactor7h-propagation7h_$propagationScaleFactor7h-conservation7h_$conservationScaleFactor7h-dissipation10h_$dissipationScaleFactor10h-propagation10h_$propagationScaleFactor10h-conservation10h_$conservationScaleFactor10h"
mkdir -p "$outputFolder"
# also generating the currentPerturbations folder and the iterationMatrices folder, to account for issue #38
# mkdir -p $outputFolder/currentPerturbations  # not needed, as currentPerturbations are not generated in this simulation
mkdir -p "$outputFolder/iterationMatrices"

echo "Running simulation with dissipation6h $dissipationScaleFactor6h, propagation6h $propagationScaleFactor6h, conservation6h $conservationScaleFactor6h, dissipation7h $dissipationScaleFactor7h, propagation7h $propagationScaleFactor7h, conservation7h $conservationScaleFactor7h, dissipation10h $dissipationScaleFactor10h, propagation10h $propagationScaleFactor10h, conservation10h $conservationScaleFactor10h"
echo """mpirun --mca pml ob1 --mca btl tcp,self --mca btl_tcp_if_include wlan0 -np 8 ./build/masfenon-MPI \
        --graphsFilesFolder $graphsFolder \
        --initialPerturbationPerTypeFolder $initialPerturbationFolder \
        --typeInteractionFolder $typeInteractionsFolder \
        --nodeDescriptionFolder $nodesFolder \
        --dissipationModel custom \
        --dissipationModelParameters $dissipationScaleFactor6h $dissipationScaleFactor7h $dissipationScaleFactor10h \
        --propagationModel customScalingNeighbors \
        --propagationModelParameters $propagationScaleFactor6h $propagationScaleFactor7h $propagationScaleFactor10h \
        --conservationModel custom \
        --conservationModelParameters $conservationScaleFactor6h $conservationScaleFactor7h $conservationScaleFactor10h \
        --intertypeIterations 10 \
        --intratypeIterations 5 \
        --timestep 1 \
        --virtualNodesGranularity typeAndNode \
        --outputFormat iterationMatrix \
        --outputFolder $outputFolder"""
mpirun --mca pml ob1 --mca btl tcp,self --mca btl_tcp_if_include wlan0 -np 8 ./build/masfenon-MPI \
        --graphsFilesFolder $graphsFolder \
        --initialPerturbationPerTypeFolder $initialPerturbationFolder \
        --typeInteractionFolder $typeInteractionsFolder \
        --nodeDescriptionFolder $nodesFolder \
        --dissipationModel custom \
        --dissipationModelParameters "$dissipationScaleFactor6h" "$dissipationScaleFactor7h" "$dissipationScaleFactor10h" \
        --propagationModel customScalingNeighbors \
        --propagationModelParameters "$propagationScaleFactor6h" "$propagationScaleFactor7h" "$propagationScaleFactor10h" \
        --conservationModel custom \
        --conservationModelParameters "$conservationScaleFactor6h" "$conservationScaleFactor7h" "$conservationScaleFactor10h" \
        --intertypeIterations 10 \
        --intratypeIterations 5 \
        --timestep 1 \
        --virtualNodesGranularity typeAndNode \
        --outputFormat iterationMatrix \
        --outputFolder "$outputFolder"
        # --saturation \
        # --saturationTerm 0.05 \