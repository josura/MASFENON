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
# TODO fix fitting process to consider partial derivatives instead of what is doing at the moment since it's not really using partial dervivatives
# TODO also try to change the initial way of initializing the parameters, especially because prelim_A parameters are used as the starting point, while prelim_B is used to control the perturbation, that is on singular mechanics(it's not a good way of encoding these kind of fitting model at the moment)
# -----------------------------


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
    --nodes  /path/node_descriptions \\
    --initial /path/initial_values \\
    --interactions /path/interactions \\
    --real-data-dir /path/real_series \\
    --out /path/output \\
    [--init-params-a /path/paramsA --init-params-b /path/paramsB]

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
INIT_A="" INIT_B=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --epochs) EPOCHS="$2"; shift 2;;
    --graphs) GRAPHS="$2"; shift 2;;
    --nodes) NODES="$2"; shift 2;;          # node descriptions (no values)
    --initial) INITIAL="$2"; shift 2;;      # initial node values for the sim
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
echo "Initial values: $INITIAL"
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

copy_params_tree() { rm -rf "$2"; mkdir -p "$2"; cp -r "$1"/. "$2"/; }

sim_cmd() {
  # $1=params_root  $2=sim_out_dir
  local p="$1" out="$2"
  # make the output directory if it does not exist, to avoid simulator errors
  mkdir -p "$out"
  # also make the iterationMatrices subdir to avoid simulator errors
  mkdir -p "$out/iterationMatrices"
  local cmd=()
  cmd+=("$MPIRUN_BIN" -np "$MPIRUN_NP")
  [[ -n "$MPIRUN_EXTRA" ]] && cmd+=($MPIRUN_EXTRA)
  cmd+=("$SIMULATOR"
        --graphsFilesFolder "$GRAPHS"
        --nodeDescriptionFolder "$NODES"                        # nodes (no values)
        --initialPerturbationPerTypeFolder "$INITIAL"           # initial values
        --typeInteractionFolder "$INTERACTIONS"
        --propagationModel customPropagation
        --propagationModelParameterFolder "$p/propagationParameters"
        --dissipationModel custom
        --dissipationModelParameterFolder "$p/dissipationParameters"
        --conservationModel custom
        --conservationModelParameterFolder "$p/conservationParameters"
        --virtualNodesGranularity "$VNG"
        --outputFormat "$OUTPUT_FORMAT"
        --outputFolder "$out")
  [[ "$USE_SATURATION" -eq 1 ]] && cmd+=(--saturation)
  [[ "$USE_VERBOSE"    -eq 1 ]] && cmd+=(--verbose)
  printf '%q ' "${cmd[@]}"
}

run_sim_and_errors() {
  # $1=tag  $2=params_root -> sets globals: EPOCH_DIR, ERR_DIR
  local tag="$1" params="$2"
  EPOCH_DIR="$FITTING_ROOT/$tag"
  mkdir -p "$EPOCH_DIR"
  local SIM_OUT="$EPOCH_DIR/sim_output"; mkdir -p "$SIM_OUT"
  # sim
  echo "[cmd] $(sim_cmd "$params" "$SIM_OUT")"
  eval "$(sim_cmd "$params" "$SIM_OUT")"
  local SIM_OUT_IterationMatrix="$SIM_OUT/iterationMatrices"
  [[ -d "$SIM_OUT_IterationMatrix" ]] || { echo "[error] sim output iteration matrices missing"; exit 2; }
  # errors
  ERR_DIR="$EPOCH_DIR/errors"; mkdir -p "$ERR_DIR"
  python3 "$SCRIPT_ERROR" --sim-dir "$SIM_OUT_IterationMatrix" --real-dir "$REAL_DIR" --out-dir "$ERR_DIR"
  # python3 "$SCRIPT_ERROR" --sim-dir "$SIM_OUT" --real-dir "$REAL_DIR" --out-dir "$ERR_DIR"
}

sum_rmse_in_folder() {
  # $1=errors_dir -> echo sum
  local d="$1"
  find "$d" -maxdepth 1 -type f -name "*$SUFFIX" -print0 \
    | xargs -0 -I{} python3 "$SCRIPT_RMSE" --file "{}" --node-col "$REAL_NODE_COL" \
    | awk '{s+=$1} END{printf "%.10g\n", s+0}'
}

# ==============
# Prep output
# ==============
mkdir -p "$OUT_ROOT"
FITTING_ROOT="${OUT_ROOT%/}/fittingProcess"
mkdir -p "$FITTING_ROOT"
RMSE_TSV="$FITTING_ROOT/RMSE.tsv"

# ==============
# Initial params
# ==============
INIT_A_DIR="$FITTING_ROOT/init_A"
INIT_B_DIR="$FITTING_ROOT/init_B"
if [[ -n "$INIT_A" || -n "$INIT_B" ]]; then
  [[ -n "$INIT_A" && -n "$INIT_B" ]] || { echo "[error] provide both --init-params-a and --init-params-b"; exit 2; }
  copy_params_tree "$INIT_A" "$INIT_A_DIR"
  copy_params_tree "$INIT_B" "$INIT_B_DIR"
else
  echo "[info] Auto-generating initial parameter sets A=$DEFAULT_A, B=$DEFAULT_B"
  generate_uniform_param_set "$INIT_A_DIR" "$DEFAULT_A"
  generate_uniform_param_set "$INIT_B_DIR" "$DEFAULT_B"
fi

# ====================
# Preliminary runs A/B
# ====================
echo "[info] Running preliminary simulations..."
echo "[info]   first simulation with initial params A, only one since it is simulating some original parameters already known"
run_sim_and_errors "prelim_A" "$INIT_A_DIR"; PREV_PARAMS="$INIT_A_DIR"; PREV_ERRORS="$ERR_DIR"


echo "[info]   second simulation with initial params B, selecting one parameter mechanics at a time, this is because we need to see the individual changes in the simulation"
echo "[info] simulation A: dissipation only change"
mkdir -p "${OUT_ROOT}/prelim_B" #general folder that contains the different experiment parameters change
# dissipation perturbed only
mkdir -p "${OUT_ROOT}/prelim_B/experiment_dissipation"  #folder containing the only dissipation perturbed parameters, copied from INIT_B, the other parameters are copied from INIT_A
cp -r $INIT_A_DIR/dissipationParameters ${OUT_ROOT}/prelim_B/experiment_dissipation
cp -r $INIT_B_DIR/conservationParameters ${OUT_ROOT}/prelim_B/experiment_dissipation
cp -r $INIT_A_DIR/propagationParameters ${OUT_ROOT}/prelim_B/experiment_dissipation
# propagation perturbed only
mkdir -p "${OUT_ROOT}/prelim_B/experiment_propagation"  #folder containing the only propagation perturbed parameters, copied from INIT_B, the other parameters are copied from INIT_A
cp -r $INIT_A_DIR/propagationParameters ${OUT_ROOT}/prelim_B/experiment_propagation
cp -r $INIT_B_DIR/conservationParameters ${OUT_ROOT}/prelim_B/experiment_propagation
cp -r $INIT_A_DIR/dissipationParameters ${OUT_ROOT}/prelim_B/experiment_propagation
# conservation perturbed only
mkdir -p "${OUT_ROOT}/prelim_B/experiment_conservation"  #folder containing the only conservation perturbed parameters, copied from INIT_B, the other parameters are copied from INIT_A
cp -r $INIT_A_DIR/conservationParameters ${OUT_ROOT}/prelim_B/experiment_conservation
cp -r $INIT_B_DIR/dissipationParameters ${OUT_ROOT}/prelim_B/experiment_conservation
cp -r $INIT_A_DIR/propagationParameters ${OUT_ROOT}/prelim_B/experiment_conservation



# RMSE header
printf "epoch\tRMSE\n" > "$RMSE_TSV"



# TO TEST
# ===========
# Epoch loop
# ===========

# for ((epoch=1; epoch<=EPOCHS; epoch++)); do
#   echo "[info] Epoch $epoch"
#   epoch_dir="$FITTING_ROOT/epoch_$epoch"
#   mkdir -p "$epoch_dir"
#   NEXT_PARAMS="$epoch_dir/parameters"; mkdir -p "$NEXT_PARAMS"

#   # Create next params (Δp/(Δs+eps) rule), for dissipation, conservation and propagation
#   echo "[info] Creating next parameters set"
#   mechanism=(propagation dissipation conservation)
#   for mech in "${mechanismFolders[@]}"; do
#     mkdir -p "$NEXT_PARAMS/$mech"
#     cmd=(python3 "$SCRIPT_PARAMS"
#         --nodes-dir "$NODES"
#         --params-dir "$CURR_PARAMS/${mech}Parameters"
#         --prev-params-dir "$PREV_PARAMS/${mech}Parameters"
#         --errors-dir "$CURR_ERRORS"
#         --prev-errors-dir "$PREV_ERRORS"
#         --out-dir "$NEXT_PARAMS/$mech"
#         --nodes-name-col "$NODES_NAME_COL"
#         --errors-name-col "$REAL_NODE_COL"
#         --suffix "$SUFFIX"
#         --lr "$LR"
#         --eps "$EPS")
#     [[ -n "$MAX_SCALE" ]] && cmd+=(--max-scale "$MAX_SCALE")
#     echo "[cmd] ${cmd[*]}"
#     "${cmd[@]}"
#   done
#   # mechanismFolders=(propagationParameters dissipationParameters conservationParameters)
#   # for mech in "${mechanismFolders[@]}"; do
#   #   mkdir -p "$NEXT_PARAMS/$mech"
#   #   cmd=(python3 "$SCRIPT_PARAMS"
#   #       --nodes-dir "$NODES"
#   #       --params-dir "$CURR_PARAMS/$mech"
#   #       --prev-params-dir "$PREV_PARAMS/$mech"
#   #       --errors-dir "$CURR_ERRORS"
#   #       --prev-errors-dir "$PREV_ERRORS"
#   #       --out-dir "$NEXT_PARAMS/$mech"
#   #       --nodes-name-col "$NODES_NAME_COL"
#   #       --errors-name-col "$REAL_NODE_COL"
#   #       --suffix "$SUFFIX"
#   #       --lr "$LR"
#   #       --eps "$EPS")
#   #   [[ -n "$MAX_SCALE" ]] && cmd+=(--max-scale "$MAX_SCALE")
#   #   echo "[cmd] ${cmd[*]}"
#   #   "${cmd[@]}"
#   # done
#   # cmd=(python3 "$SCRIPT_PARAMS"
#   #      --nodes-dir "$NODES"
#   #      --params-dir "$CURR_PARAMS"
#   #      --prev-params-dir "$PREV_PARAMS"
#   #      --errors-dir "$CURR_ERRORS"
#   #      --prev-errors-dir "$PREV_ERRORS"
#   #      --out-dir "$NEXT_PARAMS"
#   #      --nodes-name-col "$NODES_NAME_COL"
#   #      --errors-name-col "$REAL_NODE_COL"
#   #      --suffix "$SUFFIX"
#   #      --lr "$LR"
#   #      --eps "$EPS")
#   # [[ -n "$MAX_SCALE" ]] && cmd+=(--max-scale "$MAX_SCALE")
#   # echo "[cmd] ${cmd[*]}"
#   # "${cmd[@]}"

#   # Sim + errors for this epoch
#   echo "[info] Running simulation with updated parameters for epoch $epoch"
#   run_sim_and_errors "epoch_$epoch" "$NEXT_PARAMS"

#   # Sum RMSE across types
#   RMSE_SUM=$(sum_rmse_in_folder "$ERR_DIR")
#   printf "%d\t%s\n" "$epoch" "$RMSE_SUM" >> "$RMSE_TSV"
#   echo "[info] Epoch $epoch RMSE_sum = $RMSE_SUM"

#   # Slide windows
#   PREV_PARAMS="$CURR_PARAMS"; PREV_ERRORS="$CURR_ERRORS"
#   CURR_PARAMS="$NEXT_PARAMS";  CURR_ERRORS="$ERR_DIR"
# done

# echo "[done] Fitting finished."
# echo "      Results at: $FITTING_ROOT"
# echo "      RMSE per epoch: $RMSE_TSV"