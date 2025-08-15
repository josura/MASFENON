#!/bin/bash

# control if parameters are passed, if only 2 arguments are passed, the input folder where the other folders are and the number of times every simulation should be run
if [ $# -ne 2 ]; then
    echo "$# arguments passed"
    echo "Please pass the folder with the input folders and the number of times every simulation should be run"
    exit 1
fi

inputsFolder=$1
numIterations=$2

nodesList=$(seq 1000 1000 10000)

# Clear output file to avoid appending to old results
echo -e "nodes\tmemory_kb" > spaceOccupied.tsv

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
        mem_used=$( (/usr/bin/time -f "%M" python simulation.py "$inputName" sir 100) 2>&1 >/dev/null )
        if [ $? -ne 0 ]; then
            echo "Simulation failed for $nodes nodes, iteration $i"
            exit 1
        fi
        echo -e "${nodes}\t${mem_used}" >> spaceOccupied.tsv
        echo "Simulation completed for $nodes nodes, iteration $i"
    done
    echo "All simulations completed for $nodes nodes"
done

echo "All simulations completed for all nodes"
