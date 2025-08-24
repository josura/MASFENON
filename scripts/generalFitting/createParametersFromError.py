#!/usr/bin/env python3
"""
Update (or initialize) per-node parameter vectors using signed error matrices.

Inputs (by type, meaning files named <type>.tsv):
    --nodes-dir:   TSVs with at least "nodeNames" plus timepoint columns (t0,t1,...).
    --params-dir:  <optional> TSVs with columns: name, parameters
                    where "parameters" is a comma-separated list of floats.
    --errors-dir:  <optional> TSVs produced by the error-matrix script, same shape
                    as nodes (index: nodeNames, columns: timepoints).

Output:
    --out-dir:  For each <type> present in --nodes-dir, writes <type>.tsv
                with columns: name, parameters (comma-separated floats).

Behavior:
    * If --params-dir is provided and has <type>.tsv, those are the starting params.
        Missing nodes get initialized.
    * If --params-dir is omitted (or missing for a type), parameters are initialized.
    * If --errors-dir is provided and has <type>.tsv, parameters are updated via:
        new_params = old_params - lr * error
        per timepoint (NaN errors treated as 0). If missing, no update is applied.
    * Parameter vector length is matched to the number of timepoints in nodes/errors.
        If old length differs: truncate or pad with --init-value.
    * Final files include only nodes listed in the <type>.tsv from --nodes-dir.

Usage example:
    python update_params.py \
        --nodes-dir data/nodes \
        --params-dir data/old_params \
        --errors-dir data/errors \
        --out-dir data/new_params \
        --lr 0.05

Initialization-only (no prior params, no errors):
    python update_params.py \
        --nodes-dir data/nodes \
        --out-dir data/new_params \
        --num-params 10 \
        --init-value 0.0
"""

import argparse
import os
import sys
from typing import Dict, List, Tuple
import pandas as pd
import numpy as np
import re

def natural_key(s: str):
    """Sort strings with embedded numbers (e.g., t2 < t10)."""
    return [int(t) if t.isdigit() else t.lower() for t in re.split(r"(\d+)", str(s))]

def read_nodes(path: str, node_col: str) -> Tuple[pd.Index, List[str]]:
    df = pd.read_csv(path, sep="\t", dtype=str)
    if node_col not in df.columns:
        raise ValueError(f"{os.path.basename(path)} missing '{node_col}' column.")
    # timepoint columns are everything except node_col
    time_cols = [c for c in df.columns if c != node_col]
    time_cols = sorted(time_cols, key=natural_key)
    nodes = df[node_col].astype(str)
    return nodes, time_cols
