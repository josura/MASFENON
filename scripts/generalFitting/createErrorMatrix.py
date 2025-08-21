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



def find_matching_files(sim_dir: str, real_dir: str, suffix: str) -> List[Tuple[str, str, str]]:
    sim_files = {f for f in os.listdir(sim_dir) if f.endswith(suffix) and os.path.isfile(os.path.join(sim_dir, f))}
    real_files = {f for f in os.listdir(real_dir) if f.endswith(suffix) and os.path.isfile(os.path.join(real_dir, f))}
    both = sorted(sim_files & real_files, key=natural_key)
    missing_in_real = sorted(sim_files - real_files, key=natural_key)
    missing_in_sim = sorted(real_files - sim_files, key=natural_key)

    if missing_in_real:
        print(f"[warn] Files present in sim but missing in real ({len(missing_in_real)}): {missing_in_real}", file=sys.stderr)
    if missing_in_sim:
        print(f"[warn] Files present in real but missing in sim ({len(missing_in_sim)}): {missing_in_sim}", file=sys.stderr)

    pairs = [(os.path.join(sim_dir, f), os.path.join(real_dir, f), f) for f in both]
    return pairs


def main():
    p = argparse.ArgumentParser(description="Create signed error matrices (simulation - real) from MASFENON-style TSVs.")
    p.add_argument("--sim-dir", required=True, help="Directory with simulation TSVs (e.g., t0.tsv, t1.tsv, ...)")
    p.add_argument("--real-dir", required=True, help="Directory with real data TSVs (matching filenames).")
    p.add_argument("--out-dir", required=True, help="Directory to write error TSVs.")
    p.add_argument("--prefix", default="", help="Optional filename prefix for outputs (default: '').")
    p.add_argument("--pattern", default=".tsv", help="Process files ending with this suffix (default: .tsv).")
    p.add_argument("--node-col", default="nodeNames", help="Name of node column (default: nodeNames).")
    p.add_argument("--keep-all", action="store_true",
                   help="Use union of nodes/timepoints (keeps NaNs where missing). Default: intersection only.")
    args = p.parse_args()

    for d in (args.sim_dir, args.real_dir):
        if not os.path.isdir(d):
            print(f"[error] Not a directory: {d}", file=sys.stderr)
            sys.exit(2)

    pairs = find_matching_files(args.sim_dir, args.real_dir, args.pattern)
    if not pairs:
        print("[error] No matching files to process.", file=sys.stderr)
        sys.exit(1)

    os.makedirs(args.out_dir, exist_ok=True)

    processed = 0
    for sim_path, real_path, fname in pairs:
        out_name = args.prefix + fname
        out_path = os.path.join(args.out_dir, out_name)
        try:
            make_error(sim_path, real_path, out_path, node_col=args.node_col, keep_all=args.keep_all)
            processed += 1
            print(f"[ok] Wrote {out_path}")
        except Exception as e:
            print(f"[fail] {fname}: {e}", file=sys.stderr)

    if processed == 0:
        print("[error] No files were successfully processed.", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
