#!/usr/bin/env python3
"""
Compute aggregated RMSE from an error matrix TSV.

Input:
  --file : path to a TSV file (error matrix)
           - first column = node IDs (default 'nodeNames')
           - remaining columns = timepoints with numeric errors

Output:
  Prints the RMSE (single float) to stdout.
"""

import argparse
import pandas as pd
import numpy as np
import sys


def main():
    ap = argparse.ArgumentParser(description="Compute RMSE from an error matrix file.")
    ap.add_argument("--file", required=True, help="Path to error matrix TSV.")
    ap.add_argument("--node-col", default="nodeNames", help="Name of node column (default: nodeNames).")
    args = ap.parse_args()

    try:
        df = pd.read_csv(args.file, sep="\t")
    except Exception as e:
        print(f"[error] Failed to read {args.file}: {e}", file=sys.stderr)
        sys.exit(2)

    if args.node_col not in df.columns:
        print(f"[error] File missing required column {args.node_col}. Found: {list(df.columns)}", file=sys.stderr)
        sys.exit(2)

    # numeric values only (drop node column)
    vals = df.drop(columns=[args.node_col]).to_numpy(dtype=float)

    # Flatten, remove NaNs
    vals = vals[np.isfinite(vals)]

    if vals.size == 0:
        print("[error] No numeric error values found.", file=sys.stderr)
        sys.exit(1)

    rmse = np.sqrt(np.mean(vals**2))
    print(f"{rmse:.10g}")


if __name__ == "__main__":
    main()
