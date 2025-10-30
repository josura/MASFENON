#!/usr/bin/env bash
set -euo pipefail

# -----------------------------
# MAS Fitting Driver (Bash), with finer gradient building(single parameter perturbation), and selection of network to fit
# -----------------------------
# Requirements:
# - bash, awk, python3
# - Scripts available/executable:
#     createErrorMatrix.py
#     createParametersIterationAware-singleParameter.py
#     computeRMSE-fromErrorMatrix.py
# - MASFENON simulator executable (invoked under mpirun)
# -----------------------------
# TODO - add threshold when the errors of other parameters are too high, meaning that the single parameter perturbation has bigger consequences for other parameters than the one being perturbed


# ===========================
#  EDITABLE CONSTANTS (inline)
# ===========================
# Simulator + mpirun
SIMULATOR="/home/josura/Projects/ccc/MASFENON/build/masfenon-MPI"     # <- change here if needed for the simulator path
MPIRUN_BIN="mpirun"
MPIRUN_NP=4
MPIRUN_EXTRA="--mca pml ob1 --mca btl tcp,self --mca btl_tcp_if_include wlan0"
VIRTUAL_ENV="/home/josura/Projects/ccc/MASFENON/scripts/generalFitting/venv"
source "$VIRTUAL_ENV/bin/activate"

# Fixed simulator flags (edit here if needed)
VNG="typeAndNode"
OUTPUT_FORMAT="iterationMatrix"
USE_SATURATION=0
USE_VERBOSE=0

# Helper python scripts (relative or full paths)
SCRIPT_ERROR="createErrorMatrix.py"
SCRIPT_PARAMS="createParametersIterationAware.py"
SCRIPT_RMSE="computeRMSE-fromErrorMatrix.py"

# Parameter update settings
LR=0.001
GRADIENT_STEP_SIZE=0.0001  # step size for the gradient (Δs) when computing parameter updates
EPS=1e-12
MAX_SCALE=1000      # e.g., 1000 to clip |Δp/(Δs+eps)|; leave empty to disable

# Data conventions
SUFFIX=".tsv"
NODES_NAME_COL="Name"      # column in --nodes files
REAL_NODE_COL="nodeNames"  # row index column in real data & error matrices

# Defaults for auto-generated initial params (when initial parameter not supplied)
DEFAULT_PARAMS=0.01

# ===========================
#  CLI (only IO & epochs)
# ===========================
usage() {
  cat <<EOF
Usage:
  $(basename "$0") --epochs N \\
    --graphs /path/graphs \\
    --nodes  /path/node_descriptions \\
    --initial /path/initial_values \\
    --interactions /path/interactions \\
    --real-data-dir /path/real_series \\
    --out /path/output \\
    [--init-params /path/params]

Notes:
- Only IO and epoch count are CLI-configurable; edit constants at the top for everything else.
- Simulator receives:
    --nodeDescriptionFolder      (from --nodes)
    --initialPerturbationPerTypeFolder (from --initial)
    --graphsFilesFolder (from --graphs)
    --typeInteractionFolder (from --interactions)
- Optionally, the simulator 
- Params folders must contain subdirs:
    propagationParameters/ dissipationParameters/ conservationParameters/
EOF
}

EPOCHS="" GRAPHS="" NODES="" INITIAL="" INTERACTIONS="" REAL_DIR="" OUT_ROOT=""
INIT_PARAMS="" # initial params folder (optional)

while [[ $# -gt 0 ]]; do
  case "$1" in
    --epochs) EPOCHS="$2"; shift 2;;
    --graphs) GRAPHS="$2"; shift 2;;
    --nodes) NODES="$2"; shift 2;;          # node descriptions (no values)
    --initial) INITIAL="$2"; shift 2;;      # initial node values for the sim
    --interactions) INTERACTIONS="$2"; shift 2;;
    --real-data-dir) REAL_DIR="$2"; shift 2;;
    --out) OUT_ROOT="$2"; shift 2;;
    --init-params) INIT_PARAMS="$2"; shift 2;;
    -h|--help) usage; exit 0;;
    *) echo "[error] Unknown arg: $1"; usage; exit 2;;
  esac
done


# printing configuration
echo "-----------------------------"
echo "MASFENON Fitting Driver"
echo "-----------------------------"
echo "Epochs: $EPOCHS"
echo "Graphs: $GRAPHS"
echo "Nodes: $NODES"
echo "Initial values: $INITIAL"
echo "Interactions: $INTERACTIONS"
echo "Real data: $REAL_DIR"
echo "Output root: $OUT_ROOT"
if [[ -n "$INIT_PARAMS" ]]; then
  echo "Init params: $INIT_PARAMS"
else
  echo "Init params: (auto-generated)"
fi
echo "-----------------------------"


# Validate the necessary options
for v in EPOCHS GRAPHS NODES INITIAL INTERACTIONS REAL_DIR OUT_ROOT; do
  [[ -n "${!v}" ]] || { echo "[error] missing --${v,,}"; usage; exit 2; }
done
for d in "$GRAPHS" "$NODES" "$INITIAL" "$INTERACTIONS" "$REAL_DIR"; do
  [[ -d "$d" ]] || { echo "[error] not a directory: $d"; exit 2; }
done
[[ -f "$SCRIPT_ERROR" ]] || { echo "[error] missing $SCRIPT_ERROR"; exit 2; }
[[ -f "$SCRIPT_PARAMS" ]] || { echo "[error] missing $SCRIPT_PARAMS"; exit 2; }
[[ -f "$SCRIPT_RMSE"   ]] || { echo "[error] missing $SCRIPT_RMSE";   exit 2; }
[[ -x "$SIMULATOR" || -f "$SIMULATOR" ]] || { echo "[error] simulator not found: $SIMULATOR"; exit 2; }

# ==============
# Helper funcs
# ==============
natural_sort() { sort -V; }

list_types() {
  find "$NODES" -maxdepth 1 -type f -name "*$SUFFIX" -printf "%f\n" | sed "s/$SUFFIX$//" | natural_sort
}

extract_node_names() {
  # $1=nodes_file
  awk -v col="$NODES_NAME_COL" 'BEGIN{FS=OFS="\t"} NR==1{for(i=1;i<=NF;i++) if($i==col) c=i; next}
                                 {if(c) print $c}' "$1"
}

count_timepoints_from_real() {
  # $1=real_file -> number of timepoint columns (excludes REAL_NODE_COL)
  awk -v col="$REAL_NODE_COL" 'BEGIN{FS=OFS="\t"} NR==1{n=0; for(i=1;i<=NF;i++) if($i!=col) n++; print n; exit}' "$1"
}

join_by_comma() { awk -v v="$1" -v n="$2" 'BEGIN{for(i=1;i<=n;i++){printf "%s",v; if(i<n) printf ","} printf "\n"}'; }

write_uniform_params_for_type() {
  # $1=out_model_dir $2=type $3=value
  local dir="$1" typ="$2" val="$3"
  local nodes_file="$NODES/$typ$SUFFIX"
  local real_file="$REAL_DIR/$typ$SUFFIX"
  [[ -f "$nodes_file" ]] || { echo "[error] nodes missing: $nodes_file"; exit 2; }
  [[ -f "$real_file"  ]] || { echo "[error] real missing:  $real_file";  exit 2; }
  local L; L=$(count_timepoints_from_real "$real_file")
  [[ "$L" -gt 0 ]] || { echo "[error] no timepoints in $real_file"; exit 2; }
  mkdir -p "$dir"
  local line; line=$(join_by_comma "$val" "$L")
  {
    printf "name\tparameters\n"
    extract_node_names "$nodes_file" | while IFS= read -r nm; do
      printf "%s\t%s\n" "$nm" "$line"
    done
  } > "$dir/$typ$SUFFIX"
}