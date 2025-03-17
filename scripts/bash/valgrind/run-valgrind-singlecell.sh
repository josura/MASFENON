#! /bin/bash
valgrind --leak-check=yes --log-file="valgrind-single-cell.log" ./build/masfenon-MPI \
        --graphsFilesFolder /home/josura/Projects/ccc/datiIdo/inputGraphs/1h/graphsWithLR \
        --initialPerturbationPerTypeFolder /home/josura/Projects/ccc/datiIdo/inputGraphs/1h/nodeValuesWithLR \
        --typeInteractionFolder /home/josura/Projects/ccc/datiIdo/inputGraphs/1h/interactionsWithLR \
        --nodeDescriptionFolder /home/josura/Projects/ccc/datiIdo/inputGraphs/1h/nodesWithLR \
        --dissipationModel scaled \
        --dissipationModelParameters 0.2 \
        --propagationModel neighbors \
        --propagationModelParameters 0.5 \
        --intertypeIterations 20 \
        --intratypeIterations 5 \
        --timestep 0.1 \
        --virtualNodesGranularity typeAndNode \
        --saturation \
        --outputFolder /home/josura/Projects/ccc/datiIdo/inputGraphs/1h/outputWithLR