graphsFolder="/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/graphs"
initialPerturbationFolder="/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/nodeValues"
typeInteractionsFolder="/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/interactions"
nodesFolder="/home/josura/Projects/ccc/datiIdo/inputGraphs/1h/nodes"

mpirun --mca pml ob1 --mca btl tcp,self --mca btl_tcp_if_include wlan0 -np 12 ./build/c2c-sepia-MPI --graphsFilesFolder $graphsFolder \
        --initialPerturbationPerTypeFolder $initialPerturbationFolder \
        --typeInteractionFolder $typeInteractionsFolder \
        --nodeDescriptionFolder $nodesFolder \
        --dissipationModel scaled \
        --dissipationModelParameters $dissipationScaleFactor \
        --propagationModel neighbors \
        --propagationModelParameters $propagationScaleFactor \
        --intertypeIterations 20 \
        --intratypeIterations 5 \
        --timestep 0.1 \
        --virtualNodesGranularity typeAndNode \
        --saturation \
        --undirectedEdges \
        --undirectedTypeEdges \
        --outputFolder $outputFolder"