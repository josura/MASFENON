# MASFENON
Standalone project for the framework behind [c2c-sepia](https://github.com/josura/c2c-sepia), containing only the most recent version of the MASFENON framework.

## DEPENDENCIES
- [armadillo](https://arma.sourceforge.net/)
- MPI implementation (tested on [OpenMPI](https://www.open-mpi.org))
- [boost](https://www.boost.org/) (eventually program options as well)

## FILE STRUCTURE
```shell
.
├── build # contains the object files
├── src # contains the source files
└── scripts # contains some scripts
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
