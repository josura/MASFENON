#!/bin/bash

# returning the new parameters ranges after computing them from the MSE table passed with command line argument
## The table has the following data(header is "MSE experiment")
## <MSE-value> dissipation_<dissValue>-propagation_<propaValue>-conservation_<consValue>
# The program takes parameters
# - The MSE file
# - The number of records to use for the generation of the new parameters(top n MSE)
# - The number of intervals

if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <mse_file> <top_n> <intervals>"
    exit 1
fi

mse_file="$1"
top_n="$2"
intervals="$3"

diss_vals=()
prop_vals=()
cons_vals=()

# Extract top N lines (skip header)
while read -r line; do
    # Extract the parameter string
    experiment=$(echo "$line" | cut -f2)
    # Parse dissipation, propagation, conservation
    diss=$(echo "$experiment" | sed -n 's/.*dissipation_\([0-9.]*\)-.*/\1/p')
    prop=$(echo "$experiment" | sed -n 's/.*propagation_\([0-9.]*\)-.*/\1/p')
    cons=$(echo "$experiment" | sed -n 's/.*conservation_\([0-9.]*\)/\1/p')
    
    diss_vals+=("$diss")
    prop_vals+=("$prop")
    cons_vals+=("$cons")
done < <(tail -n +2 "$mse_file" | head -n "$top_n")

#printing the values read
echo "top 5 dissipation values: ${diss_vals[@]}"
echo "top 5 propagation values: ${prop_vals[@]}"
echo "top 5 conservation values: ${cons_vals[@]}"

# Function to compute min and max in Bash using awk
compute_range() {
    local vals=("$@")
    local min=$(printf "%s\n" "${vals[@]}" | awk 'NR == 1 { min = $1 } $1 < min { min = $1 } END { print min }')
    local max=$(printf "%s\n" "${vals[@]}" | awk 'NR == 1 { max = $1 } $1 > max { max = $1 } END { print max }')
    # Optional: apply 5% padding
    local range=$(awk -v min="$min" -v max="$max" 'BEGIN { print max - min }')
    local pad=$(awk -v r="$range" 'BEGIN { print r * 0.05 }')
    min=$(awk -v min="$min" -v pad="$pad" 'BEGIN { printf "%.6f", min - pad }')
    max=$(awk -v max="$max" -v pad="$pad" 'BEGIN { printf "%.6f", max + pad }')
    echo "$min $max"
}

read diss_min diss_max <<< $(compute_range "${diss_vals[@]}")
read prop_min prop_max <<< $(compute_range "${prop_vals[@]}")
read cons_min cons_max <<< $(compute_range "${cons_vals[@]}")

# Output new parameter ranges
echo "dissipation: min=$diss_min, max=$diss_max, intervals=$intervals"
echo "propagation: min=$prop_min, max=$prop_max, intervals=$intervals"
echo "conservation: min=$cons_min, max=$cons_max, intervals=$intervals"
