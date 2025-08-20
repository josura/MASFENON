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

import os
import sys
import pandas as pd