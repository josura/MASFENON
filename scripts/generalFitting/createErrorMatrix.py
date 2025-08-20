#!/usr/bin/env python3
"""
This script creates an error matrix for fitting models to synthetic time series data. The synthetic data can be swapped with real data, but additional steps may be required to ensure compatibility with the fitting process.

This script takes one simulation output folder as input, and a real data time series folder as input. The files inside each folder are expected to be from the MASFENON output, that is
- The folder for the simulation output should contain files named `t0.tsv`, `t1.tsv`, etc., which represent the different agent names, inside these files, the header is composed by the first column "nodeNames", the other columns are the timepoints. The  rows are the node name and the value at that timepoint.
- The folder for the real data time series should contain files named `t0.tsv`, `t1.tsv`, etc., which represent the different agent names, inside these files, the header is composed by the first column "nodeNames", the other columns are the timepoints. The rows are the node name and the value at that timepoint.