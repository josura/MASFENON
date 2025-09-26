#!/usr/bin/env python3
"""
Plot RMSE from a file containing RMSE values.
Input file format:
- TSV with two columns: epoch (int), RMSE (float).
"""

import argparse
import pandas as pd
import matplotlib.pyplot as plt
import os
import sys


def plot_rmse(input_file: str, output_file: str = None):
    # Read TSV file
    df = pd.read_csv(input_file, sep="\t", header=None, names=["epoch", "rmse"])

    # Plot
    plt.figure(figsize=(8, 5))
    plt.plot(df["epoch"], df["rmse"], marker="o", linestyle="-", label="RMSE")
    plt.xlabel("Epoch")
    plt.ylabel("RMSE")
    plt.title("RMSE over Epochs")
    plt.grid(True)
    plt.legend()

    # Save or show
    if output_file:
        plt.savefig(output_file, dpi=300, bbox_inches="tight")
        print(f"Plot saved to {output_file}")
    else:
        plt.show()


def main():
    parser = argparse.ArgumentParser(description="Plot RMSE from a file.")
    parser.add_argument("input_file", type=str, help="Path to the input TSV file containing RMSE values.")
    parser.add_argument("-o", "--output_file", type=str, default=None, help="Path to save the output plot (optional).")
    args = parser.parse_args()

    plot_rmse(args.input_file, args.output_file)
    sys.exit(0)


if __name__ == "__main__":
    main()
