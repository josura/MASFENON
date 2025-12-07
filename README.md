# MASFENON
[![Docs](https://img.shields.io/badge/docs-latest-blue.svg)](https://josura.github.io/MASFENON/)
[![License](https://img.shields.io/badge/license-CC%20BY%204.0-lightgrey.svg)](http://creativecommons.org/licenses/by/4.0/)
[![Language](https://img.shields.io/github/languages/top/josura/MASFENON)](https://github.com/search?q=repo%3Ajosura%2FMASFENON++language%3AC%2B%2B&type=code)


Standalone project for the framework behind [c2c-sepia](https://github.com/josura/c2c-sepia), containing only the most recent version of the MASFENON framework. The meaning of the acronym is **M**ultiagent **A**daptive **S**imulation **F**ramework for **E**volution in **N**etworks **o**f **N**etworks. Simulating the communication dynamics in complex systems represented as networks of networks. Encoding temporal communication between agents as perturbations that spread through the system according to predefined interaction rules.

## DEPENDENCIES
- [armadillo](https://arma.sourceforge.net/)
- MPI implementation (tested on [OpenMPI](https://www.open-mpi.org))
- [boost](https://www.boost.org/) (if you have some problems during compilation, consider installing program options from boost as well)

## FILE STRUCTURE
```shell
.
├── CMakeLists.txt # cmake file
├── examples.md # examples of how to use the program
├── .github # contains the github actions
├── build # contains the object files
├── src # contains the source files
├── data # contains the test data files
└── scripts # contains some scripts in bash and python to be used in the project to visualize the reulting data
```

## BUILD
```bash
cmake -B build
cd build
make
```


## PARAMETER CUSTOMIZATION
The custom functions used for the scaling of the perturbation values (dissipation and conservation) are defined in <src/CustomFunctions> in the source folder


## USAGE
### HELP MESSAGE
```bash
./build/masfenon-MPI --help
```

### EXAMPLE
```bash
./build/masfenon-MPI --fUniqueGraph [graph].tsv --fInitialPerturbationPerType [matrix].tsv --typeInteractionFolder [typesInteractionFolder]
```

# LICENSE

Under development, if any problems arise open an issue in the [repository](https://github.com/josura/MASFENON/issues)

Shield: [![CC BY 4.0][cc-by-shield]][cc-by]

This work is licensed under a
[Creative Commons Attribution 4.0 International License][cc-by].

[![CC BY 4.0][cc-by-image]][cc-by]

[cc-by]: http://creativecommons.org/licenses/by/4.0/
[cc-by-image]: https://i.creativecommons.org/l/by/4.0/88x31.png
[cc-by-shield]: https://img.shields.io/badge/License-CC%20BY%204.0-lightgrey.svg
