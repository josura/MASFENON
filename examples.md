
# MPI examples

This page contains a collection of MPI examples of running the framework on test data.

## Homogeneous graph
    
### Example 1: custom propagation model (neighbors if the code in src/propagationModelCustom.cxx is unmodified), scaled dissipation and conservation models set at constants, saturation at default value (1.0), virtual nodes granularity set to type, also save the iteration matrix
```bash
mpirun -np 2 ./build/masfenon-MPI --fUniqueGraph data/testdata/testHomogeneousGraph/edges-Graph1-general.tsv \
            --fInitialPerturbation data/testdata/testHomogeneousGraph/initialValues-general.tsv \
            --typeInteractionFolder data/testdata/testHomogeneousGraph/interactions \
            --dissipationModel scaled \
            --dissipationModelParameters 0.2 \
            --conservationModel scaled \
            --conservationModelParameters 0.2 \
            --propagationModel customPropagation \
            --propagationModelParameters 0.2 \
            --saturation \
            --outputFolder outputs/testingMPIsingle \
            --outputFormat iterationMatrix
```

### Example 2: custom propagation model (neighbors if the code in src/propagationModelCustom.cxx is unmodified), scaled dissipation and conservation models set at constants, saturation at default value (1.0), virtual nodes granularity set to type and node, also save the iteration matrix
```bash
mpirun -np 2 ./build/masfenon-MPI --fUniqueGraph data/testdata/testHomogeneousGraph/edges-Graph1-general.tsv \
            --fInitialPerturbation data/testdata/testHomogeneousGraph/initialValues-general.tsv \
            --typeInteractionFolder data/testdata/testHomogeneousGraph/interactions \
            --dissipationModel scaled \
            --dissipationModelParameters 0.2 \
            --conservationModel scaled \
            --conservationModelParameters 0.2 \
            --propagationModel customPropagation \
            --propagationModelParameters 0.2 \
            --virtualNodesGranularity typeAndNode \
            --saturation \
            --outputFolder outputs/testingMPISingleGranular \
            --outputFormat iterationMatrix
```

### Example 3: custom propagation model (neighbors if the code in src/propagationModelCustom.cxx is unmodified), scaled dissipation and conservation models set at constants, saturation at default value (1.0), virtual nodes granularity set to type and node, using a folder for the  initial perturbation values, also save the iteration matrix
```bash
mpirun -np 2 ./build/masfenon-MPI --fUniqueGraph data/testdata/testHomogeneousGraph/edges-Graph1-general.tsv \
            --initialPerturbationPerTypeFolder data/testdata/testHomogeneousGraph/initialValues \
            --typeInteractionFolder data/testdata/testHomogeneousGraph/interactions \
            --dissipationModel scaled \
            --dissipationModelParameters 0.2 \
            --conservationModel scaled \
            --conservationModelParameters 0.2 \
            --propagationModel customPropagation \
            --propagationModelParameters 0.2 \
            --virtualNodesGranularity typeAndNode \
            --saturation \
            --outputFolder outputs/testingMPISingleGranular \
            --outputFormat iterationMatrix
```


## Heterogeneous temporal graph

### Example 1: custom propagation model (neighbors if the code in src/propagationModelCustom.cxx is unmodified), scaled dissipation and conservation models set at constants, saturation at default value (1.0), also save the iteration matrix
```bash
mpirun -np 2 ./build/masfenon-MPI --graphsFilesFolder data/testdata/testHeterogeneousGraph/graphs \
            --initialPerturbationPerTypeFolder data/testdata/testHeterogeneousTemporalGraph/initialValuesPartialTypes \
            --typeInteractionFolder data/testdata/testHeterogeneousTemporalGraph/interactions \
            --dissipationModel scaled \
            --dissipationModelParameters 0.2 \
            --conservationModel scaled \
            --conservationModelParameters 0.2 \
            --propagationModel customPropagation \
            --propagationModelParameters 0.2 \
            --virtualNodesGranularity typeAndNode \
            --saturation \
            --outputFolder outputs/testingMPIgranularDifferentProcessors \
            --outputFormat iterationMatrix
```

### Example 2: custom propagation model (neighbors if the code in src/propagationModelCustom.cxx is unmodified), scaled dissipation and conservation models set at constants, saturation at default value (1.0), node description folder provided to get the nodes in the graph, also save the iteration matrix
```bash
mpirun -np 2 ./build/masfenon-MPI --graphsFilesFolder data/testdata/testHeterogeneousGraph/graphs \
            --initialPerturbationPerTypeFolder data/testdata/testHeterogeneousTemporalGraph/initialValuesPartialTypes \
            --typeInteractionFolder data/testdata/testHeterogeneousTemporalGraph/interactions \
            --dissipationModel scaled \
            --dissipationModelParameters 0.2 \
            --conservationModel scaled \
            --conservationModelParameters 0.2 \
            --propagationModel customPropagation \
            --propagationModelParameters 0.2 \
            --virtualNodesGranularity typeAndNode \
            --saturation \
            --nodeDescriptionFolder data/testdata/testHeterogeneousGraph/nodesDescriptionDifferentStructure \
            --outputFolder outputs/testingMPIgranularDifferentProcessors \
            --outputFormat iterationMatrix
```

### Example 3: custom propagation model (neighbors if the code in src/propagationModelCustom.cxx is unmodified), scaled dissipation and conservation models set at constants, saturation at default value (1.0), node description folder provided to get the nodes in the graph, using temporal contact information between types, also save the iteration matrix
```bash
mpirun -np 2 ./build/masfenon-MPI --graphsFilesFolder data/testdata/testHeterogeneousTemporalGraph/graphs \
            --initialPerturbationPerTypeFolder data/testdata/testHeterogeneousTemporalGraph/initialValuesPartialTypes \
            --typeInteractionFolder data/testdata/testHeterogeneousTemporalGraph/interactions \
            --dissipationModel scaled \
            --dissipationModelParameters 0.2 \
            --conservationModel scaled \
            --conservationModelParameters 0.2 \
            --propagationModel customPropagation \
            --propagationModelParameters 0.2 \
            --virtualNodesGranularity typeAndNode \
            --saturation \
            --nodeDescriptionFolder data/testdata/testHeterogeneousGraph/nodesDescriptionDifferentStructure \
            --outputFolder outputs/testingMPIgranularDifferentProcessors \
            --outputFormat iterationMatrix
```


### Example 4: custom propagation model (neighbors if the code in src/propagationModelCustom.cxx is unmodified), scaled dissipation and conservation models set at constants, no saturation, node description folder provided to get the nodes in the graph, using temporal contact information between types, also save the iteration matrix
```bash
mpirun -np 2 ./build/masfenon-MPI --graphsFilesFolder data/testdata/testHeterogeneousTemporalGraph/graphs \
            --initialPerturbationPerTypeFolder data/testdata/testHeterogeneousTemporalGraph/initialValuesPartialTypes \
            --typeInteractionFolder data/testdata/testHeterogeneousTemporalGraph/interactions \
            --dissipationModel scaled \
            --dissipationModelParameters 0.2 \
            --conservationModel scaled \
            --conservationModelParameters 0.2 \
            --propagationModel customPropagation \
            --propagationModelParameters 0.2 \
            --virtualNodesGranularity typeAndNode \
            --nodeDescriptionFolder data/testdata/testHeterogeneousGraph/nodesDescriptionDifferentStructure \
            --outputFolder outputs/testingMPIgranularDifferentProcessors \
            --outputFormat iterationMatrix
```