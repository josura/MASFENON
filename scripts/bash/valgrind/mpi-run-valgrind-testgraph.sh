#!  /bin/bash
graphsFolder="/home/josura/Projects/ccc/MASFENON/data/testdata/testHeterogeneousTemporalGraphMultipleInteractions/graphs"
initialPerturbationFolder="/home/josura/Projects/ccc/MASFENON/data/testdata/testHeterogeneousTemporalGraphMultipleInteractions/initialValuesPartialTypes"
typeInteractionsFolder="/home/josura/Projects/ccc/MASFENON/data/testdata/testHeterogeneousTemporalGraphMultipleInteractions/interactions"
nodesFolder="/home/josura/Projects/ccc/MASFENON/data/testdata/testHeterogeneousTemporalGraphMultipleInteractions/nodesDescriptionDifferentStructure"

outputFolder="/tmp/outputValgrind"
dissipationScaleFactor=0.2
propagationScaleFactor=0.5

mpirun --mca pml ob1 --mca btl tcp,self --mca btl_tcp_if_include wlan0 -np 2 valgrind --suppressions=$PREFIX/share/openmpi/openmpi-valgrind.supp --leak-check=yes --log-file="/tmp/valgrind-testgraph.log" ./build/masfenon-MPI \
        --graphsFilesFolder $graphsFolder \
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
        --outputFolder $outputFolder