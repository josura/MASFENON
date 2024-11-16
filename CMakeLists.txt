cmake_minimum_required(VERSION 3.24)

project(masfenon)

#if (UNIX)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -fopenmp")   #  -lblas -llapack are included directly #blas is very fast for me on this machine
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -lopenblas")

add_definitions(-DARMA_DONT_USE_WRAPPER)
# set (CMAKE_BINARY_DIR "bin")
# set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# set cmake policy to find packages to NEW (using <package_name>_ROOT to find packages)
# cmake_policy(SET CMP0144 NEW)
# set cmake policy to CMP0167 to NEW to ``find_package(Boost)`` to search for the upstream ``BoostConfig.cmake``.
cmake_policy(SET CMP0167 NEW)
# set boost root and linker path to find boost, not necessary if boost is installed in the system and there is only one version
# set(BOOST_ROOT "/path/to/boost")
# set(LD_LIBRARY_PATH "/path/to/boost/lib:$LD_LIBRARY_PATH")
#export CPLUS_INCLUDE_PATH=/usr/local/include:$CPLUS_INCLUDE_PATH


find_package(MPI REQUIRED)

find_package(Armadillo REQUIRED)
include_directories(${ARMADILLO_INCLUDE_DIRS})

find_package(BLAS)
find_package(LAPACK)
if(LAPACK_FOUND AND BLAS_FOUND)
  set(lapackblas_libraries ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES})
else()
  message( SEND_ERROR "lapack or blas not found")
endif()


FIND_PACKAGE(Boost COMPONENTS program_options REQUIRED)
# print out the boost include directories
MESSAGE( STATUS "Boost_INCLUDE_DIRS: " ${Boost_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

# include the directories
include_directories(src)

set(EXEC_MAIN "src/mains/main.cxx")
set(EXEC_MPI_MAIN "src/mains/mainMPI.cxx")
set(EXPERIMENT_MAIN "src/mains/testingPerformance.cxx")
set(LIB_SRCS 
    src/data_structures/WeightedEdgeGraph.cxx
    src/utils/utilities.cxx
    src/utils/mathUtilities.cxx
    src/utils/stringUtilities.cxx
    src/utils/armaUtilities.cxx
    src/utils/optimization.cxx
    src/data_structures/Matrix.cxx
    src/computation/Computation.cxx
    src/computation/ComputationVectorized.cxx
    src/computation/DissipationModel.cxx
    src/computation/DissipationModelVectorized.cxx
    src/computation/DissipationModelPow.cxx
    src/computation/DissipationModelRandom.cxx
    src/computation/DissipationModelPeriodic.cxx
    src/computation/DissipationModelScaled.cxx
    src/computation/ConservationModel.cxx
    src/computation/ConservationModelVectorized.cxx
    src/computation/PropagationModel.cxx
    src/computation/PropagationModelVectorized.cxx
    src/computation/PropagationModelNeighbors.cxx
    src/computation/PropagationModelOriginal.cxx
    src/computation/PropagationModelCustom.cxx
    src/CustomFunctions.cxx
    src/logging/Logger.cxx
    src/checkpoint/Checkpoint.cxx
)

if(CUDAavailable)
#using CUDA implementation
elseif(OpenCLavailable)
#using OpenCL implementation
endif(CUDAavailable)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library( mysharedlib SHARED ${LIB_SRCS} )

add_executable( masfenon ${EXEC_MAIN} )
target_link_libraries( masfenon 
                       mysharedlib
                       ${lapackblas_libraries}
                       ${Boost_LIBRARIES})


add_executable( masfenon-MPI ${EXEC_MPI_MAIN} )
target_link_libraries( masfenon-MPI 
                        mysharedlib
                        ${lapackblas_libraries}
                        ${Boost_LIBRARIES}
                        MPI::MPI_CXX)

add_executable( experiments ${EXPERIMENT_MAIN})
target_link_libraries( experiments 
                       mysharedlib  
                       ${lapackblas_libraries})


# TESTING
enable_testing()

add_executable(GraphTesting  "src/testing/GraphTesting.cc")
target_link_libraries(
  GraphTesting
  GTest::gtest_main
  mysharedlib
  ${lapackblas_libraries}
)


add_executable(MatrixTesting  "src/testing/MatrixTesting.cc")
target_link_libraries(
  MatrixTesting
  GTest::gtest_main
  mysharedlib
  ${lapackblas_libraries}
)

add_executable(ComputationTesting  "src/testing/ComputationTesting.cc")
target_link_libraries(
  ComputationTesting
  GTest::gtest_main
  mysharedlib
  ${lapackblas_libraries}
)

add_executable(DissipationModelTesting  "src/testing/DissipationModelTesting.cc")
target_link_libraries(
  DissipationModelTesting
  GTest::gtest_main
  mysharedlib
  ${lapackblas_libraries}
)

add_executable(ConservationModelTesting  "src/testing/ConservationModelTesting.cc")
target_link_libraries(
  ConservationModelTesting
  GTest::gtest_main
  mysharedlib
  ${lapackblas_libraries}
)

add_executable(ComputationTestingPerturbation  "src/testing/ComputationTestingPerturbation.cc")
target_link_libraries(
  ComputationTestingPerturbation
  GTest::gtest_main
  mysharedlib
  ${lapackblas_libraries}
)

add_executable(armaUtilitiesTesting  "src/testing/armaUtilitiesTesting.cc")
target_link_libraries(
  armaUtilitiesTesting
  GTest::gtest_main
  mysharedlib
  ${lapackblas_libraries}
)

add_executable(utilitiesTesting  "src/testing/utilitiesTesting.cc")
target_link_libraries(
  utilitiesTesting
  GTest::gtest_main
  mysharedlib
  ${lapackblas_libraries}
)

add_executable(PropagationModelTesting  "src/testing/PropagationModelTesting.cc")
target_link_libraries(
  PropagationModelTesting
  GTest::gtest_main
  mysharedlib
  ${lapackblas_libraries}
)

include(GoogleTest)
gtest_discover_tests(GraphTesting)
gtest_discover_tests(MatrixTesting)
gtest_discover_tests(ComputationTesting)
gtest_discover_tests(DissipationModelTesting)
gtest_discover_tests(ConservationModelTesting)
gtest_discover_tests(ComputationTestingPerturbation)
gtest_discover_tests(armaUtilitiesTesting)
gtest_discover_tests(utilitiesTesting)
gtest_discover_tests(PropagationModelTesting)