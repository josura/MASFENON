#!  /bin/bash
graphsFolder="/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/graphsWithLR"
initialPerturbationFolder="/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/nodeValuesWithLR"
typeInteractionsFolder="/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/interactionsWithLR"
nodesFolder="/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/nodesWithLR"

allOutputsFolder="/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/outputWithLR"
dissipationScaleFactors=(0 0.1 0.2 0.3)
propagationScaleFactors=(0 0.1 0.2 0.3)
conservationScaleFactors=(0 0.1 0.2 0.3)

for dissipationScaleFactor in "${dissipationScaleFactors[@]}"; do
    for propagationScaleFactor in "${propagationScaleFactors[@]}"; do
        for conservationScaleFactor in "${conservationScaleFactors[@]}"; do
                outputFolder="$allOutputsFolder/dissipation_$dissipationScaleFactor/propagation_$propagationScaleFactor/conservation_$conservationScaleFactor"
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
