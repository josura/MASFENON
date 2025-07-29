#! /bin/bash

# control if parameters are passed, if only 2 arguments are passed, the input folder where the other folders are and the number of times every simulation should be run
if [ $# -ne 2 ]; then
    echo $# arguments passed
    echo "Please pass the folder with the inputs folders,and the number of times every simulation should be run"
    exit 1
fi

inputsFolder=$1
numIterations=$2

nodesList=$(seq 1000 1000 10000)

# iterate over all nodes in input folder
for nodes in ${nodesList[@]}; do
    inputName="${inputsFolder}/${nodes}Nodes/edge_data.tsv"
    if [ ! -f "$inputName" ]; then
        echo "Input file $inputName does not exist, skipping..."
        continue
    fi
    echo "Running simulation for $nodes nodes, $numIterations iterations"
    for ((i=1; i<=numIterations; i++)); do
        echo "Iteration $i"
        python simulation.py $inputName sir 100
        if [ $? -ne 0 ]; then
            echo "Simulation failed for $nodes nodes, iteration $i"
            exit 1
        fi
        echo "Simulation completed for $nodes nodes, iteration $i"
    done
    echo "All simulations completed for $nodes nodes"
done

echo "All simulations completed for all nodes"