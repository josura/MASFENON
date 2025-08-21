#!/usr/bin/env python3
"""
Create signed error matrices (simulation - real) for MASFENON-style time series.

Input folder structure (both sim and real):
- Files named like `t0.tsv`, `t1.tsv`, ... (one file per agent/type).
- Each TSV has:
    - First column: "nodeNames"
    - Remaining columns: timepoints (e.g., 0,1,2 or t0,t1,...). Values are numeric.
- Rows: nodeName, value_at_timepoint_1, value_at_timepoint_2, ...

Output:
- For every file present in BOTH folders, produces an error matrix TSV in --out-dir
  with the same filename (or prefixed via --prefix).
- Error is computed as (simulation - real), preserving sign.

Mismatches:
- Nodes and timepoints are aligned by intersection.
- Missing nodes/timepoints in either input lead to dropped columns/rows (intersection only).
- If any column or node is dropped, a warning is printed.
- Non-numeric entries are coerced to NaN.

Usage:
    python make_error_matrices.py \
        --sim-dir path/to/sim \
        --real-dir path/to/real \
        --out-dir path/to/errors

Optional:
    --prefix ERR_                # filename prefix for outputs (default: "")
    --node-col nodeNames         # name of node column (default: nodeNames)
    --pattern .tsv               # only process files ending with this suffix (default: .tsv)
    --keep-all                   # keep union of timepoints/nodes; NaNs where missing (default: intersection)
"""

import argparse
import os
import sys
from typing import List, Tuple
import pandas as pd
import re


def natural_key(s: str):
    #Sort strings by embedded numbers: 't2' < 't10'.
    return [int(text) if text.isdigit() else text.lower() for text in re.split(r"(\d+)", str(s))]


def read_tsv(path: str, node_col: str) -> pd.DataFrame:
    df = pd.read_csv(path, sep="\t", dtype=str)  # read as str first to preserve headers faithfully
    if node_col not in df.columns:
        raise ValueError(f"{os.path.basename(path)} is missing required column '{node_col}'. Found: {list(df.columns)}")
    # Coerce numeric timepoint cols; keep node_col as index
    time_cols = [c for c in df.columns if c != node_col]
    # Try to convert; non-numeric becomes NaN
    for c in time_cols:
        df[c] = pd.to_numeric(df[c], errors="coerce")
    df.set_index(node_col, inplace=True)
    # Sort timepoints naturally for stability
    df = df[sorted(time_cols, key=natural_key)]
    return df

