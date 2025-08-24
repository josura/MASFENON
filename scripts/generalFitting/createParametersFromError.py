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
    python createParametersFromError.py \
        --nodes-dir data/nodes \
        --params-dir data/old_params \
        --errors-dir data/errors \
        --out-dir data/new_params \
        --lr 0.05

Initialization-only (no prior params, no errors):
    python createParametersFromError.py \
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

def read_errors(path: str, node_col: str) -> pd.DataFrame:
    df = pd.read_csv(path, sep="\t", dtype=str)
    if node_col not in df.columns:
        raise ValueError(f"{os.path.basename(path)} (errors) missing '{node_col}'.")
    df.set_index(node_col, inplace=True)
    # Ensure numeric
    for c in df.columns:
        df[c] = pd.to_numeric(df[c], errors="coerce")
    # Natural sort columns
    df = df[sorted(df.columns, key=natural_key)]
    return df

def read_params(path: str) -> Dict[str, List[float]]:
    """Read name,parameters TSV -> dict[name] = list[float]."""
    df = pd.read_csv(path, sep="\t", dtype=str)
    cols = [c.lower() for c in df.columns]
    try:
        name_col = df.columns[cols.index("name")]
        params_col = df.columns[cols.index("parameters")]
    except ValueError:
        raise ValueError(f"{os.path.basename(path)} must have columns 'name' and 'parameters'.")
    result = {}
    for _, row in df.iterrows():
        name = str(row[name_col])
        raw = str(row[params_col]) if pd.notna(row[params_col]) else ""
        if raw.strip() == "":
            result[name] = []
            continue
        try:
            vec = [float(x) for x in raw.split(",")]
        except Exception:
            raise ValueError(f"Bad parameters list for '{name}' in {os.path.basename(path)}: {raw!r}")
        result[name] = vec
    return result

def write_params(path: str, mapping: Dict[str, List[float]]):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    rows = []
    for name, vec in mapping.items():
        param_str = ",".join(format(x, ".10g") for x in vec)
        rows.append({"name": name, "parameters": param_str})
    out_df = pd.DataFrame(rows, columns=["name", "parameters"])
    out_df.to_csv(path, sep="\t", index=False)


def ensure_length(vec: List[float], length: int, fill: float) -> List[float]:
    if len(vec) == length:
        return vec
    if len(vec) > length:
        return vec[:length]
    return vec + [fill] * (length - len(vec))


def get_types(nodes_dir: str, suffix: str) -> List[str]:
    return sorted(
        [f[:-len(suffix)] for f in os.listdir(nodes_dir)
         if f.endswith(suffix) and os.path.isfile(os.path.join(nodes_dir, f))],
        key=natural_key
    )

def main():
    ap = argparse.ArgumentParser(description="Update/init per-node parameters using error matrices (gradient-descent-like).")
    ap.add_argument("--nodes-dir", required=True, help="Directory with <type>.tsv files containing 'nodeNames' and timepoint columns.")
    ap.add_argument("--params-dir", help="Directory with old <type>.tsv (columns: name, parameters).")
    ap.add_argument("--errors-dir", help="Directory with error matrices <type>.tsv (index: nodeNames; columns: timepoints).")
    ap.add_argument("--out-dir", required=True, help="Output directory for updated <type>.tsv (columns: name, parameters).")
    ap.add_argument("--node-col", default="nodeNames", help="Name of the node column in nodes and error files. Default: nodeNames")
    ap.add_argument("--suffix", default=".tsv", help="Suffix for all per-type files. Default: .tsv")
    ap.add_argument("--lr", type=float, default=0.1, help="Learning rate for parameter update. Default: 0.1")
    ap.add_argument("--num-params", type=int, help="Used ONLY when initializing without old params; if omitted, uses #timepoints.")
    ap.add_argument("--init-value", type=float, default=0.0, help="Initial value for parameters and padding. Default: 0.0")
    args = ap.parse_args()
    # Validate dirs
    if not os.path.isdir(args.nodes_dir):
        print(f"[error] --nodes-dir is not a directory: {args.nodes_dir}", file=sys.stderr)
        sys.exit(2)
    if args.params_dir and not os.path.isdir(args.params_dir):
        print(f"[warn] --params-dir not found, will initialize parameters: {args.params_dir}", file=sys.stderr)
        args.params_dir = None
    if args.errors_dir and not os.path.isdir(args.errors_dir):
        print(f"[warn] --errors-dir not found, no updates will be applied: {args.errors_dir}", file=sys.stderr)
        args.errors_dir = None

    types = get_types(args.nodes_dir, args.suffix)
    if not types:
        print("[error] No <type>.tsv files found in --nodes-dir.", file=sys.stderr)
        sys.exit(1)

    os.makedirs(args.out_dir, exist_ok=True)

    processed = 0
    for typ in types:
        nodes_path = os.path.join(args.nodes_dir, typ + args.suffix)
        try:
            node_list, time_cols = read_nodes(nodes_path, args.node_col)
        except Exception as e:
            print(f"[fail] {typ}: cannot read nodes file: {e}", file=sys.stderr)
            continue

        # Determine time axis / length
        time_len = len(time_cols)

        # Errors (optional)
        err_df = None
        if args.errors_dir:
            err_path = os.path.join(args.errors_dir, typ + args.suffix)
            if os.path.isfile(err_path):
                try:
                    err_df = read_errors(err_path, args.node_col)
                    # Align error columns to node time columns (intersection, ordered)
                    common_cols = [c for c in time_cols if c in err_df.columns]
                    if not common_cols:
                        print(f"[warn] {typ}: no overlapping timepoints between nodes and errors; skipping updates.", file=sys.stderr)
                        err_df = None
                    else:
                        if len(common_cols) != len(time_cols):
                            missing = [c for c in time_cols if c not in err_df.columns]
                            if missing:
                                print(f"[warn] {typ}: missing error for timepoints {missing}; treating as 0.", file=sys.stderr)
                        # Reindex errors to nodes/timepoints; fill missing with 0
                        err_df = err_df.reindex(index=node_list.astype(str), columns=time_cols)
                        err_df = err_df.fillna(0.0)
                except Exception as e:
                    print(f"[warn] {typ}: cannot read/align errors: {e}; skipping updates.", file=sys.stderr)
                    err_df = None
            else:
                print(f"[warn] {typ}: no error file found; skipping updates.", file=sys.stderr)

        # Old params (optional)
        old_params_map: Dict[str, List[float]] = {}
        if args.params_dir:
            param_path = os.path.join(args.params_dir, typ + args.suffix)
            if os.path.isfile(param_path):
                try:
                    old_params_map = read_params(param_path)
                except Exception as e:
                    print(f"[warn] {typ}: cannot read old params: {e}; initializing.", file=sys.stderr)
                    old_params_map = {}
            else:
                print(f"[warn] {typ}: no old params file found; initializing.", file=sys.stderr)

        # Decide length for initialization
        init_len = args.num_params if args.num_params is not None else time_len
        if init_len <= 0:
            print(f"[fail] {typ}: invalid parameter length (computed {init_len}).", file=sys.stderr)
            continue

        new_params_map: Dict[str, List[float]] = {}
        for name in node_list.astype(str):
            # get existing vector or initialize
            vec = old_params_map.get(name, None)
            if vec is None or len(vec) == 0:
                vec = [args.init_value] * init_len
            # match to time axis length
            target_len = time_len if time_len > 0 else len(vec)
            if args.num_params is not None:
                # If user forced num-params, honor it when no timepoints in nodes
                target_len = time_len if time_len > 0 else args.num_params
            vec = ensure_length(vec, target_len, args.init_value)

            # apply update if errors available
            if err_df is not None and target_len == len(time_cols):
                # errors row for this node (already aligned/fillna=0)
                e = err_df.loc[name].to_numpy(dtype=float, copy=False)
                # gradient-descent-like step
                vec = (np.asarray(vec, dtype=float) - args.lr * e).tolist()

            new_params_map[name] = vec

        out_path = os.path.join(args.out_dir, typ + args.suffix)
        try:
            write_params(out_path, new_params_map)
            processed += 1
            print(f"[ok] Wrote {out_path}")
        except Exception as e:
            print(f"[fail] {typ}: cannot write output: {e}", file=sys.stderr)

    if processed == 0:
        print("[error] No files were successfully processed.", file=sys.stderr)
        sys.exit(1)




if __name__ == "__main__":
    main()