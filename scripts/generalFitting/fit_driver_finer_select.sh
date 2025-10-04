#!/usr/bin/env bash
set -euo pipefail

# -----------------------------
# MAS Fitting Driver (Bash), with finer gradient building(single parameter perturbation), and selection of network to fit
# -----------------------------
# Requirements:
# - bash, awk, python3
# - Scripts available/executable:
#     createErrorMatrix.py
#     createParametersIterationAware-singleParameter.py
#     computeRMSE-fromErrorMatrix.py
# - MASFENON simulator executable (invoked under mpirun)
# -----------------------------
# TODO - add threshold when the errors of other parameters are too high, meaning that the single parameter perturbation has bigger consequences for other parameters than the one being perturbed
