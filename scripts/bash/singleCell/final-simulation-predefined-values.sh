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
echo "allOutputsFolder set to $allOutputsFolder"

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


outputFolder="$allOutputsFolder/dissipation_$dissipationScaleFactor-propagation_$propagationScaleFactor-conservation_$conservationScaleFactor"
mkdir -p "$outputFolder"
# also generating the currentPerturbations folder and the iterationMatrices folder, to account for issue #38
# mkdir -p $outputFolder/currentPerturbations  # not needed, as currentPerturbations are not generated in this simulation
mkdir -p "$outputFolder/iterationMatrices"

echo "Running simulation with dissipation: $dissipationScaleFactor, propagation: $propagationScaleFactor, conservation: $conservationScaleFactor"
echo """mpirun --mca pml ob1 --mca btl tcp,self --mca btl_tcp_if_include wlan0 -np 8 ./build/masfenon-MPI \
        --graphsFilesFolder $graphsFolder \
        --initialPerturbationPerTypeFolder $initialPerturbationFolder \
        --typeInteractionFolder $typeInteractionsFolder \
        --nodeDescriptionFolder $nodesFolder \
        --dissipationModel custom \
        --dissipationModelParameters $dissipationScaleFactor6h $dissipationScaleFactor7h $dissipationScaleFactor \
        --propagationModel customScalingNeighbors \
        --propagationModelParameters $propagationScaleFactor6h $propagationScaleFactor7h $propagationScaleFactor \
        --conservationModel custom \
        --conservationModelParameters $conservationScaleFactor6h $conservationScaleFactor7h $conservationScaleFactor \
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
        --dissipationModelParameters "$dissipationScaleFactor6h" "$dissipationScaleFactor" "$dissipationScaleFactor" \
        --propagationModel customScalingNeighbors \
        --propagationModelParameters "$propagationScaleFactor6h" "$propagationScaleFactor" "$propagationScaleFactor" \
        --conservationModel custom \
        --conservationModelParameters "$conservationScaleFactor6h" "$conservationScaleFactor" "$conservationScaleFactor" \
        --intertypeIterations 10 \
        --intratypeIterations 5 \
        --timestep 1 \
        --virtualNodesGranularity typeAndNode \
        --outputFormat iterationMatrix \
        --outputFolder "$outputFolder"
        # --saturation \
        # --saturationTerm 0.05 \