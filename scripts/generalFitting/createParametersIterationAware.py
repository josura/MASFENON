#!/usr/bin/env python3
"""
Parameter update using finite-difference scaling:

    p_{t+1} = p_t - lr * e_t * (Δp / (Δs + eps))
    where Δp = p_t - p_{t-1}, and Δs = s_t - s_{t-1} = e_t - e_{t-1}

Inputs (per <type>.tsv):
  --nodes-dir        : TSVs with 'nodeNames' + timepoint columns (used for node/time axis)
  --params-dir       : CURRENT params p_t  (<type>.tsv with columns: name, parameters)
  --prev-params-dir  : PREVIOUS params p_{t-1}
  --errors-dir       : CURRENT error matrices e_t (<type>.tsv, index=nodeNames, columns=timepoints)
  --prev-errors-dir  : PREVIOUS error matrices e_{t-1}

Output:
  --out-dir          : Updated params p_{t+1} as <type>.tsv with columns: name, parameters

Notes:
- If any ingredient for a node/timepoint is missing, the script falls back safely:
  * Missing e_t -> no update (kept 0)
  * Missing e_{t-1} -> Δs = 0
  * Missing p_{t-1} -> Δp = 0
- Division is stabilized by --eps (default 1e-12). You can also clip the scale term via --max-scale.

Example:
  python createParametersIterationAware.py \
    --nodes-dir data/nodes \
    --params-dir data/params_t \
    --prev-params-dir data/params_t-1 \
    --errors-dir data/errors_t \
    --prev-errors-dir data/errors_t-1 \
    --out-dir data/params_t+1 \
    --nodes-name-col Name \
    --errors-name-col nodeNames \
    --lr 0.05
TODO there is a problem in case the previous parameter and the current parameter are the same, because then delta_p = 0 and no update is done. This can be fixed by adding a small random noise to the previous parameters when reading them.
TODO The problem if the errors are the same is less severe, because then delta_s = 0 and the update becomes p_{t+1} = p_t - lr * e_t * (delta_p / eps), which is still an update.
TODO In reality, the problem is a lot worse if the error is the same, while the parameters are different, because then delta_p / delta_s becomes very large and the update becomes very large. This cannot be fixed by clipping or adding eps. 
TODO These cases where the values are the same are coming from nodes that have no predecessors and probably other cases. we need to address this since it is directly affecting even the example to the general fitting model. 
"""

import argparse
import os
import sys
import re
from typing import Dict, List, Tuple
import pandas as pd
import numpy as np


def natural_key(s: str):
    return [int(t) if t.isdigit() else t.lower() for t in re.split(r"(\d+)", str(s))]


def list_types(nodes_dir: str, suffix: str) -> List[str]:
    return sorted(
        [f[:-len(suffix)] for f in os.listdir(nodes_dir)
         if f.endswith(suffix) and os.path.isfile(os.path.join(nodes_dir, f))],
        key=natural_key
    )


def read_node_names(path: str, nodes_name_col: str) -> pd.Index:
    df = pd.read_csv(path, sep="\t", dtype=str)
    if nodes_name_col not in df.columns:
        raise ValueError(f"{os.path.basename(path)} missing '{nodes_name_col}'. Found: {list(df.columns)}")
    return df[nodes_name_col].astype(str)


def read_errors(path: str, errors_name_col: str) -> pd.DataFrame:
    """Read an error matrix TSV; returns empty DataFrame if file missing."""
    if not os.path.isfile(path):
        return pd.DataFrame()
    df = pd.read_csv(path, sep="\t", dtype=str)
    if errors_name_col not in df.columns:
        raise ValueError(f"{os.path.basename(path)} (errors) missing '{errors_name_col}'.")
    df.set_index(errors_name_col, inplace=True)
    # coerce numeric timepoint columns
    for c in df.columns:
        df[c] = pd.to_numeric(df[c], errors="coerce")
    # natural sort timepoints
    df = df[sorted(df.columns, key=natural_key)]
    return df


def read_params_map(path: str) -> Dict[str, List[float]]:
    """Read name,parameters TSV -> dict[name] = list[float]."""
    if not os.path.isfile(path):
        return {}
    df = pd.read_csv(path, sep="\t", dtype=str)
    cols_lower = [c.lower() for c in df.columns]
    if "name" not in cols_lower or "parameters" not in cols_lower:
        raise ValueError(f"{os.path.basename(path)} must have columns 'name' and 'parameters'.")
    name_col = df.columns[cols_lower.index("name")]
    par_col  = df.columns[cols_lower.index("parameters")]
    out: Dict[str, List[float]] = {}
    for _, r in df.iterrows():
        name = str(r[name_col])
        raw = "" if pd.isna(r[par_col]) else str(r[par_col])
        vec = [float(x) for x in raw.split(",")] if raw.strip() else []
        out[name] = vec
    return out


def ensure_len(vec: List[float], L: int, fill: float) -> List[float]:
    if len(vec) == L:
        return vec
    if len(vec) > L:
        return vec[:L]
    return vec + [fill]*(L - len(vec))


def write_params(path: str, mapping: Dict[str, List[float]]):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    rows = [{"name": k, "parameters": ",".join(format(x, ".10g") for x in v)} for k, v in mapping.items()]
    pd.DataFrame(rows, columns=["name", "parameters"]).to_csv(path, sep="\t", index=False)


def main():
    ap = argparse.ArgumentParser(description="p_new = p_t - lr * e_t * (Δp / (Δs + eps)); nodes from nodes-dir, timepoints from errors.")
    ap.add_argument("--nodes-dir", required=True, help="Folder with <type>.tsv containing node column (default 'Name').")
    ap.add_argument("--params-dir", required=True, help="CURRENT params dir (<type>.tsv with columns: name, parameters).")
    ap.add_argument("--prev-params-dir", required=True, help="PREVIOUS params dir.")
    ap.add_argument("--errors-dir", required=True, help="CURRENT errors dir (<type>.tsv, index=<errors-name-col>, columns=timepoints).")
    ap.add_argument("--prev-errors-dir", required=True, help="PREVIOUS errors dir.")
    ap.add_argument("--out-dir", required=True, help="Output dir for updated params.")
    ap.add_argument("--out-dir-gradients", help="Optional output dir for gradients (for testing).")
    ap.add_argument("--suffix", default=".tsv")
    ap.add_argument("--nodes-name-col", default="Name", help="Node column in nodes-dir files (default: Name).")
    ap.add_argument("--errors-name-col", default="nodeNames", help="Node column in error files (default: nodeNames).")
    ap.add_argument("--init-value", type=float, default=0.0, help="Fill when a node lacks previous/current params.")
    ap.add_argument("--eps", type=float, default=1e-12, help="Small number added to Δs to avoid divide-by-zero.")
    ap.add_argument("--lr", type=float, default=0.1, help="Learning rate η.")
    ap.add_argument("--max-scale", type=float, default=None,
                    help="Optional clip for |Δp/(Δs+eps)| to avoid extreme updates (e.g., 1e3).")
    args = ap.parse_args()

    # Validate dirs
    for d in [args.nodes_dir, args.params_dir, args.prev_params_dir, args.errors_dir, args.prev_errors_dir]:
        if not os.path.isdir(d):
            print(f"[error] Not a directory: {d}", file=sys.stderr)
            sys.exit(2)

    types = list_types(args.nodes_dir, args.suffix)
    if not types:
        print("[error] No <type>.tsv in --nodes-dir.", file=sys.stderr)
        sys.exit(1)

    os.makedirs(args.out_dir, exist_ok=True)

    processed = 0
    for typ in types:
        # 1) Node list from nodes-dir
        nodes_path = os.path.join(args.nodes_dir, typ + args.suffix)
        try:
            nodes = read_node_names(nodes_path, args.nodes_name_col)  # <-- will fix var name below
        except Exception as e:
            print(f"[fail] {typ}: nodes read error: {e}", file=sys.stderr)
            continue
        nodes = nodes.astype(str)

        # 2) Read current & previous errors (timepoints from current errors)
        e_t_path   = os.path.join(args.errors_dir, typ + args.suffix)
        e_tm1_path = os.path.join(args.prev_errors_dir, typ + args.suffix)
        try:
            e_t_df = read_errors(e_t_path, args.errors_name_col)
        except Exception as e:
            print(f"[fail] {typ}: current errors read error: {e}", file=sys.stderr)
            continue
        if e_t_df.empty:
            print(f"[warn] {typ}: no current error file; skipping.", file=sys.stderr)
            continue

        # Timepoints from current errors
        time_cols = list(e_t_df.columns)
        # Reindex current errors to nodes/timepoints; fill missing with 0
        e_t_df = e_t_df.reindex(index=nodes, columns=time_cols).fillna(0.0)

        # Previous errors (optional but required by formula; if missing, treated as 0)
        try:
            e_tm1_df = read_errors(e_tm1_path, args.errors_name_col)
        except Exception as e:
            print(f"[warn] {typ}: prev errors read error: {e}; treating as zeros.", file=sys.stderr)
            e_tm1_df = pd.DataFrame()
        if e_tm1_df.empty:
            e_tm1_df = pd.DataFrame(0.0, index=nodes, columns=time_cols)
        else:
            e_tm1_df = e_tm1_df.reindex(index=nodes, columns=time_cols).fillna(0.0)

        # Δs
        delta_s = (e_t_df - e_tm1_df).to_numpy(dtype=float)

        # 3) Read current & previous params
        try:
            p_t_map   = read_params_map(os.path.join(args.params_dir, typ + args.suffix))
            p_tm1_map = read_params_map(os.path.join(args.prev_params_dir, typ + args.suffix))
        except Exception as e:
            print(f"[fail] {typ}: params read error: {e}", file=sys.stderr)
            continue

        L = len(time_cols)
        out_map: Dict[str, List[float]] = {}

        # For fast row access
        e_t_np = e_t_df.to_numpy(dtype=float)
        out_gradients_map: Dict[str, List[float]] = {}

        for i, name in enumerate(nodes):
            p_t   = np.asarray(ensure_len(p_t_map.get(name,   []), L, args.init_value), dtype=float)
            p_tm1 = np.asarray(ensure_len(p_tm1_map.get(name, []), L, args.init_value), dtype=float)

            delta_p = p_t - p_tm1
            denom   = delta_s[i, :] + args.eps
            # control if delta_s is zero and if delta_p is larger than zero, in that case set the scale to zero
            # this is to avoid extreme updates when delta_s is zero and delta_p is not zero
            if np.all(denom == args.eps) and np.any(delta_p != 0):
                scale = 0.0
            else:
                scale   = delta_p / denom
            if args.max_scale is not None:
                scale = np.clip(scale, -args.max_scale, args.max_scale)

            e_cur = e_t_np[i, :]
            # p_new = p_t - args.lr * e_cur * scale
            gradient = e_cur * scale
            # for TESTING, save the gradient in a folder, defaults to ${out_dir}_gradients
            p_new = p_tm1 - args.lr * e_cur * scale

            out_map[name] = p_new.tolist()
            out_gradients_map[name] = gradient.tolist()

        out_path = os.path.join(args.out_dir, typ + args.suffix)
        if args.out_dir_gradients:
            os.makedirs(args.out_dir_gradients, exist_ok=True)
            out_grad_path = os.path.join(args.out_dir_gradients, typ + args.suffix)
            try:
                write_params(out_grad_path, out_gradients_map)
                print(f"[ok] {typ}: wrote gradients {out_grad_path}")
            except Exception as e:
                print(f"[fail] {typ}: write gradients error: {e}", file=sys.stderr)
        try:
            write_params(out_path, out_map)
            processed += 1
            print(f"[ok] {typ}: wrote {out_path}")
        except Exception as e:
            print(f"[fail] {typ}: write error: {e}", file=sys.stderr)

    if processed == 0:
        print("[error] No files updated.", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()