#!/usr/bin/env bash
set -euo pipefail

# -----------------------------
# MAS Fitting Driver (Bash)
# -----------------------------
# Requirements:
# - bash, awk, python3
# - Scripts available/executable:
#     createErrorMatrix.py
#     createParametersIterationAware.py
#     computeRMSE-fromErrorMatrix.py
# - MASFENON simulator executable (invoked under mpirun)
#
# Usage example:
#   ./fit_driver.sh \
#     --epochs 10 \
#     --graphs /path/graphs \
#     --nodes /path/nodes \
#     --interactions /path/interactions \
#     --real-data-dir /path/real_series \
#     --out /tmp/fitting_output \
#     --simulator /path/to/masfenon_exe \
#     --mpirun-np 8 \
#     --mpirun-extra "--mca pml ob1 --mca btl tcp,self --mca btl_tcp_if_include wlan0" \
#     --saturation --verbose
#
# Optional: provide two initial parameter folders (skips 0.5/0.6 auto-gen):
#   --init-params-a /path/paramsA --init-params-b /path/paramsB
#
# Notes:
# - nodes files: per-type TSVs in --nodes; must have a column "Name" with node IDs.
# - real data: per-type TSVs in --real-data-dir; must have "nodeNames" + timepoint columns.
# - params folders contain subfolders:
#       propagationParameters/ dissipationParameters/ conservationParameters/
#   each with per-type TSV files (name, parameters).
# -----------------------------


# ===========================
#  EDITABLE CONSTANTS (inline)
# ===========================
# Simulator + mpirun
SIMULATOR="/home/josura/Projects/ccc/MASFENON/build/masfenon-MPI"     # <- change here if needed for the simulator path
MPIRUN_BIN="mpirun"
MPIRUN_NP=8
MPIRUN_EXTRA="--mca pml ob1 --mca btl tcp,self --mca btl_tcp_if_include wlan0"

# Fixed simulator flags (edit here if needed)
VNG="typeAndNode"
OUTPUT_FORMAT="iterationMatrix"
USE_SATURATION=0
USE_VERBOSE=1

# Helper python scripts (paths)
SCRIPT_ERROR="createErrorMatrix.py"
SCRIPT_PARAMS="createParametersIterationAware.py"
SCRIPT_RMSE="computeRMSE-fromErrorMatrix.py"

# printing configuration
echo "-----------------------------"
echo "MAS Fitting Driver"
echo "-----------------------------"