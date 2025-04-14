# control if the output folder exists
if [ ! -d "/tmp/outputExamples" ]; then
    mkdir /tmp/outputExamples
fi


# run the examples
## homogeneous graphs
mpirun --mca pml ob1 --mca btl tcp,self --mca btl_tcp_if_include wlan0 -np 2 ./build/masfenon-MPI --fUniqueGraph data/testdata/testHomogeneousGraph/edges-Graph1-general.tsv \
            --fInitialPerturbation data/testdata/testHomogeneousGraph/initialValues-general.tsv \
            --typeInteractionFolder data/testdata/testHomogeneousGraph/interactions \
            --dissipationModel scaled \
            --dissipationModelParameters 0.2 \
            --conservationModel scaled \
            --conservationModelParameters 0.2 \
            --propagationModel customPropagation \
            --propagationModelParameters 0.2 \
            --saturation \
            --outputFolder /tmp/outputExamples/testingMPIsingle \
            --outputFormat iterationMatrix

mpirun --mca pml ob1 --mca btl tcp,self --mca btl_tcp_if_include wlan0 -np 2 ./build/masfenon-MPI --fUniqueGraph data/testdata/testHomogeneousGraph/edges-Graph1-general.tsv \
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
            --outputFolder /tmp/outputExamples/testingMPISingleGranular \
            --outputFormat iterationMatrix

mpirun --mca pml ob1 --mca btl tcp,self --mca btl_tcp_if_include wlan0 -np 2 ./build/masfenon-MPI --fUniqueGraph data/testdata/testHomogeneousGraph/edges-Graph1-general.tsv \
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
            --outputFolder /tmp/outputExamples/testingMPISingleGranular-2 \
            --outputFormat iterationMatrix

## heterogeneous graphs
mpirun --mca pml ob1 --mca btl tcp,self --mca btl_tcp_if_include wlan0 -np 2 ./build/masfenon-MPI --graphsFilesFolder data/testdata/testHeterogeneousGraph/graphs \
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
            --outputFolder /tmp/outputExamples/testingMPIgranularDifferentProcessors \
            --outputFormat iterationMatrix

mpirun --mca pml ob1 --mca btl tcp,self --mca btl_tcp_if_include wlan0 -np 2 ./build/masfenon-MPI --graphsFilesFolder data/testdata/testHeterogeneousGraph/graphs \
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
            --outputFolder /tmp/outputExamples/testingMPIgranularDifferentProcessors-2 \
            --outputFormat iterationMatrix

mpirun --mca pml ob1 --mca btl tcp,self --mca btl_tcp_if_include wlan0 -np 2 ./build/masfenon-MPI --graphsFilesFolder data/testdata/testHeterogeneousTemporalGraph/graphs \
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
            --outputFolder /tmp/outputExamples/testingMPIgranularDifferentProcessors-3 \
            --outputFormat iterationMatrix

mpirun --mca pml ob1 --mca btl tcp,self --mca btl_tcp_if_include wlan0 -np 2 ./build/masfenon-MPI --graphsFilesFolder data/testdata/testHeterogeneousTemporalGraph/graphs \
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
            --outputFolder /tmp/outputExamples/testingMPIgranularDifferentProcessors-4 \
            --outputFormat iterationMatrix