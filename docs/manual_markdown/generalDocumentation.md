General framework documentation           {#GeneralDocumentation}
==============
# Framework Documentation

This part of the documentation serves as a practical guide for users who wish to explore the technical details of the **MASFENON** framework and learn how to operate it effectively. Here we provide an overview of MASFENON usage, including:

- Running MASFENON with the provided scripts.
- Understanding and configuring simulation parameters.
- Addressing common deployment issues, such as running MPI in local builds.
- Notes on parallel execution and hardware considerations.

The purpose of this section is not only to document operational procedures, but also to support reproducibility of the experiments described in the thesis and to help future users adapt MASFENON to their own research use cases.

For readers seeking more in-depth technical documentation of the internal implementation (class structures, methods, source code references, etc.), the complete and always up-to-date **Doxygen-generated documentation** is publicly available at:

👉 [https://josura.github.io/MASFENON/](https://josura.github.io/MASFENON/)

This online resource provides detailed developer-oriented documentation, including C++ class hierarchies, function descriptions, code examples, and internal module dependencies. It is recommended for advanced users and developers intending to extend the framework or integrate it into custom workflows.

---

## B.1 Introduction

**MASFENON** (Multiagent Adaptive Simulation Framework for Evolution in Networks of Networks) is a standalone simulation framework designed to handle complex dynamic systems through modular multi-agent interaction. It was originally developed as part of the **c2c-sepia** project and has now been extracted as a clean, optimized version.

### B.1.1 Dependencies

- [Armadillo](https://arma.sourceforge.net/)
- MPI implementation (tested on [OpenMPI](https://www.open-mpi.org))
- [Boost](https://www.boost.org/) — ensure `program_options` is installed if needed

### B.1.2 File Structure

```

.
\|-- CMakeLists.txt           # CMake build configuration
\|-- examples.md              # Usage and plotting examples
\|-- .github/                 # CI workflows
\|-- build/                   # Build output
\|-- src/                     # Source code
\|-- data/                    # Sample/test datasets
\-- scripts/                  # Bash and Python helper scripts

```

### B.1.3 Build

```bash
cmake -B build
cd build
make
```

### B.1.4 Usage

**Help Message:**

```bash
./build/masfenon-MPI --help
```

**Example Run:**

```bash
./build/masfenon-MPI \
  --fUniqueGraph graph.tsv \
  --fInitialPerturbationPerType matrix.tsv \
  --typeInteractionFolder typesInteractionFolder/
```

### B.1.5 Parameter Customization

Custom scaling functions (dissipation, conservation, etc.) can be modified in `src/CustomFunctions`.

### B.1.6 Propagation Function Customization

Propagation functions can be customized in `src/computation/PropagationModelCustom`.

### B.1.7 License

This project is released under the [Creative Commons Attribution 4.0 International License](http://creativecommons.org/licenses/by/4.0/).

For issues, please use the GitHub issues page: [https://github.com/josura/MASFENON/issues](https://github.com/josura/MASFENON/issues)

---

## B.2 Inputs and Parameters of the Framework

This section documents the command-line interface (CLI) inputs, expected file formats, and configuration parameters of the **MASFENON** framework. Options are provided via Boost.Program\_options. Boolean switches are disabled by default unless explicitly set.

### B.2.1 Data Inputs and File Formats

At minimum, the framework requires:

* A graph (unique file or a folder of graphs), and
* An initial perturbation matrix (unique file or a folder of matrices).

Exactly one of the following must be specified for graphs:

* `--fUniqueGraph <file>` **OR** `--graphsFilesFolder <folder>`

Exactly one of the following should be specified for perturbations:

* `--fInitialPerturbationPerType <file>` **OR** `--initialPerturbationPerTypeFolder <folder>`

Optionally, inter-type interactions, node descriptions, and a list of subtypes can be provided.

#### File Schemas

**Graph (edge list):** `graph.tsv`

```
start    end    weight
geneA    geneB  0.123
...
```

**Initial perturbation (homogeneous):** `initialPerturbationHomogeneous.tsv`

```
name    type1          type2        ...    typeN
gene1   lfc_type1:g1   lfc_type2:g1 ...    lfc_typeN:g1
gene2   lfc_type1:g2   lfc_type2:g2 ...    lfc_typeN:g2
...
```

**Initial perturbation (heterogeneous):** `initialPerturbation-typeN.tsv`

```
name    value
gene1   lfc_typeN:g1
gene2   lfc_typeN:g2
...
```

**Inter-type interactions:** `typesInteraction.tsv`

```
startType  startNodeName  endType  endNodeName  weight
type0      gene-e         type1    gene-a       1.0
...
```

**Subtypes list (optional):** `subtypes.txt`

```
type1
type3
...
```

**Node descriptions (optional)**:

* Single file: `--nodeDescriptionFile <file>`
* Per-graph folder: `--nodeDescriptionFolder <folder>`

Each file should list node identifiers (and optionally annotations). If not provided, nodes are inferred from edges.

---

### B.2.2 Input Locations

* `--fUniqueGraph <string>`
* `--graphsFilesFolder <string>`
* `--fInitialPerturbationPerType <string>`
* `--initialPerturbationPerTypeFolder <string>`
* `--typeInteractionFolder <string>`
* `--nodeDescriptionFile <string>`
* `--nodeDescriptionFolder <string>`
* `--subtypes <string>`

---

### B.2.3 Dynamics and Time Discretization

* `--intratypeIterations <int>`
* `--intertypeIterations <int>`
* `--timestep <double>` → Δt
  Total simulated time: `T = I_inter × Δt`
* `--quantizationMethod <string>` (`single` or `multiple`)

---

### B.2.4 Physical/Computational Models and Parameters

**Dissipation**

* `--dissipationModel <string>`: `none | power | random | periodic | scaled | custom`
* `--dissipationModelParameters <vector<double>>`

**Conservation**

* `--conservationModel <string>`: `none | scaled | random | custom`
* `--conservationModelParameters <vector<double>>`

**Propagation**

* `--propagationModel <string>`: `default | scaled | neighbors | customScaling | customScalingNeighbors | customPropagation`
* `--propagationModelParameters <vector<double>>`

---

### B.2.5 Saturation and Norm Constraints

* `--saturation`
* `--saturationTerm <double>` → bounds \[-s, s]
* `--customSaturationFunction`
* `--conservateInitialNorm`

---

### B.2.6 Topology and Virtual Nodes

* `--undirectedEdges`
* `--undirectedTypeEdges`
* `--sameTypeCommunication`
* `--virtualNodesGranularity <string>` (`type`, `node`, `typeAndNode`)
* `--virtualNodesGranularityParameters <vector<string>>`
* `--resetVirtualOutputs`

---

### B.2.7 Logging, Robustness, and Runtime Controls

* `--loggingOptions <string>`: `all` (default) or `none`
* `--verbose`
* `--treatWarningAsError`
* `--resumeCheckpoint`

---

### B.2.8 Outputs and Performance Tracking

* `--outputFolder <string>`
* `--outputFormat <string>` (`singleIteration` or `iterationMatrix`)
* `--saveAugmentedNetworks`
* `--savePerformance <string>`

---

### B.2.9 Example CLI Invocation

```bash
./masfenon-MPI \
  --fUniqueGraph data/testdata/testGraph/edges-Graph1-general.tsv \
  --fInitialPerturbationPerType data/testdata/testGraph/initialValues-general.tsv \
  --typeInteractionFolder data/testdata/testHeterogeneousGraph/interactions \
  --intratypeIterations 20 \
  --intertypeIterations 10 \
  --timestep 0.1 \
  --propagationModel scaled \
  --propagationModelParameters 0.8 \
  --dissipationModel power \
  --dissipationModelParameters 0.95 \
  --conservationModel scaled \
  --conservationModelParameters 0.9 \
  --saturation --saturationTerm 1.0 \
  --undirectedEdges --undirectedTypeEdges \
  --outputFolder out/run1 \
  --outputFormat iterationMatrix \
  --savePerformance out/run1/perf.tsv \
  --loggingOptions all --verbose
```

---

### B.2.10 Tabular Summary of Options

| Option                              | Type           | Default         | Notes                                            |
| ----------------------------------- | -------------- | --------------- | ------------------------------------------------ |
| `fUniqueGraph`                      | string         | —               | Single graph file; XOR with `graphsFilesFolder`. |
| `graphsFilesFolder`                 | string         | —               | Folder of graphs.                                |
| `fInitialPerturbationPerType`       | string         | —               | Single perturbation file.                        |
| `initialPerturbationPerTypeFolder`  | string         | —               | Folder of perturbation files.                    |
| `typeInteractionFolder`             | string         | —               | Inter-type edges folder.                         |
| `nodeDescriptionFile`               | string         | —               | Node names (common graph).                       |
| `nodeDescriptionFolder`             | string         | —               | Node names (per-graph).                          |
| `subtypes`                          | string         | —               | Optional subtype list.                           |
| `intratypeIterations`               | int            | —               | >0                                               |
| `intertypeIterations`               | int            | —               | >0                                               |
| `timestep`                          | double         | —               | Δt                                               |
| `quantizationMethod`                | string         | —               | single \| multiple                               |
| `dissipationModel`                  | string         | none            | See list above.                                  |
| `dissipationModelParameters`        | vector<double> | —               | Model-dependent.                                 |
| `conservationModel`                 | string         | none            | See list above.                                  |
| `conservationModelParameters`       | vector<double> | —               | Model-dependent.                                 |
| `propagationModel`                  | string         | default         | See list above.                                  |
| `propagationModelParameters`        | vector<double> | —               | Model-dependent.                                 |
| `saturation`                        | flag           | off             | Enable clipping.                                 |
| `saturationTerm`                    | double         | 1.0             | Requires `--saturation`.                         |
| `customSaturationFunction`          | flag           | off             | User-defined.                                    |
| `conservateInitialNorm`             | flag           | off             | Norm constraint.                                 |
| `undirectedEdges`                   | flag           | off             | Intra-graph undirected.                          |
| `undirectedTypeEdges`               | flag           | off             | Inter-type undirected.                           |
| `sameTypeCommunication`             | flag           | off             | Same-type virtual node.                          |
| `resetVirtualOutputs`               | flag           | off             | Zeroed each iteration.                           |
| `virtualNodesGranularity`           | string         | type            | type \| node \| typeAndNode                      |
| `virtualNodesGranularityParameters` | vector<string> | —               | Reserved.                                        |
| `loggingOptions`                    | string         | all             | all \| none                                      |
| `verbose`                           | flag           | off             | Extra logs.                                      |
| `treatWarningAsError`               | flag           | off             | Fail on warning.                                 |
| `resumeCheckpoint`                  | flag           | off             | Resume run.                                      |
| `outputFolder`                      | string         | —               | Required for outputs.                            |
| `outputFormat`                      | string         | singleIteration | Iteration output mode.                           |
| `saveAugmentedNetworks`             | flag           | off             | Save augmented graphs.                           |
| `savePerformance`                   | string         | off             | Save runtime summary.                            |

---

### B.2.11 Additional considerations

If there are problems with the options, file, or consistency, the program will throw exceptions with explanatory error messages, or print warnings. Use `--verbose` for more details.

## B.3 Fitting Procedure and Scripts

In addition to the general-purpose simulation parameters, **MASFENON** includes a dedicated fitting workflow for calibrating the simulation to experimental single-cell data. In the current implementation, this process has been applied to fit the model to the *AT1* cell metabolite layer in scRNA-seq data.

### B.3.1 Overview

The fitting pipeline iteratively adjusts the **dissipation**, **propagation**, and **conservation** model parameters to minimize the mean squared error (MSE) between simulated outputs and experimental data at specific timepoints. The process is controlled by a main Bash script, which:

1. Sets the fitting mode (manual parameter ranges or resume from previous results).
2. Runs multiple **epochs**, each consisting of:
   - Executing the MASFENON simulation with the current parameter ranges.
   - Computing MSE values for each experiment/timepoint.
   - Generating updated parameter ranges for the next epoch, based on MSE ranking.
3. Saves all intermediate results, MSE tables, and updated parameter ranges in a user-defined output folder.

### B.3.2 Required Scripts

The fitting workflow depends on the following scripts:

- **`simulation_custom_parameters.sh`**  
  Runs a MASFENON simulation with explicit dissipation, propagation, and conservation parameter ranges.  
  Parameters:  
```

minDiss maxDiss dissSteps minProp maxProp propSteps minCons maxCons consSteps

```

- **`compute-MSE-metabolites.py`**  
Computes the MSE between simulation outputs and experimental data for each available timepoint (e.g., 6h, 7h, 10h).  
Outputs an MSE-sorted table in the format:
```

MSE    experiment
0.012  dissipation\_<d>-propagation\_<p>-conservation\_<c>
...

```
This script is part of the [c2c-sepia repository](https://github.com/josura/c2c-sepia).

- **`createNewParametersFromMSEtable.sh`**  
Reads an MSE table and generates new parameter ranges around the best-performing configurations, for use in the next fitting epoch.

### B.3.3 Execution Modes

The main fitting script supports two execution modes:

**Manual Range Mode**
```bash
./fit-and-run-simulation.sh <epochs> <OutputFolder> <minParam> <maxParam> <intervals>
```

All three parameter families (dissipation, propagation, conservation) share the same starting min, max, and number of intervals.

**Resume Mode**

```bash
./fit-and-run-simulation.sh <epochs> <OutputFolder>
```

Reads previous simulation results from `<OutputFolder>`, computes the MSE tables, and generates new ranges from them. Useful to continue fitting without restarting from scratch.

### B.3.4 Epoch Loop and Parameter Update

Each epoch proceeds as follows:

1. Create an `epoch_<n>` subfolder inside `<OutputFolder>`.
2. Run the simulation with current parameter ranges:

   ```bash
   bash simulation_custom_parameters.sh \
       minDiss maxDiss dissSteps \
       minProp maxProp propSteps \
       minCons maxCons consSteps
   ```
3. Compute MSE values for each timepoint:

   ```bash
   python3 compute-MSE-metabolites.py epoch_<n>
   ```
4. From the MSE table of the selected fitting timepoint (e.g., 10h), extract top configurations and generate updated parameter ranges:

   ```bash
   bash createNewParametersFromMSEtable.sh \
       epoch_<n>/mse_10h.tsv \
       5 \
       <intervals> \
       epoch_<n>/newParams_mse_10h.txt
   ```
5. Update `min`, `max`, and `intervals` for each parameter family from the generated file.

### B.3.5 Environment Requirements

The fitting process requires:

* A Python virtual environment with dependencies for `compute-MSE-metabolites.py`. The main script activates it via:

  ```bash
  source /path/to/venv/bin/activate
  ```
* The [c2c-sepia repository](https://github.com/josura/c2c-sepia) for MSE computation scripts.
* The **MASFENON** repository for simulation scripts.

### B.3.6 Outputs

For each epoch, the output folder contains:

* Simulation outputs from the current parameters.
* MSE tables for all fitted timepoints.
* Updated parameter range files for the next epoch.

### B.3.7 Example Run

Example: 3-epoch manual range fitting starting from 0.1–1.0 with 5 intervals:

```bash
./fit-and-run-simulation.sh 3 out_fitting 0.1 1.0 5
```

The final MSE results and the best parameters are stored in `out_fitting`.

### B.3.8 Per-Node Error-Based Adjustment Script

In addition to the global fitting workflow, a second (non-public) script exists for fine-grained parameter tuning.
Instead of computing MSE on aggregated outputs, this script:

1. Selects a specific network or subnetwork.
2. Computes error **per individual node** with respect to experimental data.
3. Aggregates per-node errors to identify parameter sensitivities.
4. Adjusts dissipation, propagation, and conservation parameters independently.

This method is useful for:

* Networks with heterogeneous dynamics.
* Subsets of nodes representing critical biological processes.
* Iterative localized parameter tuning.

---

## B.4 Visualization

In addition to numerical outputs, **MASFENON** includes interactive visualization tools for exploring simulation results.

Two visualization scripts are available:

1. A single-network plotter for node time series (in `c2c-sepia/scripts/python/temporalSingleCell/plotResults_withLR.py`).
2. A web-based multi-network explorer using Flask and Plotly (in `MASFENON/scripts/python/visualization/visualizationWebPageComplete.py`).

### B.4.1 Web Application for Multi-Network Visualization

This tool provides an interactive web interface for visualizing all simulated networks and their temporal evolution.
Implemented in **Flask** and **Plotly**, it allows:

* Selecting a network (*type*) from outputs.
* Selecting a timepoint to visualize.
* Inspecting node attributes and edge weights interactively.
* Color mapping and node size scaling based on values.

#### B.4.1.1 Data Sources

The app uses:

1. **Iteration matrices**: `<OutputDirectory>/<type>.tsv`
2. **Augmented graph files**: `<AugmentedGraphDirectory>/<type>.tsv`
3. **Initial node values**: `<InputValuesDirectory>/<type>.tsv`

#### B.4.1.2 Preprocessing and Caching

At startup, the app:

* Reads all `.tsv` output files.
* Loads node value series and graph structure.
* Generates a static **networkx spring layout**.
* Creates Plotly traces for edges and nodes for every timepoint.
* Saves pickled caches:

  * `node_traces_for_networks.pkl`
  * `edge_traces_for_networks.pkl`
  * `dict_for_plot.pkl`

#### B.4.1.3 Server Routes

* `/` → main interface with slider
* `/read_types` → list of network types
* `/types/<type>` → set active network
* `/plot/<timepoint>` → figure JSON for current type at timepoint
* `/plot_singular/<type>/<timepoint>` → figure JSON for specific type/timepoint

#### B.4.1.4 Visualization Details

* **Edges**: thin gray lines (`#888`), hover = weight
* **Nodes**: `YlGnBu` reversed scale, size ∈ \[4,22]
* **Layout**: fixed `spring_layout`
* **Annotations**: fixed parameter settings shown

#### B.4.1.5 Workflow Summary

```bash
python3 visualizationWebPageComplete.py
```

Then open [http://127.0.0.1:5000](http://127.0.0.1:5000) in a browser.

---

## B.5 Legacy Framework: c2c-SEPIA

Before the standalone release of **MASFENON**, its initial development and related experiments, scripts, and input preparation pipelines were maintained in the [c2c-sepia repository](https://github.com/josura/c2c-sepia).

This legacy repository still contains:

* Original MASFENON source code (pre-migration).
* Scripts for single-cell experiments.
* Utilities for image documentation.
* Example datasets and workflows for cell-to-cell communication (CCC) simulations.

It was designed for **cell-to-cell communication simulations with enriched pathways**, using annotated scRNA-seq data and multi-agent system (MAS) principles, extended with parallel and distributed computation.

Dependencies and build process are nearly identical to MASFENON. The legacy repo now mainly contains domain-specific scripts for CCC simulation.

### Additional Scripts and Utilities

The **MASFENON** repository also provides scripts for:

* Input dataset generation (biological, epidemiological, etc.).
* Preprocessing pipelines for single-cell data (including missing-data handling).
* Scenario-specific transformations and format conversions.

These are **not detailed here** since:

* Input preparation was already described in the main text.
* Scripts are documented in other research articles and repository `README` files.

For comprehensive instructions, refer to those sources.
