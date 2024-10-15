# MASFENON
Standalone project for the framework behind [c2c-sepia](https://github.com/josura/c2c-sepia), containing only the most recent version of the MASFENON framework.

## DEPENDENCIES
- [armadillo](https://arma.sourceforge.net/)
- MPI implementation (tested on [OpenMPI](https://www.open-mpi.org))
- [boost](https://www.boost.org/) (eventually program options as well)

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


## USAGE
### HELP MESSAGE
```bash
./build/masfenon --help
```
