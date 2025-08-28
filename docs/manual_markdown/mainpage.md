MASFENON documentation                         {#mainpage}
============
[![Docs](https://img.shields.io/badge/docs-latest-blue.svg)](https://josura.github.io/MASFENON/)
[![License](https://img.shields.io/badge/license-CC%20BY%204.0-lightgrey.svg)](http://creativecommons.org/licenses/by/4.0/)
[![Language](https://img.shields.io/github/languages/top/josura/MASFENON)](https://github.com/search?q=repo%3Ajosura%2FMASFENON++language%3AC%2B%2B&type=code)

---

**MASFENON** (Multiagent Adaptive Simulation Framework for Evolution in Networks of Networks) is a standalone
simulation framework designed to handle complex dynamic systems through modular multi-agent interaction.
It was originally developed as part of the [c2c-sepia](https://github.com/josura/c2c-sepia) project and has now been extracted as a clean, optimized version.

---

## 🔧 Dependencies
- [Armadillo](https://arma.sourceforge.net/)
- MPI implementation (tested on [OpenMPI](https://www.open-mpi.org))
- [Boost](https://www.boost.org/) (make sure `program_options` is installed if needed)

## 📁 File Structure
```txt
.
├── CMakeLists.txt           # CMake build configuration
├── examples.md              # Usage and plotting examples
├── .github/                 # CI workflows
├── build/                   # Build output
├── src/                     # Source code
├── data/                    # Sample/test datasets
└── scripts/                 # Bash and Python helper scripts
```

## Other sections
- \ref Building "Building"
- \ref FittingSingleCell "Fitting of single cell experiments"
- \ref Visualization "Visualization of simulation results"
- \ref GeneralDocumentation "General framework documentation"

## 🧪 Usage
### Help Message
```bash
./build/masfenon-MPI --help
```

### Example Run
```bash
./build/masfenon-MPI \
--fUniqueGraph graph.tsv \
--fInitialPerturbationPerType matrix.tsv \
--typeInteractionFolder typesInteractionFolder/
```

## ⚙️ Parameter Customization
Custom scaling functions (dissipation, conservation, etc.) can be modified in the `src/CustomFunctions` files.

## 📊 Propagation function customization
Propagation functions can be customized in the `src/computation/PropagationModelCustom` files.

 
## 📄 License
This project is released under the [Creative Commons Attribution 4.0 International License](http://creativecommons.org/licenses/by/4.0/).

If you encounter any issues, feel free to open one on the [GitHub issues page](https://github.com/josura/MASFENON/issues).