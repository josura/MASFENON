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


def align_frames(sim: pd.DataFrame, real: pd.DataFrame, keep_all: bool = False) -> Tuple[pd.DataFrame, pd.DataFrame, List[str], List[str]]:
    if keep_all:
        # Union, aligning with NaNs where missing
        all_cols = sorted(list(set(sim.columns) | set(real.columns)), key=natural_key)
        all_idx = sorted(list(set(sim.index) | set(real.index)))
        sim2 = sim.reindex(index=all_idx, columns=all_cols)
        real2 = real.reindex(index=all_idx, columns=all_cols)
        dropped_cols = []
        dropped_rows = []
    else:
        # Intersection only
        common_cols = sorted(list(set(sim.columns) & set(real.columns)), key=natural_key)
        common_idx = sorted(list(set(sim.index) & set(real.index)))
        dropped_cols = sorted(list((set(sim.columns) ^ set(real.columns))), key=natural_key)
        dropped_rows = sorted(list((set(sim.index) ^ set(real.index))))
        sim2 = sim.reindex(index=common_idx, columns=common_cols)
        real2 = real.reindex(index=common_idx, columns=common_cols)
    return sim2, real2, dropped_cols, dropped_rows


def make_error(sim_path: str, real_path: str, out_path: str, node_col: str, keep_all: bool) -> None:
    sim = read_tsv(sim_path, node_col)
    real = read_tsv(real_path, node_col)

    sim_al, real_al, dropped_cols, dropped_rows = align_frames(sim, real, keep_all=keep_all)

    if not keep_all:
        if dropped_cols:
            print(f"[warn] {os.path.basename(sim_path)}: dropped timepoints not common to both: {dropped_cols}", file=sys.stderr)
        if dropped_rows:
            print(f"[warn] {os.path.basename(sim_path)}: dropped nodes not common to both (|Δ|={len(dropped_rows)}).", file=sys.stderr)

    # Signed error: simulation - real
    err = sim_al.subtract(real_al)

    # Write TSV, keep index name as node_col
    err.index.name = sim.index.name  # should be node_col
    os.makedirs(os.path.dirname(out_path), exist_ok=True)
    err.to_csv(out_path, sep="\t", float_format="%.10g")  # avoid scientific unless needed
