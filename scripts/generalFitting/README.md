# FITTING
This directory contains scripts and data for fitting models to synthetic time series data. These scripts are designed to test the fitting capabilities of the MASFENON framework and to be easily adaptable for different types of time series data.
The fitting process is composed of more steps:
- create error matrix
- generate the parameters with default values, or update them based on old parameters and errors

# TESTING
To test the single scripts and their own functionalities run the following parts

## CREATING THE ERROR MATRIX
Creating error matrix for a defined simulation:
- first run the simulation
```bash
../../build/masfenon-MPI --graphsFilesFolder  ../../data/testdata/testHeterogeneousGraph/graphs \
                     --initialPerturbationPerTypeFolder  ../../data/testdata/testHeterogeneousGraph/initialValuesPartialTypes \
                     --typeInteractionFolder  ../../data/testdata/testHeterogeneousGraph/interactions \
                     --propagationModel customPropagation \
                     --propagationModelParameterFolder ../../data/testdata/testHeterogeneousTemporalGraphMultipleInteractions/parameters/propagationParametersAllEqual \
                     --dissipationModel custom \
                     --dissipationModelParameterFolder ../../data/testdata/testHeterogeneousTemporalGraphMultipleInteractions/parameters/dissipationParametersAllEqual \
                     --conservationModel custom \
                     --conservationModelParameterFolder ../../data/testdata/testHeterogeneousTemporalGraphMultipleInteractions/parameters/conservationParametersAllEqual \
                     --virtualNodesGranularity typeAndNode \
                     --saturation \
                     --verbose \
                     --outputFormat iterationMatrix \
                     --outputFolder /tmp/testingCustomFunctionsVectorizedFinal
```