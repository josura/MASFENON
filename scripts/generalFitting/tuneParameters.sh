#!/usr/bin/env python3
"""
MASFENON fitting driver.

Pipeline per your spec:
1) Inputs: epochs; graph/nodes/interactions folders; optional two initial parameter folders
   OR auto-generate 0.5/0.6 parameter sets; script paths; real data for error calc.
2) If no initial parameter folders are provided:
     - Generate two parameter sets (0.5) and (0.6) uniformly for ALL three models:
       propagation, dissipation, conservation.
3) Create fittingProcess/{epoch_1,...,epoch_N}/
   - each epoch folder contains:
       parameters/propagationParameters, dissipationParameters, conservationParameters
       errors/ (error matrices per type)
       sim_output/ (raw simulation output)
   - fittingProcess/RMSE.tsv aggregates RMSE per epoch (epoch \t RMSE)
4) At each epoch:
     - run simulation
     - compute error matrices via createErrorMatrix.py
     - compute RMSE via computeRMSE-fromErrorMatrix.py for each type, sum for epoch, append to RMSE.tsv
     - update parameters for next epoch via createParametersIterationAware.py
       (uses current + previous params and current + previous error matrices)
5) Repeat

Notes:
- We ONLY create parameters for timepoints that exist in REAL data (<type>.tsv columns except 'nodeNames').
- Requires Python scripts:
    * createErrorMatrix.py
    * createParametersIterationAware.py
    * computeRMSE-fromErrorMatrix.py
  Provide paths via CLI flags or keep them next to this driver.

Example:
  python tuneParameters.py \
    --epochs 10 \
    --graphs /path/graphs \
    --nodes /path/nodes \
    --interactions /path/interactions \
    --real-data-dir /path/real_series \
    --out /tmp/fitting_output \
    --simulator /path/to/masfenon_exe \
    --mpirun-np 8 \
    --mpirun-extra "--mca pml ob1 --mca btl tcp,self --mca btl_tcp_if_include wlan0" \
    --saturation --verbose

If you already have two initial parameter folders:
  --init-params-a /path/params_runA \
  --init-params-b /path/params_runB
"""