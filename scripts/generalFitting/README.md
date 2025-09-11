# FITTING
This directory contains scripts and data for fitting models to synthetic time series data. These scripts are designed to test the fitting capabilities of the MASFENON framework and to be easily adaptable for different types of time series data.
The fitting process is composed of more steps:
- create error matrix
- generate the parameters with default values, or update them based on old parameters and errors

# PRELIMINARY STEPS
Preparing the environment
```bash
python -m venv venv
source venv/bin/activate
pip install -r requirements
```

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
                     --propagationModelParameterFolder ../../data/testFitting/parameters/propagationParameters \
                     --dissipationModel custom \
                     --dissipationModelParameterFolder ../../data/testFitting/parameters/dissipationParameters \
                     --conservationModel custom \
                     --conservationModelParameterFolder ../../data/testFitting/parameters/conservationParameters \
                     --virtualNodesGranularity typeAndNode \
                     --saturation \
                     --verbose \
                     --outputFormat iterationMatrix \
                     --outputFolder /tmp/testingCustomFunctionsVectorizedFinal
```
- then create the error matrix
```bash
python createErrorMatrix.py --sim-dir /tmp/testingCustomFunctionsVectorizedFinal/iterationMatrices --real-dir ../../data/testFitting/syntheticTimeSeries --out-dir /tmp/testingErrors 
```
- let's also create another error matrix for another simulation since we will be using it to train initially the framework
```bash
../../build/masfenon-MPI --graphsFilesFolder  ../../data/testdata/testHeterogeneousGraph/graphs \
                     --initialPerturbationPerTypeFolder  ../../data/testdata/testHeterogeneousGraph/initialValuesPartialTypes \
                     --typeInteractionFolder  ../../data/testdata/testHeterogeneousGraph/interactions \
                     --propagationModel customPropagation \
                     --propagationModelParameterFolder ../../data/testFitting/parameters-2/propagationParameters \
                     --dissipationModel custom \
                     --dissipationModelParameterFolder ../../data/testFitting/parameters-2/dissipationParameters \
                     --conservationModel custom \
                     --conservationModelParameterFolder ../../data/testFitting/parameters-2/conservationParameters \
                     --virtualNodesGranularity typeAndNode \
                     --saturation \
                     --verbose \
                     --outputFormat iterationMatrix \
                     --outputFolder /tmp/testingCustomFunctionsVectorizedFinal-2
```
- and create another error matrix
```bash
python createErrorMatrix.py --sim-dir /tmp/testingCustomFunctionsVectorizedFinal-2/iterationMatrices --real-dir ../../data/testFitting/syntheticTimeSeries --out-dir /tmp/testingErrors-2 
```
## CREATING THE NEW PARAMETERS FOR DISSIPATION(ITERATION AWARE)
```bash
python createParametersIterationAware.py --nodes-dir ../../data/testdata/testHeterogeneousTemporalGraphMultipleInteractions/nodesDescriptionDifferentStructure --params-dir ../../data/testdata/testHeterogeneousTemporalGraphMultipleInteractions/parameters/dissipationParameters --prev-params-dir ../../data/testdata/testHeterogeneousTemporalGraphMultipleInteractions/parameters/dissipationParametersAllEqual --errors-dir /tmp/testingErrors --prev-errors-dir /tmp/testingErrors-2 --out-dir /tmp/testingNewParams --lr 0.05
```


# FITTING THE MODEL
To fit the model with some default initial parameters (all 0.5 for the first parameter and all 0.6 for the second parameter)to the synthetic time series data run the following script:
```bash
bash fit_driver.sh --graphs ../../data/testFitting/graphs --nodes ../../data/testFitting/nodesDescriptionDifferentStructure  --initial ../../data/testFitting/initialValues --interactions ../../data/testFitting/interactions --real-data-dir ../../data/testFitting/syntheticTimeSeries --out /tmp/testingFittingDriver --epochs 5
```

To fit the model with some vectorized parameters (all 0.2,0.2,-0.2 for the first set of parameter, and all 0.4,0.4,-0.4 for the second set of parameters) to the synthetic time series data run the following script:
```bash
bash fit_driver.sh --graphs ../../data/testFitting/graphs --nodes ../../data/testFitting/nodesDescriptionDifferentStructure  --initial ../../data/testFitting/initialValues --interactions ../../data/testFitting/interactions --real-data-dir ../../data/testFitting/syntheticTimeSeries --out /tmp/testingFittingDriver --epochs 5 --init-params-a ../../data/testFitting/parameters --init-params-b ../../data/testFitting/parameters-2
```