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
  python update_params_fdscale.py \
    --nodes-dir data/nodes \
    --params-dir data/params_t \
    --prev-params-dir data/params_t-1 \
    --errors-dir data/errors_t \
    --prev-errors-dir data/errors_t-1 \
    --out-dir data/params_t+1 \
    --lr 0.05
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


def read_nodes(path: str, node_col: str) -> Tuple[pd.Index, List[str]]:
    df = pd.read_csv(path, sep="\t", dtype=str)
    if node_col not in df.columns:
        raise ValueError(f"{os.path.basename(path)} missing '{node_col}'.")
    time_cols = sorted([c for c in df.columns if c != node_col], key=natural_key)
    return df[node_col].astype(str), time_cols


def read_errors(path: str, node_col: str, time_cols: List[str], nodes: pd.Index) -> pd.DataFrame:
    """Read errors; align to nodes/time_cols; missing -> 0.0"""
    if not os.path.isfile(path):
        # return all-zeros aligned frame
        return pd.DataFrame(0.0, index=nodes, columns=time_cols)
    df = pd.read_csv(path, sep="\t", dtype=str)
    if node_col not in df.columns:
        raise ValueError(f"{os.path.basename(path)} (errors) missing '{node_col}'.")
    df.set_index(node_col, inplace=True)
    for c in df.columns:
        df[c] = pd.to_numeric(df[c], errors="coerce")
    df = df.reindex(index=nodes, columns=time_cols)
    return df.fillna(0.0)


def read_params_map(path: str) -> Dict[str, List[float]]:
    """Read name,parameters TSV -> dict[name] = list[float]."""
    if not os.path.isfile(path):
        return {}
    df = pd.read_csv(path, sep="\t", dtype=str)
    cols = [c.lower() for c in df.columns]
    if "name" not in cols or "parameters" not in cols:
        raise ValueError(f"{os.path.basename(path)} must have columns 'name' and 'parameters'.")
    name_col = df.columns[cols.index("name")]
    par_col  = df.columns[cols.index("parameters")]
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


def list_types(nodes_dir: str, suffix: str) -> List[str]:
    return sorted(
        [f[:-len(suffix)] for f in os.listdir(nodes_dir)
         if f.endswith(suffix) and os.path.isfile(os.path.join(nodes_dir, f))],
        key=natural_key
    )


def main():
    ap = argparse.ArgumentParser(description="Parameter update: p_new = p - lr * e_t * (Δp / (Δs + eps)).")
    ap.add_argument("--nodes-dir", required=True, help="Folder with <type>.tsv having 'nodeNames' + timepoints.")
    ap.add_argument("--params-dir", required=True, help="CURRENT params dir (<type>.tsv with columns: name, parameters).")
    ap.add_argument("--prev-params-dir", required=True, help="PREVIOUS params dir.")
    ap.add_argument("--errors-dir", required=True, help="CURRENT errors dir (<type>.tsv, index=nodeNames, columns=timepoints).")
    ap.add_argument("--prev-errors-dir", required=True, help="PREVIOUS errors dir.")
    ap.add_argument("--out-dir", required=True, help="Output dir for updated params.")
    ap.add_argument("--node-col", default="nodeNames")
    ap.add_argument("--suffix", default=".tsv")
    ap.add_argument("--init-value", type=float, default=0.0, help="Fill when a node lacks previous params.")
    ap.add_argument("--eps", type=float, default=1e-12, help="Small number added to Δs to avoid divide-by-zero.")
    ap.add_argument("--lr", type=float, default=0.1, help="Learning rate η.")
    ap.add_argument("--max-scale", type=float, default=None,
                    help="Optional clip for |Δp/(Δs+eps)| to avoid extreme updates (set e.g. 1e3).")
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
        nodes_path = os.path.join(args.nodes_dir, typ + args.suffix)
        try:
            node_list, time_cols = read_nodes(nodes_path, args.node_col)
        except Exception as e:
            print(f"[fail] {typ}: nodes read error: {e}", file=sys.stderr)
            continue

        L = len(time_cols)
        if L == 0:
            print(f"[warn] {typ}: no timepoints; skipping.", file=sys.stderr)
            continue

        # Errors: current and previous
        try:
            e_t   = read_errors(os.path.join(args.errors_dir, typ + args.suffix), args.node_col, time_cols, node_list)
            e_tm1 = read_errors(os.path.join(args.prev_errors_dir, typ + args.suffix), args.node_col, time_cols, node_list)
        except Exception as e:
            print(f"[fail] {typ}: error matrix read error: {e}", file=sys.stderr)
            continue

        # Δs = e_t - e_{t-1}
        delta_s = (e_t - e_tm1).to_numpy(dtype=float)

        # Params: current and previous
        try:
            p_t_map   = read_params_map(os.path.join(args.params_dir, typ + args.suffix))
            p_tm1_map = read_params_map(os.path.join(args.prev_params_dir, typ + args.suffix))
        except Exception as e:
            print(f"[fail] {typ}: params read error: {e}", file=sys.stderr)
            continue

        out_map: Dict[str, List[float]] = {}

        for name in node_list.astype(str):
            p_t   = p_t_map.get(name, [args.init_value]*L)
            p_tm1 = p_tm1_map.get(name, [args.init_value]*L)
            p_t   = np.asarray(ensure_len(p_t,   L, args.init_value), dtype=float)
            p_tm1 = np.asarray(ensure_len(p_tm1, L, args.init_value), dtype=float)

            # Current error for this node
            e_cur = e_t.loc[name].to_numpy(dtype=float) if name in e_t.index else np.zeros(L, dtype=float)

            # Δp
            delta_p = p_t - p_tm1

            # scale = Δp / (Δs + eps)
            denom = delta_s[e_t.index.get_loc(name)] + args.eps  # row aligned with same node order
            scale = delta_p / denom

            # Optional clipping to avoid huge steps if Δs ≈ 0
            if args.max_scale is not None:
                scale = np.clip(scale, -args.max_scale, args.max_scale)

            # Update
            p_new = p_t - args.lr * e_cur * scale

            out_map[name] = p_new.tolist()

        out_path = os.path.join(args.out_dir, typ + args.suffix)
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
