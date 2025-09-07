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

# Helper python scripts (relative or full paths)
SCRIPT_ERROR="createErrorMatrix.py"
SCRIPT_PARAMS="createParametersIterationAware.py"
SCRIPT_RMSE="computeRMSE-fromErrorMatrix.py"

# Parameter update settings
LR=0.1
EPS=1e-12
MAX_SCALE=""      # e.g., 1000 to clip |Δp/(Δs+eps)|; leave empty to disable

# Data conventions
SUFFIX=".tsv"
NODES_NAME_COL="Name"      # column in --nodes files
REAL_NODE_COL="nodeNames"  # row index column in real data & error matrices

# Defaults for auto-generated initial params (when A/B not supplied)
DEFAULT_A=0.5
DEFAULT_B=0.6

# ===========================
#  CLI (only IO & epochs)
# ===========================
usage() {
  cat <<EOF
Usage:
  $(basename "$0") --epochs N \\
    --graphs /path/graphs \\
    --nodes  /path/nodes \\
    --interactions /path/interactions \\
    --real-data-dir /path/real_series \\
    --out /path/output \\
    [--init-params-a /path/paramsA --init-params-b /path/paramsB]

Notes:
- Only IO and epoch count are CLI-configurable; edit constants at the top for everything else.
- Params folders must contain subdirs:
    propagationParameters/ dissipationParameters/ conservationParameters/
EOF
}

EPOCHS="" GRAPHS="" NODES="" INTERACTIONS="" REAL_DIR="" OUT_ROOT=""
INIT_A="" INIT_B=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --epochs) EPOCHS="$2"; shift 2;;
    --graphs) GRAPHS="$2"; shift 2;;
    --nodes) NODES="$2"; shift 2;;
    --interactions) INTERACTIONS="$2"; shift 2;;
    --real-data-dir) REAL_DIR="$2"; shift 2;;
    --out) OUT_ROOT="$2"; shift 2;;
    --init-params-a) INIT_A="$2"; shift 2;;
    --init-params-b) INIT_B="$2"; shift 2;;
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
echo "Interactions: $INTERACTIONS"
echo "Real data: $REAL_DIR"
echo "Output root: $OUT_ROOT"
if [[ -n "$INIT_A" && -n "$INIT_B" ]]; then
  echo "Init params A: $INIT_A"
  echo "Init params B: $INIT_B"
else
  echo "Init params A: (auto-generated)"
  echo "Init params B: (auto-generated)"
fi
echo "-----------------------------"


# Validate the necessary options
for v in EPOCHS GRAPHS NODES INTERACTIONS REAL_DIR OUT_ROOT; do
  [[ -n "${!v}" ]] || { echo "[error] missing --${v,,}"; usage; exit 2; }
done
for d in "$GRAPHS" "$NODES" "$INTERACTIONS" "$REAL_DIR"; do
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

generate_uniform_param_set() {
  # $1=out_root $2=value
  local out="$1" val="$2"
  mkdir -p "$out"/{propagationParameters,dissipationParameters,conservationParameters}
  list_types | while read -r typ; do
    write_uniform_params_for_type "$out/propagationParameters" "$typ" "$val"
    write_uniform_params_for_type "$out/dissipationParameters"  "$typ" "$val"
    write_uniform_params_for_type "$out/conservationParameters" "$typ" "$val"
  done
}
