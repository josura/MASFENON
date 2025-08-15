/**
 * @file mainMPI.cxx
 * @ingroup Mains
 * @brief Main function for the MASFENON simulation framework. With the use of MPI
 * @details This file contains the main function for running the MASFENON simulation framework with MPI. It handles command-line arguments, initializes the computation, and executes the simulation.
 * @note This version of the main function is designed for MPI platforms.
 * @note This version is the  most up to date and is the one that should be used for the simulations.
 * @todo Move the Agent related code and logic to separate files and classes.
 */
#include <mpi.h>
#include <iostream>
#include <vector>
#include <string>
#include <boost/program_options/value_semantic.hpp>
#include <iostream>
#include <boost/program_options.hpp>
#include <map>
#include <sys/types.h>
#include <tuple>
#include "computation/Computation.hxx"
#include "computation/PropagationModel.hxx"
#include "computation/PropagationModelOriginal.hxx"
#include "computation/PropagationModelNeighbors.hxx"
#include "computation/PropagationModelCustom.hxx"
#include "computation/ConservationModel.hxx"
#include "computation/DissipationModel.hxx"
#include "computation/DissipationModelPow.hxx"
#include "computation/DissipationModelRandom.hxx"
#include "computation/DissipationModelScaled.hxx"
#include "data_structures/WeightedEdgeGraph.hxx"
#include "utils/utilities.hxx"
#include "utils/mathUtilities.hxx"
#include "CustomFunctions.hxx"
#include "logging/Logger.hxx"
#include "checkpoint/Checkpoint.hxx"
#include "utils/boost_ignore_numbers_parser.hxx"

namespace po = boost::program_options;///< namespace for program options
    

int main(int argc, char** argv) {    
    //program options
    bool sameTypeCommunication=false; ///< boolean variable to indicate if the same type communication is used
    bool saturation=false; ///< boolean variable to indicate if saturation is used
    bool customSaturation=false; ///< boolean variable to indicate if custom saturation is used
    bool conservateInitialNorm=false; ///< boolean variable to indicate if the initial norm is conserved
    bool undirected = false; ///< boolean variable to indicate if the single graphs associated to every type are undirected
    bool undirectedTypeEdges = false; ///< boolean variable to indicate if the edges between types are undirected
    bool resetVirtualOutputs = false; ///< boolean variable to indicate if the virtual outputs are reset at each iteration
    bool resumeCheckpoint = false; ///< boolean variable to indicate if the computation should resume from the checkpoint
    bool saveAugmentedNetworks = false; ///< boolean variable to indicate if the augmented networks should be saved
    std::string logMode=""; ///< string variable to indicate the logging mode
    bool verbose = false; ///< boolean variable to indicate if verbose mode is used
    bool treatWarningAsError = false; ///< boolean variable to indicate if warnings should be treated as errors
    std::string quantizationMethod = "single"; ///< string variable to indicate the quantization method
    std::string virtualNodesGranularity = "type"; ///< string variable to indicate the virtual nodes granularity
    std::string performanceFilename = ""; ///< string variable to indicate the performance filename where the performance times are saved
    std::string outputFormat = "singleIteration"; ///< string variable to indicate the output format
    po::options_description desc("Allowed options"); ///< options description
    desc.add_options()
        ("help", "() print help section")//<initialPerturbationPerType>.tsv [<subtypes>.txt] [<typesInteraction>.tsv]\nFILE STRUCTURE SCHEMA:\ngraph.tsv\nstart end weight\n<gene1> <gene2>  <0.something>\n...\n\n\ninitialPerturbationPerType.tsv\n type1 type2 ... typeN\ngene1 <lfc_type1:gene1> <lfc_type2:gene1> ... <lfc_typeN:gene1>\ngene1 <lfc_type1:gene2> <lfc_type2:gene2> ... <lfc_typeN:gene2>\n...\n\n\ntypesInteraction.tsv\nstartType:geneLigand endType:geneReceptor weight\n<type1:geneLigand> <type2:genereceptor>  <0.something>\n...\n\n\nsubtypes.txt\ntype1\ntype3\n...")
        ("fUniqueGraph", po::value<std::string>(), "(string) graph filename, for an example graph see in resources. NOTE: if this option is chosen, graphsFilesFolder cannot be used. For an example see in data data/testdata/testGraph/edges-Graph1-general.tsv")
        ("fInitialPerturbationPerType", po::value<std::string>(), "(string) initialPerturbationPerType matrix filename, for an example see in data data/testdata/testGraph/initialValues-general.tsv")
        ("subtypes", po::value<std::string>(), "subtypes filename, for an example see in data, see data/testdata/testGraph/subcelltypes.txt")
        ("initialPerturbationPerTypeFolder", po::value<std::string>(), "(string) initialPerturbationPerType folder, for an example see in data data/testdata/testGraph/initialValues")
        ("typeInteractionFolder", po::value<std::string>(), "(string) directory for the type interactions, for an example see in data data/testdata/testHeterogeneousGraph/interactions")
        ("nodeDescriptionFile", po::value<std::string>(), "(string) node description file, used to generate the output description in the case of common graph between types, if not specified no names are used. For an example see in data resources/graphs/metapathwayNew/nodes.tsv")
        ("nodeDescriptionFolder", po::value<std::string>(), "(string) nodes folder, where the files containing the description/nodes for all the graphs are contained, used to read the graph nodes, if not specified the graphs will be built with the edges files(could not contain some isolated nodes) for an example see the folder structure in data/testdata/testHeterogeneousTemporalGraph/nodesDescriptionDifferentStructure")
        ("sameTypeCommunication",po::bool_switch(&sameTypeCommunication),"() use same type communication, since it is not permitted as the standard definition of the model, this adds a virtual node for the same type type")
        ("outputFolder",po::value<std::string>(),"(string) output folder for output of the algorithm at each iteration")
        ("intertypeIterations",po::value<int>(),"(positive integer) number of iterations for intertype communication")
        ("intratypeIterations",po::value<int>(),"(positive integer) number of iterations for intratype communication")
        ("timestep",po::value<double>(),"timestep to use for the iteration, the final time is iterationIntracell*iterationIntercell*timestep")
        ("dissipationModel",po::value<std::string>(),"(string) the dissipation model for the computation, available models are: 'none (default)','power','random','periodic','scaled' and 'custom'")
        ("dissipationModelParameters",po::value<std::vector<double>>()->multitoken(),"(string) the parameters for the dissipation model, for the power dissipation indicate the base, for the random dissipation indicate the min and max value, for the periodic dissipation indicate the period")
        ("dissipationModelParameterFolder", po::value<std::string>(),"(string) the folder where the parameters for the dissipation model are contained. Only supported with 'custom' dissipation. Each type can have a parameter file as a mapping of node->parameter(or parameters), if a file is missing for a type, than the parameters for that type will be 0, same can be said about nodes with no mapping. if not specified, the default parameters are used or the parameters in dissipationModelParameters parameter are used")
        ("graphsFilesFolder",po::value<std::string>(),"(string) graphs (pathways or other types of graphs) file folder, for an example see in data data/testdata/testHeterogeneousGraph/graphsDifferentStructure")
        ("conservationModel",po::value<std::string>(),"(string) the conservation model used for the computation, available models are: 'none (default)','scaled','random' and 'custom' ")
        ("conservationModelParameters", po::value<std::vector<double>>()->multitoken(),"(vector<double>) the parameters for the dissipation model, for the scaled parameter the constant used to scale the conservation final results, in the case of random the upper and lower limit (between 0 and 1)")
        ("conservationModelParameterFolder", po::value<std::string>(),"(string) the folder where the parameters for the conservation model are contained. Only supported with 'custom' conservation. each type can have a parameter file as a mapping of node->parameter(or parameters), if a file is missing for a type, than the parameters for that type will be 0, same can be said about nodes with no mapping. if not specified, the default parameters are used or the parameters in conservationModelParameters parameter are used")
        ("propagationModel",po::value<std::string>(),"(string) the propagation model used for the computation, available models are: 'default(pseudoinverse creation)','scaled (pseudoinverse * scale parameter)', neighbors(propagate the values only on neighbors at every iteration and scale parameter) and 'customScaling' (pseudoinverse*scalingFunction(parameters)), 'customScalingNeighbors' (neighbors propagation and scalingFunction(parameters)), 'customPropagation' (custom scaling function and custom propagation function defined in src/PropagationModelCustom) ")
        ("propagationModelParameters", po::value<std::vector<double>>()->multitoken(),"(vector<double>) the parameters for the propagation model, for the scaled parameter the constant used to scale the conservation final results")
        ("propagationModelParameterFolder", po::value<std::string>(),"(string) the folder where the parameters for the propagation model are contained, each type can have a parameter file as a mapping of node->parameter(or parameters), if a file is missing for a type, than the parameters for that type will be 0, same can be said about nodes with no mapping. if not specified, the default parameters are used or the parameters in propagationModelParameters parameter are used")
        ("saturation",po::bool_switch(&saturation),"use saturation of values, default to 1, if another value is needed, use the saturationTerm")
        ("saturationTerm",po::value<double>(),"defines the limits of the saturation [-saturationTerm,saturationTerm], default to 1, if saturation is not set, this option is not used, if specified the program will stop the execution")
        ("customSaturationFunction",po::bool_switch(&customSaturation),"use custom saturation function defined in src/CustomFunctions.cxx, if this option is not set, the saturation function will be the default one")
        ("conservateInitialNorm",po::bool_switch(&conservateInitialNorm), "conservate the initial euclidean norm of the perturbation values, that is ||Pn|| <= ||Initial||, default to false")
        ("undirectedEdges",po::bool_switch(&undirected), "edges in the graphs are undirected")
        ("undirectedTypeEdges",po::bool_switch(&undirectedTypeEdges), "edges between types are undirected")
        ("resetVirtualOutputs",po::bool_switch(&resetVirtualOutputs), "reset the virtual outputs to 0 at each iteration, default to false")
        ("virtualNodesGranularity", po::value<std::string>(), "(string) granularity of the virtual nodes, available options are: 'type', 'node'(unstable), 'typeAndNode', default to type")
        ("virtualNodesGranularityParameters", po::value<std::vector<std::string>>()->multitoken(), "(vector<string>) parameters for the virtual nodes granularity, NOT USED for now")
        ("quantizationMethod",po::value<std::string>(), "(string) define the quantization method used to quantize the contact times for the edges between different types, available options are: 'single' and 'multiple'") // aggiungere documentazione
        ("loggingOptions",po::value<std::string>(&logMode),"(string) logging options, available options are: 'all','none'. Default to all")
        ("verbose",po::bool_switch(&verbose), "verbose mode, if set, the program will print more information during the execution(MPI information, communication information)")
        ("treatWarningAsError",po::bool_switch(&treatWarningAsError), "treat warnings as errors, if set, the program will throw an exception if a warning is encountered")
        ("savePerformance",po::value<std::string>(&performanceFilename), "(string) output performance (running time, number of total nodes, number of communities, number of total edges) to the defined file, if nothing is specified the performance are not saved")
        ("resumeCheckpoint",po::bool_switch(&resumeCheckpoint), "resume the computation from the last checkpoint, if the checkpoint is not found, the computation will start from the beginning")
        ("outputFormat",po::value<std::string>(), "(string) output format for the output files, available options are: 'singleIteration' (default) and 'iterationMatrix'. If one is chosen, the other won't generate the corrispondent output files")
        ("saveAugmentedNetworks",po::bool_switch(&saveAugmentedNetworks), "save the augmented networks for each iteration, default to false")
    ;

    

    po::variables_map vm; ///< variables map for program options
    // parse the command line arguments
    po::store(
        po::command_line_parser(argc, argv)    // use command_line_parser
        .options(desc)                     // add options_description
        .extra_style_parser(&ignore_numbers)  // intercept negatives
        .run(), vm                            // parse
    );
    // notify the variables map
    // this will throw an exception if there are any errors in the command line arguments
    po::notify(vm);
    std::string uniqueGraphFilename; ///< string variable to indicate the filename of the graph, in the case of a single graph for every type
    std::string subtypesFilename; ///< string variable to indicate the filename of the subtypes, where the list of subtypes to use during the computation is contained
    std::string typesInteractionFoldername; ///< string variable to indicate the folder where the types interactions are contained
    std::string typesInitialPerturbationMatrixFilename; ///< string variable to indicate the filename of the initial perturbation matrix, where the initial perturbation values are contained. WARNING: only use this option if the graph is unique for every type, otherwise use the initialPerturbationPerTypeFolder
    std::string graphsFilesFolder; ///< string variable to indicate the folder where the graphs edges files are contained
    std::string typeInitialPerturbationFolderFilename; ///< string variable to indicate the folder where the initial perturbation values for every type are contained
    std::string outputFoldername; ///< string variable to indicate the folder where the output files will be saved
    int intertypeIterations; ///< integer variable to indicate the number of iterations for the intertype communication
    int intratypeIterations; ///< integer variable to indicate the number of iterations for the intratype communication
    DissipationModel* dissipationModel = nullptr; ///< pointer to the dissipation model used for the computation. TODO change to a smart pointer(shared_ptr). Or change it to a list of dissipation models for future customization.
    ConservationModel* conservationModel = nullptr; ///< pointer to the conservation model used for the computation. TODO change to a smart pointer(shared_ptr). Or change it to a list of dissipation models for future customization.
    std::vector<DissipationModel*> dissipationModels = {}; ///< vector of pointers to the dissipation models used for the computation, in case of multiple dissipation models 
    std::vector<ConservationModel*> conservationModels = {}; ///< vector of pointers to the conservation models used for the computation, in case of multiple conservation models
    std::string dissipationModelParametersFolderName; ///< string variable to indicate the folder where the dissipation model parameters are contained, each type can have a parameter file as a mapping of node->parameter(or parameters), if a file is missing for a type, than the parameters for that type will be 0, same can be said about nodes with no mapping
    std::string conservationModelParametersFolderName; ///< string variable to indicate the folder where the conservation model parameters are contained, each type can have a parameter file as a mapping of node->parameter(or parameters), if a file is missing for a type, than the parameters for that type will be 0, same can be said about nodes with no mapping
    std::string propagationModelParametersFolderName; ///< string variable to indicate the folder where the propagation model parameters are contained, each type can have a parameter file as a mapping of node->parameter(or parameters), if a file is missing for a type, than the parameters for that type will be 0, same can be said about nodes with no mapping
    double timestep = 1; ///< double variable to indicate the timestep to use for the iteration, the final time is iterationIntercell*timestep. The time between to intracell iterations is the timestep divided by the number of intracell iterations.
    // final output matrices if the output format is set to iterationMatrix
    std::map<std::string,Matrix<double>*> outputMatrices; ///< map of the output matrices, where the key is the name of the type, and the value is the matrix of the output values(iteration as columns and nodes values as rows)
    std::map<std::string,std::vector<std::string>> outputMatricesRowNames; ///< map of the output matrices row names, where the key is the name of the type, and the value is the vector of the row names(node names for the selected type)


    // printing the help section if the help option is set, and return
    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    // control if no arguments are passed
    if(argc == 1){
        std::cerr << "[ERROR] no arguments passed, use --help to get the list of arguments available, aborting"<<std::endl;
        return 1;
    }

    // take starting time before initializing MPI and the computation
    auto start = std::chrono::steady_clock::now();
    // initialize MPI
    MPI_Init(&argc, &argv);

    int numProcesses, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);


    //logging options
    auto& logger = Logger::getInstance(); ///< logger object to log the messages
    if(vm.count("loggingOptions")){
        if(logMode == "all"){
            if(rank==0)std::cout << "[LOG] logging options set to all"<<std::endl;
            logger.enable();
        } else if (logMode == "none"){
            if(rank==0)std::cout << "[LOG] logging options set to none"<<std::endl;
            logger.disable();
        } else {
            if(rank==0)std::cout << "[LOG] logging options set to default (all)"<<std::endl;
            logger.enable();
        }
    } else {
        if(rank==0)std::cout << "[LOG] logging options set to default (all)"<<std::endl;
        logger.enable();
    }
    if(verbose){
        logger.enableVerbose();
        if(rank==0)logger.printLog(false,"verbose mode enabled");
    } else {
        logger.disableVerbose();
        if(rank==0)logger.printLog(false,"verbose mode disabled");
    }
    if(treatWarningAsError){
        if(rank==0)logger.printLog(false,"treat warnings as errors enabled");
        logger.setTreatWarningsAsErrors(true);
    } else {
        if(rank==0)logger.printLog(false,"treat warnings as errors disabled");
        logger.setTreatWarningsAsErrors(false);
    }


    // print the number of processes and the rank
    logger.printLog(true,"Number of processes: ", numProcesses,", rank: ", rank);


    //controls over impossible configurations
    if(vm.count("fUniqueGraph") == 0 && vm.count("graphsFilesFolder") == 0){
        //no unique graph of folder of the graphs was set
        if(rank==0)logger.printError("no unique graph filename or folder was set to get the graphs, set one. abort ") <<std::endl;
        return 1;
    }

    if(vm.count("fInitialPerturbationPerType") == 0 && vm.count("initialPerturbationPerTypeFolder") == 0){
        //no way of getting the initial perturbation values
        if(rank==0)logger.printError("no matrix for the initial values was passed as filename or single vector in files contained in the folder specified was set, set one ")<<std::endl;
        return 1;
    }

    if(vm.count("fInitialPerturbationPerType") && vm.count("graphsFilesFolder")){
        //unstable configuration of different graphs and single matrix with the same nodes
        if(rank==0)logger.printWarning("unstable configuration of different graphs and a single matrix with the initial perturbations") <<std::endl;
    }

    if(saturation && conservateInitialNorm){
        if(rank==0)logger.printError("saturation and conservateInitialNorm cannot be both true, aborting")<<std::endl;
        return 1;
    }


    if(vm.count("graphsFilesFolder") && vm.count("fUniqueGraph")){
        if(rank==0)logger.printError("fUniqueGraph and graphsFilesFolder were both set. Aborting");
        return 1;
    }
    if(vm.count("initialPerturbationPerTypeFolder") && vm.count("fInitialPerturbationPerType")){
        if(rank==0)logger.printError("fInitialPerturbationPerType and initialPerturbationPerTypeFolder were both set. Aborting");
        return 1;
    }

    if(!saturation){
        if (vm.count("saturationTerm")){
            if(rank==0)logger.printError("saturationTerm was set but saturation was not set, impossible configuration, aborting")<<std::endl;
            return 1;
        }
        if (customSaturation){
            if(rank==0)logger.printError("customSaturation was set but saturation was not set, impossible configuration, aborting")<<std::endl;
            return 1;
        }
    } 

    if(vm.count("dissipationModelParameters") && !vm.count("dissipationModel")){
        //dissipation model parameters were set but no dissipation model was set
        if(rank==0)logger.printError("dissipationModelParameters were set but no dissipationModel was set, aborting")<<std::endl;
        return 1;
    }
    if(vm.count("conservationModelParameters") && !vm.count("conservationModel")){
        //conservation model parameters were set but no conservation model was set
        if(rank==0)logger.printError("conservationModelParameters were set but no conservationModel was set, aborting")<<std::endl;
        return 1;
    }
    if(vm.count("propagationModelParameters") && !vm.count("propagationModel")){
        //propagation model parameters were set but no propagation model was set
        if(rank==0)logger.printError("propagationModelParameters were set but no propagationModel was set, aborting")<<std::endl;
        return 1;
    }

    if(vm.count("dissipationModelParameterFolder") && !vm.count("dissipationModel")){
        //dissipation model parameters folder was set but no dissipation model was set
        if(rank==0)logger.printError("dissipationModelParameterFolder was set but no dissipationModel was set, aborting")<<std::endl;
        return 1;
    }
    if(vm.count("conservationModelParameterFolder") && !vm.count("conservationModel")){
        //conservation model parameters folder was set but no conservation model was set
        if(rank==0)logger.printError("conservationModelParameterFolder was set but no conservationModel was set, aborting")<<std::endl;
        return 1;
    }
    if(vm.count("propagationModelParameterFolder") && !vm.count("propagationModel")){
        //propagation model parameters folder was set but no propagation model was set
        if(rank==0)logger.printError("propagationModelParameterFolder was set but no propagationModel was set, aborting")<<std::endl;
        return 1;
    }

    if(vm.count("dissipationModelParameterFolder") && vm.count("dissipationModelParameters")){
        //dissipation model parameters folder and parameters were both set
        if(rank==0)logger.printError("dissipationModelParameterFolder and dissipationModelParameters were both set, only one or 0(default configuration) can be set if dissipation is used, aborting")<<std::endl;
        return 1;
    }
    if(vm.count("conservationModelParameterFolder") && vm.count("conservationModelParameters")){
        //conservation model parameters folder and parameters were both set
        if(rank==0)logger.printError("conservationModelParameterFolder and conservationModelParameters were both set, only one or 0(default configuration) can be set if conservation is used, aborting")<<std::endl;
        return 1;
    }
    if(vm.count("propagationModelParameterFolder") && vm.count("propagationModelParameters")){
        //propagation model parameters folder and parameters were both set
        if(rank==0)logger.printError("propagationModelParameterFolder and propagationModelParameters were both set, only one or 0(default configuration) can be set if propagation is used, aborting")<<std::endl;
        return 1;
    }

    // control over the dissipation model, since the folder option can only be used with the custom dissipation model(custom dissipation supports both single parameters and parameters in the folder though)
    if(vm.count("dissipationModel") && vm["dissipationModel"].as<std::string>() != "custom" && vm.count("dissipationModelParameterFolder")){
        //dissipation model was set but the dissipation model is not custom
        if(rank==0)logger.printError("dissipationModel was set to a non-custom model, but dissipationModelParameterFolder was set, aborting")<<std::endl;
        return 1;
    }

    // reading the parameters

    if (vm.count("intertypeIterations")) {
        if(rank==0)logger << "[LOG] iterations intertype set to " 
    << vm["intertypeIterations"].as<int>() << ".\n";
        int testingIterations = vm["intertypeIterations"].as<int>();
        if(testingIterations <= 0){
            if(rank==0)logger.printError("intertypeIterations must be a positive value, aborting")<<std::endl;
            return 1;
        }
        intertypeIterations = testingIterations; // only assigned when positive
    } else {
        if(rank==0)logger << "[LOG] iterations intertype not set, set to default: 10 iterations \n";
        intertypeIterations = 10;
    }

    if (vm.count("intratypeIterations")) {
        if(rank==0)logger << "[LOG] iterations intratype set to " 
    << vm["intratypeIterations"].as<int>() << ".\n";
        int testingIterations = vm["intratypeIterations"].as<int>();
        if(testingIterations <= 0){
            if(rank==0)logger.printError("intratypeIterations must be a positive value, aborting")<<std::endl;
            return 1;
        }
        intratypeIterations = testingIterations;
    } else {
        if(rank==0)logger << "[LOG] iterations intratype not set, set to default: 5 iterations \n";
        intratypeIterations = 5;
    }

    if (vm.count("quantizationMethod")) {
        if(rank==0)logger << "[LOG] quantization method set to " << vm["quantizationMethod"].as<std::string>();
        if(vm["quantizationMethod"].as<std::string>() == "single"){
            quantizationMethod = "single";
        } else if(vm["quantizationMethod"].as<std::string>() == "multiple"){
            quantizationMethod = "multiple";
        } else {
            if(rank==0)logger.printError("quantizationMethod set to a not available option, aborting");
            return 1;            
        }
    }

    // reading granularity
    if(vm.count("virtualNodesGranularity")){
        virtualNodesGranularity = vm["virtualNodesGranularity"].as<std::string>();
        if(rank==0)logger << "[LOG] virtual nodes granularity set to " << virtualNodesGranularity << std::endl;
        // controls over the value
        if(virtualNodesGranularity != "type" && virtualNodesGranularity != "node" && virtualNodesGranularity != "typeAndNode"){
            if(rank==0)logger.printError("virtual nodes granularity must be one of the following: 'type', 'node' or 'typeAndNode': aborting")<<std::endl;
            return 1;
        }
        if(virtualNodesGranularity == "node"){
            if(rank==0)logger.printWarning("virtual nodes granularity set to 'node', this option is unstable and not fully implemented: aborting")<<std::endl;
            return 1;
        }
    } else {
        if(rank==0)logger << "[LOG] virtual nodes granularity not set, set to default: type \n";
        virtualNodesGranularity = "type";
    }

    // reading granularity parameters (not used for now)
    if(vm.count("virtualNodesGranularityParameters")){
        std::vector<std::string> virtualNodesGranularityParameters = vm["virtualNodesGranularityParameters"].as<std::vector<std::string>>();
        if(rank==0)logger << "[LOG] virtual nodes granularity parameters set "<< std::endl;
        if(rank==0)logger.printWarning("virtual nodes granularity parameters are not used for now")<<std::endl;
    } else {
        // logger << "[LOG] virtual nodes granularity parameters not set, set to default: empty vector \n";  // not used for now
    }

    //logging timestep settings
    if(vm.count("timestep")){
        if(rank==0)logger << "[LOG] timestep set to " 
            << vm["timestep"].as<double>() << ".\n";
        timestep = vm["timestep"].as<double>();
        // control over the sign of the value, since it should be positive
        if(timestep <= 0){
            if(rank==0)logger.printError("timestep must be a positive value, aborting")<<std::endl;
            return 1;
        }
    } else {
        if(rank==0)logger << "[LOG] timestep not set, set to default (1)"<<std::endl;
    }

    //logging edges direction in the graphs
    if(undirected){
        if(rank==0)logger << "[LOG] undirectedEdges specified, undirected edges in the graphs"<<std::endl;
    } else {
        if(rank==0)logger << "[LOG] undirectedEdges not specified, directed edges in the graphs(only the edges specified in the graph files will be added)"<<std::endl;
    }

    //logging edges direction in the graphs
    if(undirectedTypeEdges){
        if(rank==0)logger << "[LOG] undirectedTypeEdges specified, undirected edges between types"<<std::endl;
    } else {
        if(rank==0)logger << "[LOG] undirectedTypeEdges not specified, directed edges between types"<<std::endl;
    }

    //logging virtual nodes reset

    if(resetVirtualOutputs){
        if(rank==0)logger << "[LOG] resetVirtualOutputs specified, virtual outputs will be reset to 0 after each inter-propagation"<<std::endl;
    } else {
        if(rank==0)logger << "[LOG] resetVirtualOutputs not specified, virtual outputs will not be reset to 0 after each inter-propagation"<<std::endl;
    }

    // output format parameter
    if(vm.count("outputFormat")){
        outputFormat = vm["outputFormat"].as<std::string>();
        if(outputFormat != "singleIteration" && outputFormat != "iterationMatrix"){
            if(rank==0)logger.printError("outputFormat must be one of the following: 'singleIteration' or 'iterationMatrix': aborting")<<std::endl;
            return 1;
        }
    } else {
        if(rank==0)logger << "[LOG] outputFormat not set, set to default: singleIteration \n";
        outputFormat = "singleIteration";
    }


    if (vm.count("fUniqueGraph")) {
        if(rank==0)logger << "[LOG] file for the graph was set to " 
            << vm["fUniqueGraph"].as<std::string>() << ".\n";
        uniqueGraphFilename = vm["fUniqueGraph"].as<std::string>();
        if(!fileExistsPath(uniqueGraphFilename)){
            if(rank==0)logger.printError("file ")<< uniqueGraphFilename <<" for the graph do not exist: aborting"<<std::endl;
            return 1;
        }
    } else if(vm.count("graphsFilesFolder")){
        if(rank==0)logger << "[LOG] folder for the graphs was set to " 
            << vm["graphsFilesFolder"].as<std::string>() << ".\n";
        graphsFilesFolder = vm["graphsFilesFolder"].as<std::string>();
        if(!folderExists(graphsFilesFolder)){
            if(rank==0)logger.printError("folder ")<< graphsFilesFolder << " for the graphs do not exist: aborting"<<std::endl;
            return 1;
        }
    }
    if (vm.count("fInitialPerturbationPerType")) {
        if(rank==0)logger << "[LOG] file for the initialPerturbationPerType matrix was set to " 
            << vm["fInitialPerturbationPerType"].as<std::string>() << ".\n";
        typesInitialPerturbationMatrixFilename = vm["fInitialPerturbationPerType"].as<std::string>();
        if(!fileExistsPath(typesInitialPerturbationMatrixFilename)){
            if(rank==0)logger.printError("file ")<< typesInitialPerturbationMatrixFilename << " for the initialPerturbationPerType does not exist: aborting"<<std::endl;
            return 1;
        }
    } else if (vm.count("initialPerturbationPerTypeFolder")) {
        if(rank==0)logger << "[LOG] folder for the initialPerturbationPerType was set to "
            << vm["initialPerturbationPerTypeFolder"].as<std::string>() << ".\n";
        typeInitialPerturbationFolderFilename = vm["initialPerturbationPerTypeFolder"].as<std::string>();
        if(!folderExists(typeInitialPerturbationFolderFilename)){
            if(rank==0)logger.printError("folder ")<< typeInitialPerturbationFolderFilename << " for the initialPerturbationPerType do not exist: aborting"<<std::endl;
            return 1;
        }
    }

    if (vm.count("typeInteractionFolder")) {
        if(rank==0)logger << "[LOG] folder for the type interactions was set to " 
            << vm["typeInteractionFolder"].as<std::string>() << ".\n";
        typesInteractionFoldername = vm["typeInteractionFolder"].as<std::string>();
        if(!folderExists(typesInteractionFoldername)){
            if(rank==0)logger.printError("folder ")<< typesInteractionFoldername << " for the type interactions do not exist: aborting"<<std::endl;
            return 1;
        }
    } else {
        if(rank==0)logger << "[LOG] typeInteractionFolder folder was not set. computing without taking into account type interactions\n";
    }
    if (vm.count("outputFolder")) {
        if(rank==0)logger << "[LOG] output folder  was set to " 
            << vm["outputFolder"].as<std::string>() << ".\n";
        outputFoldername = vm["outputFolder"].as<std::string>();
        if(!folderExists(outputFoldername)){
            if(rank==0)logger.printWarning("folder for the output do not exist: creating the folder")<<std::endl;
            if(!createFolder(outputFoldername)){
                logger.printError("folder for the output could not be created: aborting")<<std::endl;
                return 1;
            }
        }
    } else {
        if(rank==0)logger.printError("output folder was not set. aborting");
        return 1;
    }

    // create output folder for the current perturbations, if the output format is set to singleIteration
    if(outputFormat == "singleIteration"){
        std::string outputFolderNameSingular = outputFoldername + "/currentPerturbations";
        if(!folderExists(outputFolderNameSingular)){
            if(rank==0)logger.printWarning("folder for the output of singular perturbance values do not exist: creating the folder")<<std::endl;
            if(!createFolder(outputFolderNameSingular)){
                if(rank==0)logger.printError("folder for the output of singular perturbance values could not be created: aborting")<<std::endl;
                return 1;
            }
        }
    }

    // create output folder if the output format is iterationMatrix
    if(outputFormat == "iterationMatrix"){
        std::string outputFolderNameIterationMatrix = outputFoldername + "/iterationMatrices";
        if(!folderExists(outputFolderNameIterationMatrix)){
            if(rank==0)logger.printWarning("folder for the output of iteration matrix do not exist: creating the folder")<<std::endl;
            if(!createFolder(outputFolderNameIterationMatrix)){
                if(rank==0)logger.printError("folder for the output of iteration matrix could not be created: aborting")<<std::endl;
                return 1;
            }
        }
    }



    //logging if saturation is set and saturation parameters are set
    if (saturation) {
        if(vm.count("saturationTerm") == 0){
            if(rank==0)logger << "[LOG] saturation term not specified, using the interval [-1,1]"<<std::endl;
        } else if(vm.count("saturationTerm") == 1){
            double saturationTerm = vm["saturationTerm"].as<double>();
            if(rank==0)logger << "[LOG] saturation term specified, using the interval [-" << saturationTerm << "," << saturationTerm << "]"<<std::endl;
        } else {
            if(rank==0)logger.printError("saturation term specified more than once, possibility of using more values not yet implemented: aborting")<<std::endl;
            return 1;
        }
    }

    // Checkpoint initialization
    Checkpoint checkpoint;
    //end program options section

    std::string nodesDescriptionFilename="";
    if(vm.count("nodeDescriptionFile")){
        if(rank==0)logger <<"[LOG] using node description file to get the names of the nodes in the graphs"<<std::endl;
        nodesDescriptionFilename = vm["nodeDescriptionFile"].as<std::string>();
    } else {
        if(rank==0)logger <<"[LOG] no nodes description"<<std::endl;
    }

    // if both homogenousGraphNodesFile and nodeDescriptionFolder are set, exit
    if(vm.count("homogenousGraphNodesFile") && vm.count("nodeDescriptionFolder")){
        if(rank==0)logger.printError("homogenousGraphNodesFile and nodeDescriptionFolder were both set, only one can be used when using an homogeneous configuration for the agent or with different structured agents. Aborting");
        return 1;
    }
    std::string nodesDescriptionFolder="";
    if(vm.count("nodeDescriptionFolder")){
        if(rank==0)logger <<"[LOG] using node description folder to get the names of the nodes in the graphs"<<std::endl;
        nodesDescriptionFolder = vm["nodeDescriptionFolder"].as<std::string>();
    } else {
        if(rank==0)logger <<"[LOG] no nodes description folder"<<std::endl;
    }


    // these types will be used for indexing the single processes, rank 0 is the master process, rank 1 will get the first type, rank 2 the second and so on
    std::vector<std::string> types;
    if(vm.count("fUniqueGraph")){
        if(vm.count("fInitialPerturbationPerType")){
            types = getTypesFromMatrixFile(typesInitialPerturbationMatrixFilename);

        } else if (vm.count("initialPerturbationPerTypeFolder")){
            types = getTypesFromFolderFileNames(typeInitialPerturbationFolderFilename);
        } else {
            if(rank==0)logger.printError("no initial perturbation file or folder specified: aborting")<<std::endl;
            return 1;
        }
    } else if (vm.count("graphsFilesFolder")) {
        types = getTypesFromFolderFileNames(graphsFilesFolder);
    } else {
        if(rank==0)logger.printError("no graph file or folder specified: aborting")<<std::endl;
        return 1;
    }

    std::vector<std::string> subtypes;
    if(vm.count("subtypes")){
        if(rank==0)logger << "[LOG] subtypes filename set to "
            << vm["subtypes"].as<std::string>() << ".\n";
        subtypesFilename = vm["subtypes"].as<std::string>();
        subtypes = getVectorFromFile<std::string>(subtypesFilename);
    }else{
        if(rank==0)logger << "[LOG] subtypes filename not set, set to default: all types \n";
        subtypes = types;
    }

    if(rank==0){
        logger << "[LOG] types found in the folder used for the computation: " << std::endl;
        for (auto type : types) {
            logger << type << ", ";
        }
        logger << std::endl;
    }
    
    //filter types with the subtypesresetVirtualOutputs
    types = vectorsIntersection(types, subtypes);
    if (types.size() == 0) {
        if(rank==0)logger.printError("no types in common between the types and subtypes: aborting")<<std::endl;
        return 1;
    }
    if(rank==0){
        logger << "[LOG] final types used for the computation: " << std::endl;
        for (auto type : types) {
            logger << type << ", ";
        }
        logger << std::endl;
    }

    // control if the number of processes is greater than the number of types, exit if true (useless process are not accepted)
    if (numProcesses > SizeToInt(types.size())) {
        if(rank==0){
            logger.printError("number of processes is greater than the number of types: aborting")<<std::endl;
            logger.printError("number of processes: ") << numProcesses << std::endl;
            logger.printError("number of types: ") << types.size() << std::endl;
        }
        return 1;
    }

    // workloads for each process, 
    int workloadPerProcess = round((types.size() + 0.0) / numProcesses); // one process minimum for each type, if number of processes = number of types then one process for each type, rounded to account for close number of types and processes(almost 1 process for every type) and to not leave a process without work
    int startIdx = rank * workloadPerProcess;
    int endIdx = (rank == numProcesses - 1) ? types.size() : (rank + 1) * workloadPerProcess;
    
    int finalWorkload = endIdx - startIdx;

    //map types to rank
    std::map<std::string, int> typeToRank;
    for (int i = 0; i < SizeToInt(types.size()); ++i) {
        int rankType;
        if (i >= (numProcesses - 1)*workloadPerProcess) {
            rankType = numProcesses - 1;
        } else {
            rankType = i / workloadPerProcess;
        }
        typeToRank[types[i]] = rankType;
    }

    //use the number of types for workload to allocate an array of pointers to contain the graph for each type
    WeightedEdgeGraph **graphs = new WeightedEdgeGraph*[finalWorkload];
    std::vector<std::string> typesFromFolder;
    std::vector<std::vector<std::string>> graphsNodes;
    std::vector<std::vector<std::string>> graphsNodesAll; // used only in the setup phase to read the initial input values, contains all types
    std::unordered_map<std::string, std::vector<std::string>> typeToNodeNamesMap; // map from all types to the node names, not only the ones in the workload, no virtual nodes
    std::vector<std::pair<std::vector<std::string>,std::vector<std::tuple<std::string,std::string,double>>>> namesAndEdges;
    // a single graph is used for all the types in case it is specified by the parameter
    if(vm.count("fUniqueGraph")){
        namesAndEdges.push_back(edgesFileToEdgesListAndNodesByName(uniqueGraphFilename));
        graphsNodes.push_back(namesAndEdges[0].first);
        graphs[0] = new WeightedEdgeGraph(graphsNodes[0]);
        for(int i = 1; i < finalWorkload; i++){
            namesAndEdges.push_back(namesAndEdges[0]);
            graphsNodes.push_back(namesAndEdges[0].first);
            graphs[i] = graphs[0];
        }
        // TODO only use the single graph for the setup phase, without filling all the maps for all the other types, optimize the memory usage
        for (uint i = 0; i<types.size(); i++){
            graphsNodesAll.push_back(namesAndEdges[0].first);
        }
            
        // create the map also for the use case of a single graph, TODO only use the map for the single graph case without filling all the maps
        for(uint i = 0; i < types.size(); i++){
            typeToNodeNamesMap[types[i]] = namesAndEdges[0].first;
        }

    } else if (vm.count("graphsFilesFolder")) { // the graphs are in a folder, each graph is a type
        auto allGraphs = edgesFileToEdgesListAndNodesByNameFromFolder(graphsFilesFolder);  // TODO change the function to return only the set of edges and type names, use another function to get the nodes
        
        // control if the types from the edges folder and the types from the values match
        typesFromFolder = allGraphs.first;
        auto typesFromFolderFiltered = vectorsIntersection(typesFromFolder, subtypes);
        if(typesFromFolderFiltered.size() != types.size()){
            if(rank == 0) { // only the master process prints the error
                logger.printError("types from folder (filtered with subtypes) and types from values do not have the same length: aborting") << std::endl;
            }
            return 1;
        }
        for (uint i = 0; i<typesFromFolderFiltered.size(); i++){ //TODO map the types from the folder to the types from the file
            if(typesFromFolderFiltered[i] != types[i]){  // TODO this control can be faulty, since the order of the types is not guaranteed when reading the files
                if(rank == 0) { // only the master process prints the error
                    logger.printError("types from folder(filtered with subtypes) and types from file do not match: aborting") << std::endl;
                }
                return 1;
            }
        }
        namesAndEdges = allGraphs.second;
        
        std::map<std::string, std::vector<std::string>> namesFromFolder;
        if(nodesDescriptionFolder != ""){        
            namesFromFolder = nodeNamesFromFolder(nodesDescriptionFolder);
            auto typesFromNames = getKeys<std::string,std::vector<std::string>>(namesFromFolder);
            auto typesFromNamesFiltered = vectorsIntersection(typesFromNames, subtypes);
            if(typesFromNamesFiltered.size() != types.size()){ //TODO change the control over the types read from the graph, since the values can be not expressed for some graphs
                if(rank == 0) { // only the master process prints the error
                    logger.printError("types from values(initial perturbation, filtered by subtypes) and types from file do not match: aborting") << std::endl;
                }
                return 1;
            }
            // control over the values and the order is useless since the map is unordered and doesn't guarantee the order of reading the files
            // for(uint i = 0; i < typesFromNames.size(); i++){
            //     if(typesFromNames[i] != types[i]){
            //         logger.printError("types from values(inital perturbation) and types from file do not match: aborting"<<std::endl;
            //         return 1;
            //     }
            // }
        } else {
            for(uint i = 0; i < types.size(); i++){
                //graphsNodesAll.push_back(namesAndEdges[i].first);
                int namesAndEdgesIdx = 0;
                for(int tmpidx=0; tmpidx<SizeToInt(typesFromFolder.size()); tmpidx++){
                    if(typesFromFolder[tmpidx] == types[i]){
                        namesAndEdgesIdx = tmpidx;
                        break;
                    }
                }

                namesFromFolder[types[i]] = namesAndEdges[namesAndEdgesIdx].first;
            }
        }

        // create the graphs and the map for the nodes
        for(uint i = 0; i < types.size(); i++){
            std::string tmpType = types[i];
            graphsNodesAll.push_back(namesFromFolder[tmpType]);
            auto tmpNodeNames = namesFromFolder[tmpType];
            
            typeToNodeNamesMap[tmpType] = tmpNodeNames;
        }
        for(int i = startIdx; i < endIdx; i++){
            //graphsNodes.push_back(namesAndEdges[i].first);
            graphsNodes.push_back(namesFromFolder[types[i]]);
            graphs[i-startIdx] = new WeightedEdgeGraph(graphsNodes[i-startIdx]);
        }
    } 

    //add the edges to the graphs
    if(vm.count("fUniqueGraph")){
        for(auto edge = namesAndEdges[0].second.cbegin() ; edge != namesAndEdges[0].second.cend(); edge++ ){
            graphs[0]->addEdge(std::get<0> (*edge), std::get<1> (*edge) ,std::get<2>(*edge) ,!undirected);
        }
    } else if (vm.count("graphsFilesFolder")) {
        for(int i = startIdx; i < endIdx; i++){
            int namesAndEdgesIdx = 0;
            for(int tmpidx=0; tmpidx<SizeToInt(typesFromFolder.size()); tmpidx++){
                if(typesFromFolder[tmpidx] == types[i]){
                    namesAndEdgesIdx = tmpidx;
                    break;
                }
            }
            for(auto edge = namesAndEdges[namesAndEdgesIdx].second.cbegin() ; edge != namesAndEdges[namesAndEdgesIdx].second.cend(); edge++ ){
                graphs[i-startIdx]->addEdge(std::get<0> (*edge), std::get<1> (*edge) ,std::get<2>(*edge) ,!undirected);
            }
        }
    }

    //get initial input values
    std::tuple<std::vector<std::string>, std::vector<std::string>, std::vector<std::vector<double>>> initialValues;
    std::vector<std::vector<double>> inputInitials;
    if(vm.count("fInitialPerturbationPerType")){
        if(rank==0)
            logger << "[LOG] initial perturbation per type specified, using the file "<<typesInitialPerturbationMatrixFilename<<std::endl;
        initialValues = valuesMatrixToTypeVectors(typesInitialPerturbationMatrixFilename,graphsNodes[0],subtypes);
    } else if (vm.count("initialPerturbationPerTypeFolder")){
        if(rank==0) logger << "[LOG] initial perturbation per type specified, using the folder "<<typeInitialPerturbationFolderFilename<<std::endl;
        initialValues = valuesVectorsFromFolder(typeInitialPerturbationFolderFilename,types,graphsNodesAll,subtypes); // TODO change the function to return only the values for the types in the workload
    } else {
        if(rank==0) logger.printError("no initial perturbation file or folder specified: aborting")<<std::endl;
        return 1;
    }
    logger.printLog(true, "perturbation values successfully read for rank ", rank );
    std::vector<std::string> initialNames = std::get<0>(initialValues);
    inputInitials = std::get<2>(initialValues);
    std::vector<std::string> typesFromValues = std::get<1>(initialValues);
    //this condition should take into account the intersection of the types and the subtypes
    if(typesFromValues.size() == 0){
        if(rank==0){
            logger.printError("types from the initial values folder are 0, control if the types are the same to the one specified in the matrix, in the graphs folder and in the subtypes: aborting")<<std::endl;
            logger.printError("types specified(subtypes): ");
            for(auto type: subtypes)
                std::cerr << type << " "; //TODO handle logger with these
            std::cerr << std::endl;
            logger.printError("types from file(from graphs folder or from matrix): ");
            for(auto type: types)
                std::cerr << type << " ";
            std::cerr << std::endl;
            logger.printError("types from values(from initial values folder or from values matrix) intersected with subtypes: ");
            for(auto type: typesFromValues)
                std::cerr << type << " ";
            std::cerr << std::endl;
        }
        return 1;
    }
    auto indexMapGraphTypesToValuesTypes = get_indexmap_vector_values_full(types, typesFromValues);
    if(indexMapGraphTypesToValuesTypes.size() == 0){
        if(rank==0)logger.printError("types from folder and types from file do not match even on one instance: aborting")<<std::endl;
        return 1;
    }

    // TODO get rid of the augment graph function and use the addition of nodes or edges directly
    Computation** typeComputations = new Computation*[finalWorkload];
    // I am not convinced what this variable does anymore, 
    // maybe in the past it was used to store the index of the type in the types vector when considering subtypes
    // but now it's just a useless vector that goes from 0 to finalWorkload-1, not even a -1 is inside for sure, since subtyping is already done before this point  
    for(int i = 0; i < finalWorkload; i++){
        if(indexMapGraphTypesToValuesTypes[i+startIdx] == -1){
            logger.printLog(true, "type ", types[i+startIdx], " not found in the initial perturbation files, using zero vector as input");
            std::vector<double> input = std::vector<double>(graphsNodes[i].size(),0);
            Computation* tmpCompPointer = new Computation(types[i+startIdx],input,graphs[i],graphsNodes[i]);   
            // tmpCompPointer->setDissipationModel(dissipationModel);
            // tmpCompPointer->setConservationModel(conservationModel);
            // tmpCompPointer->setDissipationModel(dissipationModels[i]);
            // tmpCompPointer->setConservationModel(conservationModels[i]);
            typeComputations[i] = tmpCompPointer;
            //No inverse computation with the augmented graph since virtual nodes edges are not yet inserted
            // TODO generalize by removing the type granularity in this code, that is by considering only the types that are encoded?
            if(virtualNodesGranularity == "type"){
                typeComputations[i]->augmentGraphNoComputeInverse(types,std::vector<std::pair<std::string,std::string>>(),std::vector<double>(), true); //self included since the code in MPI needs it
            } else if (virtualNodesGranularity == "typeAndNode"){
                typeComputations[i]->augmentGraphNoComputeInverse(std::vector<std::string>(), std::vector<std::pair<std::string,std::string>>(), std::vector<double>(), false); //no types are passed since the virtual nodes will be added to the graph in the interaction section of this code
            }
        } else {
            int index = indexMapGraphTypesToValuesTypes[i+startIdx];
            std::vector<double> input = inputInitials[index];
            Computation* tmpCompPointer = new Computation(types[i+startIdx],input,graphs[i],graphsNodes[i]); 
            // tmpCompPointer->setDissipationModel(dissipationModel);
            // tmpCompPointer->setConservationModel(conservationModel);
            // tmpCompPointer->setDissipationModel(dissipationModels[i]);
            // tmpCompPointer->setConservationModel(conservationModels[i]);
            typeComputations[i] = tmpCompPointer;
            //No inverse computation with the augmented graph since virtual nodes edges are not yet inserted
            if(virtualNodesGranularity == "type"){
                typeComputations[i]->augmentGraphNoComputeInverse(types,std::vector<std::pair<std::string,std::string>>(),std::vector<double>(), true); //self included since the code in MPI needs it
            } else if (virtualNodesGranularity == "typeAndNode"){
                typeComputations[i]->augmentGraphNoComputeInverse(std::vector<std::string>(), std::vector<std::pair<std::string,std::string>>(), std::vector<double>(), false); //no types are passed since the virtual nodes will be added to the graph in the interaction section of this code
            }
        }

    }

    // saturation function for the computation is changed if custom saturation is set
    if(saturation && customSaturation){
        if(rank==0)logger << "[LOG] custom saturation function set, using the custom saturation function defined in src/CustomFunctions.cxx"<<std::endl;
        for(int i = 0; i < finalWorkload;i++){
            typeComputations[i]->setSaturationFunction(getSaturationFunction());
        }
    } else {
        if(rank==0)logger << "[LOG] custom saturation function not set, using the default saturation function"<<std::endl;
    }

    logger << "[LOG] loading type interactions for rank "<< rank << std::endl;
    // TODO test for multiple interaction between two edges in two files
    auto allFilesInteraction = get_all(typesInteractionFoldername,".tsv");
    // define the map for the type interactions, an hash function should be defined for the pair of strings used as the identifier of the interaction
    std::unordered_map<std::pair<std::string, std::string>, std::set<double>, hash_pair_strings> interactionBetweenTypesMap;
    std::unordered_map<std::tuple<std::string, std::string, std::string, std::string>, std::set<double>, hash_quadruple_strings> interactionBetweenTypesFinerMap;
    for(auto typeInteractionFilename = allFilesInteraction.cbegin() ; typeInteractionFilename != allFilesInteraction.cend() ; typeInteractionFilename++){
        std::pair<std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>>,std::vector<std::tuple<std::string, std::string, std::string, std::string, std::set<double>,double>>> typeInteractionsEdges;
        if (subtypes.size() == 0) {
            // TODO add different contact times inside the network (quite difficult since the structure of the graphs is static)
            // the above can be implemented with the use of different matrices for every single type(from 1 to max(contactTimes)), where the matrix represent the current state of the network
            // another possibility is to use two matrices for every type-agent, one for the whole network without contact times and one is used to store the current network state at iteration i 
            // SOLUTION: granularity
            typeInteractionsEdges  = interactionContinuousContactsFileToEdgesListAndNodesByName(*typeInteractionFilename, types, intertypeIterations*timestep, virtualNodesGranularity, typeToNodeNamesMap, undirectedTypeEdges);
        } else {
            typeInteractionsEdges = interactionContinuousContactsFileToEdgesListAndNodesByName(*typeInteractionFilename, subtypes, intertypeIterations*timestep, virtualNodesGranularity, typeToNodeNamesMap, undirectedTypeEdges);
        }
        #pragma omp parallel for
        for (int i = 0; i < finalWorkload;i++) {
            if(typeInteractionsEdges.first.contains(types[i+startIdx])){
                // granularity is already considered in the function that reads from the file previously called
                typeComputations[i]->addEdgesAndNodes(typeInteractionsEdges.first[types[i+startIdx]], false, false); // no inverse computation since it is done in the propagation model
            }
        }
        for(auto edge = typeInteractionsEdges.second.cbegin() ; edge != typeInteractionsEdges.second.cend(); edge++ ){
            // first two types are the nodes in the two networks/types ,types are the third and fourth element of the tuple, while the fifth is the set of contact times
            // startNodeName, endNodeName, startType, endType, contactTimes, weight
            std::pair<std::string,std::string> keyTypes = std::make_pair(std::get<2> (*edge), std::get<3> (*edge));
            std::tuple<std::string,std::string,std::string,std::string> keyTypesFiner = std::make_tuple(std::get<0> (*edge), std::get<1> (*edge), std::get<2> (*edge), std::get<3> (*edge));
            
            if(interactionBetweenTypesMap.contains(keyTypes)){
                interactionBetweenTypesMap[keyTypes].insert(std::get<4>(*edge).begin(),std::get<4>(*edge).end()); // directly inserting means the union of the two sets
            } else {
                interactionBetweenTypesMap[keyTypes] = std::get<4>(*edge);
            }
            
            if(interactionBetweenTypesFinerMap.contains(keyTypesFiner)){
                interactionBetweenTypesFinerMap[keyTypesFiner].insert(std::get<4>(*edge).begin(),std::get<4>(*edge).end()); // directly inserting means the union of the two sets
            } else {
                interactionBetweenTypesFinerMap[keyTypesFiner] = std::get<4>(*edge);
            }
            
        }
    }

    // conservation model initialization from command line options, since the number of types is not known before, we will be reading the conservation model now instead of when we were reading the parameters
    conservationModels = std::vector<ConservationModel*>(finalWorkload, nullptr); ///< vector of conservation models for each type, initialized to nullptr
    if (vm.count("conservationModel")) {
        if(rank==0)logger << "[LOG] conservation model was set to "
            << vm["conservationModel"].as<std::string>() << ".\n";
        std::string conservationModelName = vm["conservationModel"].as<std::string>();
        if(conservationModelName == "none"){
            if(rank==0)logger << "[LOG] conservation model set to default (none)\n";
            conservationModel = new ConservationModel([](double time)->double{return 0;});
            for(int i = 0; i < finalWorkload; ++i){
                conservationModels[i] = new ConservationModel([](double time)->double{return 0;}); // all processes will use the same conservation model
            }
        } else if (conservationModelName == "scaled"){
            if (vm.count("conservationModelParameters")) {
                if(rank==0)logger << "[LOG] conservation model parameters were declared to be "
            << vm["conservationModelParameters"].as<std::vector<double>>()[0] << ".\n";
                std::vector<double> conservationModelParameters = vm["conservationModelParameters"].as<std::vector<double>>();
                if(conservationModelParameters.size() == 1){
                    conservationModel = new ConservationModel([conservationModelParameters](double time)->double{return conservationModelParameters[0];});
                    for(int i = 0; i < finalWorkload; ++i){
                        conservationModels[i] = new ConservationModel([conservationModelParameters](double time)->double{return conservationModelParameters[0];}); // all processes will use the same conservation model
                    }
                } else {
                    if(rank==0)logger.printError("conservation model parameters for scaled conservation must be one parameter: aborting")<<std::endl;
                    return 1;
                }
            } else {
                if(rank==0)logger.printError("conservation model parameters for scaled conservation was not set: setting to default 0.5 costant")<<std::endl;
                conservationModel = new ConservationModel();
                for(int i = 0; i < finalWorkload; ++i){
                    conservationModels[i] = new ConservationModel(); // all processes will use the same conservation model
                }
            }
        } else if (conservationModelName == "random"){
            if(rank==0)logger << "[LOG] conservation model was set to random, the function will be a random number between two values defined in the parameters" << std::endl;
            if (vm.count("conservationModelParameters")) {
                if(rank==0)logger << "[LOG] conservation model parameters were declared to be "
                    << vm["conservationModelParameters"].as<std::vector<double>>()[0] << " & " << vm["conservationModelParameters"].as<std::vector<double>>()[1] << ".\n";
                std::vector<double> conservationModelParameters = vm["conservationModelParameters"].as<std::vector<double>>();
                if(conservationModelParameters.size() == 2){
                    //control if lower and upper limits of the random values are within 0 and 1
                    if( (conservationModelParameters[0] < 0) || (conservationModelParameters[0] > 1) || (conservationModelParameters[1] < 0) || (conservationModelParameters[1] > 1) || (conservationModelParameters[0] > conservationModelParameters[1]) ){
                        if(rank==0)logger.printError("conservation model parameters for random conservation must be between 0 and 1 and must be a < b: aborting")<<std::endl;
                        return 1;
                    }
                    conservationModel = new ConservationModel([conservationModelParameters](double time)->double{return randomRealNumber(conservationModelParameters[0],conservationModelParameters[1]);});
                    for(int i = 0; i < finalWorkload; ++i){
                        conservationModels[i] = new ConservationModel([conservationModelParameters](double time)->double{return randomRealNumber(conservationModelParameters[0],conservationModelParameters[1]);}); // all processes will use the same conservation model
                    }
                } else {
                    if(rank==0)logger.printError("conservation model parameters for random conservation must be two: aborting")<<std::endl;
                    return 1;
                }
            } else {
                if(rank==0)logger.printError("conservation model parameters for random conservation was not set: aborting")<<std::endl;
                return 1;
            }
        } else if(conservationModelName == "custom"){
            //control if custom function for conservation returns double and takes a single parameter as double
            if(rank==0)logger << "[LOG] conservation model was set to custom, if the custom function defined for scaling is not correctly implemented, there will be errors" << std::endl;
            if (vm.count("conservationModelParameters")) {
                std::vector<double> conservationModelParameters = vm["conservationModelParameters"].as<std::vector<double>>();
                if(rank==0){
                    logger << "[LOG] conservation model parameters were declared to be: ("; // this section can bring problems with the stream and concurrency
                    for (auto param : conservationModelParameters) {
                        logger << param << ", ";
                    }
                    logger << ")" << std::endl;
                }
                conservationModel = new ConservationModel(getConservationScalingFunction(conservationModelParameters));
                for(int i = 0; i < finalWorkload; ++i){
                    conservationModels[i] = new ConservationModel(getConservationScalingFunction(conservationModelParameters)); // all processes will use the same conservation model
                }
            } else if(vm.count("conservationModelParameterFolder")){
                if(rank==0)logger << "[LOG] conservation model parameters were declared to be in the folder "<<vm["conservationModelParameterFolder"].as<std::string>()<<std::endl;
                std::string conservationModelParametersFolder = vm["conservationModelParameterFolder"].as<std::string>();
                std::map<std::string, std::vector<std::string>> typeToOrderedNodeNames;
                for(int i = 0; i < finalWorkload; ++i){
                    typeToOrderedNodeNames[types[i+startIdx]] = typeComputations[i]->getAugmentedGraph()->getNodeNames();
                }
                auto conservationModelScalingFunctions = conservationScalingFunctionsFromFolder(conservationModelParametersFolder,typeToOrderedNodeNames);
                for(int i = 0; i < finalWorkload; ++i){
                    conservationModels[i] = new ConservationModel(conservationModelScalingFunctions[types[i+startIdx]]); // all processes will use the same dissipation model with different parameters
                }
            } else {
                if(rank==0)logger << "[LOG] conservation model parameters were not set, using the default scaling function (defined in the custom functions)" << std::endl;
                conservationModel = new ConservationModel(getConservationScalingFunction());
                for(int i = 0; i < finalWorkload; ++i){
                    conservationModels[i] = new ConservationModel(getConservationScalingFunction()); // all processes will use the same conservation model
                }
            }
        } else {
            if(rank==0)logger.printError("conservation model scale function is not any of the types. Conservation model scale functions available are none(default), scaled, random and custom");
            return 1;
        }
    } else {
        if(rank==0)logger << "[LOG] conservation model was not set. set to default (none)"<<std::endl;
        conservationModel = new ConservationModel([](double time)->double{return 0;});
        for(int i = 0; i < finalWorkload; ++i){
            conservationModels[i] = new ConservationModel([](double time)->double{return 0;}); // all processes will use the same conservation model
        }
    }

    // dissipation model initialization from command line options, same situation as for the conservation model, the number of types is not known before
    dissipationModels = std::vector<DissipationModel*>(finalWorkload, nullptr); ///< vector of dissipation models for each type, initialized to nullptr
    if (vm.count("dissipationModel")) {
        if(rank==0)logger << "[LOG] dissipation model was set to "
            << vm["dissipationModel"].as<std::string>() << ".\n";
        std::string dissipationModelName = vm["dissipationModel"].as<std::string>();
        if(dissipationModelName == "none"){
            if(rank==0)logger << "[LOG] dissipation model set to default (none)\n";
            dissipationModel = new DissipationModelScaled([](double time)->double{return 0;});
            for(int i = 0; i < finalWorkload; ++i){
                dissipationModels[i] = new DissipationModelScaled([](double time)->double{return 0;}); // all processes will use the same dissipation model
            }
        } else if(dissipationModelName == "power"){
            if (vm.count("dissipationModelParameters")) {
                if(rank==0)logger << "[LOG] dissipation model parameters for power dissipation were declared to be" << vm["dissipationModelParameters"].as<std::vector<double>>()[0] << ".\n";
                std::vector<double> dissipationModelParameters = vm["dissipationModelParameters"].as<std::vector<double>>();
                if(dissipationModelParameters.size() == 1){
                    dissipationModel = new DissipationModelPow(dissipationModelParameters[0]);
                    for(int i = 0; i < finalWorkload; ++i){
                        dissipationModels[i] = new DissipationModelPow(dissipationModelParameters[0]); // all processes will use the same dissipation model
                    }
                } else {
                    if(rank==0)logger.printError("dissipation model parameters for power dissipation must be one: aborting")<<std::endl;
                    return 1;
                }
            } else {
                if(rank==0)logger.printError("dissipation model parameters for power dissipation was not set: setting to default (2)")<<std::endl;
                dissipationModel = new DissipationModelPow(2);
                for(int i = 0; i < finalWorkload; ++i){
                    dissipationModels[i] = new DissipationModelPow(2); // all processes will use the same dissipation model
                }
            }
        } else if(dissipationModelName == "random"){
            if (vm.count("dissipationModelParameters")) {
                if(rank==0)logger << "[LOG] dissipation model parameters were declared to be "
                    << vm["dissipationModelParameters"].as<std::vector<double>>()[0] << " & " << vm["dissipationModelParameters"].as<std::vector<double>>()[1] << ".\n";
                std::vector<double> dissipationModelParameters = vm["dissipationModelParameters"].as<std::vector<double>>();
                if(dissipationModelParameters.size() == 2){
                    dissipationModel = new DissipationModelRandom(dissipationModelParameters[0],dissipationModelParameters[1]);
                    for(int i = 0; i < finalWorkload; ++i){
                        dissipationModels[i] = new DissipationModelRandom(dissipationModelParameters[0],dissipationModelParameters[1]); // all processes will use the same dissipation model
                    }
                } else {
                    if(rank==0)logger.printError("dissipation model parameters for random dissipation must be two: aborting")<<std::endl;
                    return 1;
                }
            } else {
                if(rank==0)logger.printError("dissipation model parameters for random dissipation was not set: aborting")<<std::endl;
                return 1;
            }
        } else if(dissipationModelName == "scaled"){
            if(rank==0)logger << "[LOG] dissipation model was set to scaled, the function will be a constant function with the value of the parameter" << std::endl;
            if (vm.count("dissipationModelParameters")) {
                if(rank==0)logger << "[LOG] dissipation model parameters were declared to be "
                    << vm["dissipationModelParameters"].as<std::vector<double>>()[0] << ".\n";
                std::vector<double> dissipationModelParameters = vm["dissipationModelParameters"].as<std::vector<double>>();
                if(dissipationModelParameters.size() == 1){
                    dissipationModel = new DissipationModelScaled([dissipationModelParameters](double time)->double{return dissipationModelParameters[0];});
                    for(int i = 0; i < finalWorkload; ++i){
                        dissipationModels[i] = new DissipationModelScaled([dissipationModelParameters](double time)->double{return dissipationModelParameters[0];}); // all processes will use the same dissipation model
                    }
                } else {
                    if(rank==0)logger.printError("dissipation model parameters for scaled dissipation must be one: aborting")<<std::endl;
                    return 1;
                }
            } else {
                if(rank==0)logger.printError("dissipation model parameters for scaled dissipation was not set: setting to default 0.5 costant")<<std::endl;
                dissipationModel = new DissipationModelScaled();
                for(int i = 0; i < finalWorkload; ++i){
                    dissipationModels[i] = new DissipationModelScaled(); // all processes will use the same dissipation model
                }
            }
        } else if(dissipationModelName == "periodic"){
            if(rank==0)logger << "[LOG] dissipation model was set to periodic, the function will be a sinusoidal function with amplitude, period and phase" << std::endl;
            if (vm.count("dissipationModelParameters")) {
                std::vector<double> dissipationModelParameters = vm["dissipationModelParameters"].as<std::vector<double>>();
                if (dissipationModelParameters.size() == 3) {
                    if(rank==0)logger << "[LOG] dissipation model parameters were set to Amplitude:"
                        << dissipationModelParameters[0] << " & period:" << dissipationModelParameters[1] << " & phase: " << dissipationModelParameters[2] << std::endl;
                    dissipationModel = new DissipationModelScaled([dissipationModelParameters](double time)->double{return dissipationModelParameters[0]*sin(2*arma::datum::pi/dissipationModelParameters[1]*time + dissipationModelParameters[2]);});
                    for(int i = 0; i < finalWorkload; ++i){
                        dissipationModels[i] = new DissipationModelScaled([dissipationModelParameters](double time)->double{return dissipationModelParameters[0]*sin(2*arma::datum::pi/dissipationModelParameters[1]*time + dissipationModelParameters[2]);}); // all processes will use the same dissipation model
                    }
                } else {
                    if(rank==0)logger.printError("dissipation model parameters for periodic dissipation must be three for amplitude, period and phase: aborting")<<std::endl;
                    return 1;
                }
                
                
            } else {
                if(rank==0)logger.printError("dissipation model parameters for periodic dissipation was not set: aborting")<<std::endl;
                return 1;
            }
        } else if(dissipationModelName == "custom"){
            //control if custom function for dissipation returns double and takes a single parameter as double
            if(rank==0)logger << "[LOG] dissipation model was set to custom, if the function is not correctly defined there will be errors" << std::endl;
            if (vm.count("dissipationModelParameters")) {
                std::vector<double> dissipationModelParameters = vm["dissipationModelParameters"].as<std::vector<double>>();
                if(rank==0){
                    logger << "[LOG] dissipation model parameters were declared to be: ("; // this section can bring problems with the stream and concurrency
                    for (auto param : dissipationModelParameters) {
                        logger << param << ", ";
                    }
                    logger << ")" << std::endl;
                }
                dissipationModel = new DissipationModelScaled(getDissipationScalingFunction(dissipationModelParameters));
                for(int i = 0; i < finalWorkload; ++i){
                    dissipationModels[i] = new DissipationModelScaled(getDissipationScalingFunction(dissipationModelParameters)); // all processes will use the same dissipation model
                }
            } else if(vm.count("dissipationModelParameterFolder")) {
                if(rank==0)logger << "[LOG] dissipation model parameters were declared to be in the folder "<<vm["dissipationModelParameterFolder"].as<std::string>()<<std::endl;
                std::string dissipationModelParametersFolder = vm["dissipationModelParameterFolder"].as<std::string>();
                std::map<std::string, std::vector<std::string>> typeToOrderedNodeNames;
                for(int i = 0; i < finalWorkload; ++i){
                    typeToOrderedNodeNames[types[i+startIdx]] = typeComputations[i]->getAugmentedGraph()->getNodeNames();
                }
                auto dissipationModelScalingFunctions = dissipationScalingFunctionsFromFolder(dissipationModelParametersFolder,typeToOrderedNodeNames);
                for(int i = 0; i < finalWorkload; ++i){
                    dissipationModels[i] = new DissipationModelScaled(dissipationModelScalingFunctions[types[i+startIdx]]); // all processes will use the same dissipation model with different parameters
                }
            } else {
                if(rank==0)logger << "[LOG] dissipation model parameters were not set, using the default scaling function (defined in the custom functions)" << std::endl;
                dissipationModel = new DissipationModelScaled(getDissipationScalingFunction());
                for(int i = 0; i < finalWorkload; ++i){
                    dissipationModels[i] = new DissipationModelScaled(getDissipationScalingFunction()); // all processes will use the same dissipation model
                }
            }
        } else {
            if(rank==0)logger.printError("dissipation model scale function is not any of the types. Conservation model scale functions available are none(default), scaled, random and custom");
            return 1;
        }
    } else { //dissipation model set to default (none)
        if(rank==0)logger << "[LOG] dissipation model was not set. set to default (none)\n";
        dissipationModel = new DissipationModelScaled([](double time)->double{return 0;});
        for(int i = 0; i < finalWorkload; ++i){
            dissipationModels[i] = new DissipationModelScaled([](double time)->double{return 0;}); // all processes will use the same dissipation model
        }
    }
    //initialize the dissipation and conservation models for each computation
    for(int i = 0; i < finalWorkload; i++){
        typeComputations[i]->setDissipationModel(dissipationModels[i]);
        typeComputations[i]->setConservationModel(conservationModels[i]);
    }

    // create a map that maps couples of strings (source type and target type) to a vector of pairs of strings, representing how the virtual outputs are mapped in the subarray passed to MPI send 
    std::unordered_map<std::pair<std::string, std::string>, std::vector<std::pair<std::string, std::string>>,hash_pair_strings> typesPairMappedVirtualOutputsVectors;
    // create a map that maps couples of strings (source type and target type) to a vector of pairs of strings, representing how the virtual inputs are mapped in the subarray passed to MPI send
    std::unordered_map<std::pair<std::string, std::string>, std::vector<std::pair<std::string, std::string>>,hash_pair_strings> typesPairMappedVirtualInputsVectors;

    // create a map that maps couples of integers (source rank and target rank) to a vector of pairs of strings(virtual output name, virtual input name), representing how the virtual nodes are mapped in the array passed to MPI send
    std::unordered_map<std::pair<int, int>, std::vector<std::pair<std::string, std::string>>,hash_pair_ints> ranksPairMappedVirtualNodesVectors;

    
    // populate the maps
    for(auto interaction = interactionBetweenTypesFinerMap.cbegin() ; interaction != interactionBetweenTypesFinerMap.cend(); interaction++ ){
        std::string startNodeName = std::get<0> (interaction->first);
        std::string endNodeName = std::get<1> (interaction->first);
        std::string startType = std::get<2> (interaction->first);
        std::string endType = std::get<3> (interaction->first);
        std::pair<std::string, std::string> keyTypes = std::make_pair(startType, endType);
        // inverted interaction only used when specified by the command line argument (undirectedTypeEdges)
        std::pair<std::string, std::string> keyTypesInverted = std::make_pair(endType, startType);
        
        std::vector<std::pair<std::string, std::string>> virtualOutputsVector;
        std::vector<std::pair<std::string, std::string>> virtualInputsVector;
        std::string virtualOutputNodeName = "";
        std::string virtualInputNodeName = "";
        if(virtualNodesGranularity == "type"){
            virtualOutputNodeName = "v-out:" + endType;
            virtualInputNodeName = "v-in:" + startType;
        } else {
            virtualOutputNodeName = "v-out:" + endType + "_" + endNodeName;
            virtualInputNodeName = "v-in:" + startType + "_" + startNodeName;

        }
        
        // mapped virtualOutputs and mapped virtualInputs are the same in the sizes and logic, but have different names
        // mapped virtual outputs have the format (sourceNode, v-out:tTarget<_targetNode>)
        if(virtualNodesGranularity == "typeAndNode"){
            if(typesPairMappedVirtualOutputsVectors.contains(keyTypes)){
                if(!vectorContains(typesPairMappedVirtualOutputsVectors[keyTypes],std::make_pair(startNodeName, virtualOutputNodeName))){
                    typesPairMappedVirtualOutputsVectors[keyTypes].push_back(std::make_pair(startNodeName, virtualOutputNodeName));
                }
            } else {
                typesPairMappedVirtualOutputsVectors[keyTypes] = std::vector<std::pair<std::string, std::string>>();
                typesPairMappedVirtualOutputsVectors[keyTypes].push_back(std::make_pair(startNodeName, virtualOutputNodeName));
            }
            
            // mapped virtual input have the format (v-in:tSource<_sourceNode>, targetNode)
            if(typesPairMappedVirtualInputsVectors.contains(keyTypes)){
                if(!vectorContains(typesPairMappedVirtualInputsVectors[keyTypes],std::make_pair(virtualInputNodeName, endNodeName))){
                    typesPairMappedVirtualInputsVectors[keyTypes].push_back(std::make_pair(virtualInputNodeName, endNodeName));
                }
            } else {
                typesPairMappedVirtualInputsVectors[keyTypes] = std::vector<std::pair<std::string, std::string>>();
                typesPairMappedVirtualInputsVectors[keyTypes].push_back(std::make_pair(virtualInputNodeName, endNodeName));
            }
        } else if (virtualNodesGranularity == "type"){
            if(typesPairMappedVirtualOutputsVectors.contains(keyTypes)){
                if(!vectorContains(typesPairMappedVirtualOutputsVectors[keyTypes],std::make_pair(virtualInputNodeName, virtualOutputNodeName))){
                    typesPairMappedVirtualOutputsVectors[keyTypes].push_back(std::make_pair(virtualInputNodeName, virtualOutputNodeName));
                }
            } else {
                typesPairMappedVirtualOutputsVectors[keyTypes] = std::vector<std::pair<std::string, std::string>>();
                typesPairMappedVirtualOutputsVectors[keyTypes].push_back(std::make_pair(virtualInputNodeName, virtualOutputNodeName));
            }
            
            // mapped virtual input have the format (v-in:tSource<_sourceNode>, targetNode)
            if(typesPairMappedVirtualInputsVectors.contains(keyTypes)){
                if(!vectorContains(typesPairMappedVirtualInputsVectors[keyTypes],std::make_pair(virtualInputNodeName, virtualOutputNodeName))){
                    typesPairMappedVirtualInputsVectors[keyTypes].push_back(std::make_pair(virtualInputNodeName, virtualOutputNodeName));
                }
            } else {
                typesPairMappedVirtualInputsVectors[keyTypes] = std::vector<std::pair<std::string, std::string>>();
                typesPairMappedVirtualInputsVectors[keyTypes].push_back(std::make_pair(virtualInputNodeName, virtualOutputNodeName));
            }
        }
    }

    // populate the ranks map
    for(int rankTarget = 0; rankTarget < numProcesses; rankTarget++){
        int targetWorkload;
        if(rankTarget == numProcesses - 1){
            targetWorkload = types.size() - (numProcesses - 1)*workloadPerProcess;
        } else {
            targetWorkload = workloadPerProcess;
        }
        for(int sourceTarget = 0; sourceTarget < numProcesses; sourceTarget++){
            int sourceWorkload;
            if(sourceTarget == numProcesses - 1){
                sourceWorkload = types.size() - (numProcesses - 1)*workloadPerProcess;
            } else {
                sourceWorkload = workloadPerProcess;
            }
            for(int sourceIndexLocal = 0; sourceIndexLocal < sourceWorkload; sourceIndexLocal++){
                int sourceIndexGlobal = sourceIndexLocal + sourceTarget*workloadPerProcess;
                std::string sourceType = types[sourceIndexGlobal];
                for(int targetIndexLocal = 0; targetIndexLocal < targetWorkload; targetIndexLocal++){
                    int targetIndexGlobal = targetIndexLocal + rankTarget*workloadPerProcess;
                    std::string targetType = types[targetIndexGlobal];
                    std::pair<std::string, std::string> keyTypes = std::make_pair(sourceType, targetType);
                    std::pair<int, int> keyRanks = std::make_pair(sourceTarget, rankTarget);
                    // mapped virtual nodes have the format (v-out:tTarget<_targetNode> , v-in:tSource<_sourceNode>)
                    if(typesPairMappedVirtualOutputsVectors.contains(keyTypes)){
                        for(uint index = 0; index < typesPairMappedVirtualOutputsVectors[keyTypes].size(); index++){
                            std::pair<std::string, std::string> virtualOutput = typesPairMappedVirtualOutputsVectors[keyTypes][index];
                            std::pair<std::string, std::string> virtualInput = typesPairMappedVirtualInputsVectors[keyTypes][index];
                            std::pair<std::string, std::string> virtualNode = std::make_pair(virtualOutput.second, virtualInput.first);
                            
                            ranksPairMappedVirtualNodesVectors[keyRanks].push_back(virtualNode);
                        }
                    }
                    
                }
            }
        }
        
    }
    logger << "[LOG] type interactions loaded for rank "<< rank << std::endl; 
    

    // setting propagation model in this moment since in the case of the original model, the pseudoinverse should be computed for the augmented pathway
    std::function<double(double)> propagationScalingFunction = [](double time)->double{return 1;};
    if(vm.count("propagationModel")){
        if(rank==0)logger << "[LOG] propagation model was set to "
            << vm["propagationModel"].as<std::string>() << ".\n";
        std::string propagationModelName = vm["propagationModel"].as<std::string>();
        if(propagationModelName == "default"){
            if(rank==0)logger << "[LOG] propagation model set to default (pseudoinverse propagation)\n";
            for(int i = 0; i < finalWorkload ;i++ ){
                typeComputations[i]->setPropagationModel(new PropagationModelOriginal(typeComputations[i]->getAugmentedGraph(),propagationScalingFunction));
            }
            //nothing to do, default propagation scaling function is the identity
        } else if (propagationModelName == "scaled"){
            if (vm.count("propagationModelParameters")) {
                if(rank==0)logger << "[LOG] propagation model parameters were declared to be "
                    << vm["propagationModelParameters"].as<std::vector<double>>()[0] << ".\n";
                std::vector<double> propagationModelParameters = vm["propagationModelParameters"].as<std::vector<double>>();
                if(propagationModelParameters.size() == 1){
                    propagationScalingFunction = [propagationModelParameters](double time)->double{return propagationModelParameters[0];};
                    for(int i = 0; i < finalWorkload ;i++ ){
                        PropagationModel* tmpPropagationModel = new PropagationModelOriginal(typeComputations[i]->getAugmentedGraph(),propagationScalingFunction);
                        typeComputations[i]->setPropagationModel(tmpPropagationModel);
                    }
                } else {
                    if(rank==0)logger.printError("propagation model parameters for scaled propagation must be one parameter: aborting")<<std::endl;
                    return 1;
                }
            } else {
                if(rank==0)logger.printError("propagation model parameters for scaled propagation was not set: setting to default 1 costant")<<std::endl;
                for(int i = 0; i < finalWorkload ;i++ ){
                    PropagationModel* tmpPropagationModel = new PropagationModelOriginal(typeComputations[i]->getAugmentedGraph(),propagationScalingFunction);
                    typeComputations[i]->setPropagationModel(tmpPropagationModel);
                }
                //nothing to do, default propagation scaling function is the identity
            }
        } else if (propagationModelName == "neighbors"){
            if (vm.count("propagationModelParameters")) {
                if(rank==0)logger << "[LOG] propagation model parameters were declared to be "
                    << vm["propagationModelParameters"].as<std::vector<double>>()[0] << ".\n";
                std::vector<double> propagationModelParameters = vm["propagationModelParameters"].as<std::vector<double>>();
                if(propagationModelParameters.size() == 1){
                    propagationScalingFunction = [propagationModelParameters](double time)->double{return propagationModelParameters[0];};
                    for(int i = 0; i < finalWorkload;i++ ){
                        PropagationModel* tmpPropagationModel = new PropagationModelNeighbors(typeComputations[i]->getAugmentedGraph(),propagationScalingFunction);
                        typeComputations[i]->setPropagationModel(tmpPropagationModel);
                    }
                } else {
                    if(rank==0)logger.printError("propagation model parameters for scaled propagation must be one parameter: aborting")<<std::endl;
                    return 1;
                }
            } else {
                if(rank==0)logger.printError("propagation model parameters for scaled propagation was not set: setting to default 1 costant")<<std::endl;
                for(int i = 0; i < finalWorkload;i++ ){
                    PropagationModel* tmpPropagationModel = new PropagationModelNeighbors(typeComputations[i]->getAugmentedGraph(),propagationScalingFunction);
                    typeComputations[i]->setPropagationModel(tmpPropagationModel);
                }
                //nothing to do, default propagation scaling function is the identity
            }
        } else if(propagationModelName == "customScaling"){ 
            if(rank==0)logger << "[LOG] propagation model set to custom scaling propagation (with original propagation model)\n";
            if(vm.count("propagationModelParameters")){
                if(rank==0)logger << "[LOG] propagation model parameters were declared to be "
                    << vm["propagationModelParameters"].as<std::vector<double>>()[0] << std::endl; //TODO change the logger to print the whole vector
                std::vector<double> propagationModelParameters = vm["propagationModelParameters"].as<std::vector<double>>();
                propagationScalingFunction = getPropagationScalingFunction(propagationModelParameters);
            } else {
                if(rank==0)logger.printError("[LOG] propagation model parameters for custom scaling propagation was not set: setting to default custom function (no parameters passed)")<<std::endl;
                propagationScalingFunction = getPropagationScalingFunction();
            }
            for(int i = 0; i < finalWorkload;i++ ){
                PropagationModel* tmpPropagationModel = new PropagationModelOriginal(typeComputations[i]->getAugmentedGraph(),propagationScalingFunction);
                typeComputations[i]->setPropagationModel(tmpPropagationModel);
            }
        } else if(propagationModelName == "customScalingNeighbors"){
            if(rank==0)logger << "[LOG] propagation model set to custom scaling neighbors propagation (with neighbors propagation model)\n";
            if(vm.count("propagationModelParameters")){
                if(rank==0)logger << "[LOG] propagation model parameters were declared to be "
                    << vm["propagationModelParameters"].as<std::vector<double>>()[0] << std::endl; //TODO change the logger to print the whole vector
                std::vector<double> propagationModelParameters = vm["propagationModelParameters"].as<std::vector<double>>();
                propagationScalingFunction = getPropagationScalingFunction(propagationModelParameters);
            } else{
                if(rank==0)logger.printError("[LOG] propagation model parameters for custom scaling neighbors propagation was not set: setting to default custom function (no parameters passed)")<<std::endl;
                propagationScalingFunction = getPropagationScalingFunction();
            }
            for(int i = 0; i < finalWorkload;i++ ){
                PropagationModel* tmpPropagationModel = new PropagationModelNeighbors(typeComputations[i]->getAugmentedGraph(),propagationScalingFunction);
                typeComputations[i]->setPropagationModel(tmpPropagationModel);
            }
            
        } else if(propagationModelName == "customPropagation"){
            if(rank==0)logger << "[LOG] propagation model set to custom propagation (with custom propagation model)\n";
            if(vm.count("propagationModelParameters")){
                if(rank==0)logger << "[LOG] propagation model parameters were declared to be "
                << vm["propagationModelParameters"].as<std::vector<double>>()[0] << std::endl;  //TODO change the logger to print the whole vector
                std::vector<double> propagationModelParameters = vm["propagationModelParameters"].as<std::vector<double>>();
                propagationScalingFunction = getPropagationScalingFunction(propagationModelParameters);
            } else {
                if(rank==0)logger.printError("[LOG] propagation model parameters for custom propagation was not set: setting to default custom function (no parameters passed)")<<std::endl;
                propagationScalingFunction = getPropagationScalingFunction();
            }
            for(int i = 0; i < finalWorkload;i++ ){
                PropagationModel* tmpPropagationModel = new PropagationModelCustom(typeComputations[i]->getAugmentedGraph(),propagationScalingFunction);
                typeComputations[i]->setPropagationModel(tmpPropagationModel);
            }
        
        } else {
            if(rank==0)logger.printError("propagation model is not any of the types. propagation model scale functions available are default, scaled, neighbors and custom");
            return 1;
        }
    } else {
        if(rank==0)logger << "[LOG] propagation model was not set. set to default (pseudoInverse)" << std::endl;
        for(int i = 0; i < finalWorkload;i++ ){
            PropagationModel* tmpPropagationModel = new PropagationModelOriginal(typeComputations[i]->getAugmentedGraph(),propagationScalingFunction);
            typeComputations[i]->setPropagationModel(tmpPropagationModel);
        }
    }

    // virtual inputs and virtual outputs buffers for the MPI communication
    // buffer for the virtual outputs from the other processes(virtual inputs), the maximum size is the power of 2 of the workload per process(since every type will send values to every other type)
    std::vector<double*> rankVirtualInputsBuffer;
    std::vector<uint> rankVirtualInputsSizes = std::vector<uint>(numProcesses,0);
    if(virtualNodesGranularity == "type"){
        for(int i = 0; i < numProcesses; i++){
            int currentWorkload;
            if(i == (numProcesses-1)){
                currentWorkload = types.size() - (i*workloadPerProcess);
            } else {
                currentWorkload = workloadPerProcess;
            }
            rankVirtualInputsSizes[i] = finalWorkload * currentWorkload;
            rankVirtualInputsBuffer.push_back(new double[finalWorkload * currentWorkload]);   // the array contains all the virtual outputs for the process types
        }
    } else if (virtualNodesGranularity == "typeAndNode"){
        // allocate the buffer for the virtual outputs for the combination of types and nodes
        for(int sourceRank = 0; sourceRank < numProcesses; sourceRank++){
            std::pair<int, int> keyRanks = std::make_pair(sourceRank, rank);
            if(ranksPairMappedVirtualNodesVectors.contains(keyRanks)){
                rankVirtualInputsSizes[sourceRank] = ranksPairMappedVirtualNodesVectors[keyRanks].size();
            } else {
                rankVirtualInputsSizes[sourceRank] = 0;
            }
            // allocate the array for the virtual inputs directed to the local rank types
            rankVirtualInputsBuffer.push_back(new double[rankVirtualInputsSizes[sourceRank]]);
        }
            
    }

    // buffer for virtual inputs to other processes(virtual outputs)
    std::vector<double*> virtualOutputs;
        std::vector<uint> rankVirtualOutputsSizes = std::vector<uint>(numProcesses,0);
        // different granularity for the virtual nodes means different ways of building the virtual outputs arrays and sizes
        if(virtualNodesGranularity == "type"){ //classical way of building the virtual outputs arrays, one array for each type representing virtual nodes for each type
            for(int i = 0; i < numProcesses; i++){
                int currentWorkload;
                if(i == (numProcesses-1)){
                    currentWorkload = types.size() - (i*workloadPerProcess);
                } else {
                    currentWorkload = workloadPerProcess;
                }
                rankVirtualOutputsSizes[i] = currentWorkload * finalWorkload;
                virtualOutputs.push_back(new double[rankVirtualOutputsSizes[i]]);   // the array contains all the virtual outputs for the process types
            }
        } else if (virtualNodesGranularity == "typeAndNode"){ // finer granularity, one array for each type and node representing virtual nodes for each type and node (as a couple)
            
            //allocate the virtual outputs arrays
            for(int targetRank = 0; targetRank < numProcesses; targetRank++){
                std::pair<int, int> keyRanks = std::make_pair(rank, targetRank);
                if(ranksPairMappedVirtualNodesVectors.contains(keyRanks)){
                    rankVirtualOutputsSizes[targetRank] = ranksPairMappedVirtualNodesVectors[keyRanks].size();
                } else {
                    rankVirtualOutputsSizes[targetRank] = 0;
                }

                // allocate the array for the virtual outputs directed to the target rank types
                virtualOutputs.push_back(new double[rankVirtualOutputsSizes[targetRank]]);                        
            }
        } else {
            if(rank==0)logger.printError("virtual nodes granularity is not any of the types. virtual nodes granularity available are type and typeAndNode");
            return 1;
        }

    // load checkpoint if resumeCheckpoint parameter is set
    int startingInterIteration = 0;
    int startingIntraIteration = 0;
    if(resumeCheckpoint){
        for(int i = 0; i < finalWorkload; i++){
            checkpoint.loadState(types[i+startIdx], startingInterIteration, startingIntraIteration, typeComputations[i]);
        }
    }

    for(int iterationInterType = startingInterIteration; iterationInterType < intertypeIterations; iterationInterType++){
        for(int iterationIntraType = startingIntraIteration; iterationIntraType < intratypeIterations; iterationIntraType++){
            // save checkpoint
            for(int i = 0; i < finalWorkload; i++){
                checkpoint.cleanCheckpoints(types[i+startIdx]);
                checkpoint.saveState(types[i+startIdx], iterationInterType, iterationIntraType, typeComputations[i]);
            }
            
            // computation of perturbation
            #pragma omp parallel for
            for(int i = 0; i < finalWorkload; i++){
                if(rank==0)logger.printLog(true,"computation of perturbation for iteration intertype-intratype (", iterationInterType, "<->", iterationIntraType, ") for type (", types[i+startIdx], ")"); 
                // TODO use stateful scaling function to consider previous times
                try
                {
                    if (saturation) {
                        if(vm.count("saturationTerm") == 0){
                            std::vector<double> outputValues = typeComputations[i]->computeAugmentedPerturbationEnhanced4((iterationInterType*intratypeIterations + iterationIntraType)*(timestep/intratypeIterations), saturation = true);
                        } else if (vm.count("saturationTerm") >= 1) {
                            double saturationTerm = vm["saturationTerm"].as<double>();
                            std::vector<double> saturationVector = std::vector<double>(typeComputations[i]->getAugmentedGraph()->getNumNodes(),saturationTerm);
                            std::vector<double> outputValues = typeComputations[i]->computeAugmentedPerturbationEnhanced4((iterationInterType*intratypeIterations + iterationIntraType)*(timestep/intratypeIterations), saturation = true, saturationVector);
                        }
                    } else{
                        std::vector<double> outputValues = typeComputations[i]->computeAugmentedPerturbationEnhanced4((iterationInterType*intratypeIterations + iterationIntraType)*(timestep/intratypeIterations), saturation = false);
                    }
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                    exit(1);
                }
            }



            //save output values
            for(int i = 0; i < finalWorkload; i++){
                std::vector<std::string> nodeNames = typeComputations[i]->getAugmentedGraph()->getNodeNames();
                int currentIteration = iterationInterType*intratypeIterations + iterationIntraType;
                double currentTime = currentIteration*(timestep/intratypeIterations);
                if(outputFormat == "singleIteration"){
                    std::string outputFolderNameSingular = outputFoldername + "/currentPerturbations";
                    saveNodeValuesWithTimeSimple(outputFolderNameSingular, currentIteration, currentTime, types[i+startIdx], typeComputations[i]->getOutputAugmented(), nodeNames, nodesDescriptionFilename);
                } else if(outputFormat == "iterationMatrix"){
                    std::vector<double> currentPerturbation = typeComputations[i]->getOutputAugmented();
                    if(currentIteration != 0){
                        // add the column to the matrix
                        outputMatrices[types[i+startIdx]]->addColumnAtTheEnd(currentPerturbation);
                    } else {
                        // create the matrix
                        outputMatrices[types[i+startIdx]] = new Matrix<double>(currentPerturbation, currentPerturbation.size(), 1);
                        outputMatricesRowNames[types[i+startIdx]] = nodeNames;
                    }
                }
            }

            //update input
            for(int i = 0; i < finalWorkload; i++){
                //If conservation of the initial values is required, the input is first updated with the initial norm value
                if (conservateInitialNorm) {
                    int index = indexMapGraphTypesToValuesTypes[i+startIdx];
                    std::vector<double> inputInitial = inputInitials[index];
                    double initialNorm = vectorNorm(inputInitial);
                    double outputNorm = vectorNorm(typeComputations[i]->getOutputAugmented());
                    double normRatio = initialNorm/outputNorm;
                    std::vector<double> newInput = vectorScalarMultiplication(typeComputations[i]->getOutputAugmented(),normRatio);
                    logger.printLog(true,"update input with conservation of the initial perturbation for iteration intertype-intratype (", iterationInterType, "<->", iterationIntraType, ") for type (", types[i+startIdx], ")"); // could change this to verbose
                    typeComputations[i]->updateInput(newInput,true);
                } else {
                    logger.printLog(true,"update input for iteration intertype-intratype (", iterationInterType, "<->", iterationIntraType, ") for type (", types[i+startIdx], ")");  //could change this to verbose
                    typeComputations[i]->updateInput(std::vector<double>(),true);
                }
                
            }
        }

        // send virtual outputs to the other processes, the vector contains the virtual outputs for each type as an array
        // for every type, send the virtual outputs to the other processes, all in the same array (this array will be decomposed on the target)


        if(virtualNodesGranularity == "type"){ //classical way of building the virtual outputs arrays, one array for each type representing virtual nodes for each type
            // fill the arrays
            for(int i = 0; i < SizeToInt(types.size()); i++){
                int targetRank = typeToRank[types[i]];
                int targetWorkload;
                if(targetRank == (numProcesses-1)){
                    targetWorkload = types.size() - (targetRank*workloadPerProcess);
                } else {
                    targetWorkload = workloadPerProcess;
                }
                int targetPosition = i - targetRank * workloadPerProcess;
                for(int j = 0; j < finalWorkload; j++ ){
                    int virtualOutputPosition = targetPosition + j * targetWorkload;
                    
                    virtualOutputs[targetRank][virtualOutputPosition] = typeComputations[j]->getVirtualOutputForType(types[i]);
                }
                
            }
        } else if (virtualNodesGranularity == "typeAndNode"){ // finer granularity, one array for each type and node representing virtual nodes for each type and node (as a couple)    
            // fill the arrays
            for (int targetRank = 0; targetRank < numProcesses; targetRank++){
                std::pair<int, int> keyRanks = std::make_pair(rank, targetRank);
                if(ranksPairMappedVirtualNodesVectors.contains(keyRanks)){
                    for(uint i = 0; i < ranksPairMappedVirtualNodesVectors[keyRanks].size(); i++){
                        std::pair<std::string, std::string> virtualNode = ranksPairMappedVirtualNodesVectors[keyRanks][i];
                        std::vector<std::string> virtualInputNodeSplit = splitVirtualNodeStringIntoVector(virtualNode.second);
                        if(virtualInputNodeSplit.size()==3){
                            std::string sourceType = virtualInputNodeSplit[1];
                            std::string sourceNode = virtualInputNodeSplit[2];
                            int sourceTypePosition = -1;
                            for(int j = 0; j < finalWorkload; j++){
                                if(types[j+startIdx] == sourceType){
                                    sourceTypePosition = j;
                                    break;
                                }
                            }
                            if(sourceTypePosition == -1){
                                logger.printError("source type not found in the types vector: aborting") << std::endl;
                                return 1;
                            }
                            double virtualOutputValue = typeComputations[sourceTypePosition]->getOutputNodeValue(virtualNode.first);
                            virtualOutputs[targetRank][i] = virtualOutputValue;
                        } else {
                            logger.printError("virtual node string is not in the correct format: aborting") << std::endl;
                            return 1;
                        }
                    }
                }
                        
            }
        } else {
            if(rank==0)logger.printError("virtual nodes granularity is not any of the types. virtual nodes granularity available are type and typeAndNode");
            return 1;
        }


        


        // reset virtual outputs if specified, should work since virtual outputs are assigned before
        if(resetVirtualOutputs){
            for(int i = 0; i < finalWorkload; i++){
                typeComputations[i]->resetVirtualOutputs();
            }
        }



        // preliminary asynchronous receive
        
        // receive the virtual outputs from the other processes
        MPI_Request request[numProcesses];
        for(int i = 0; i < numProcesses; i++){
            int sourceRank = i;
            // receive only the virtual outputs for the types granularity (v-out for each type), or the full vectors for each pair of source type and target type
            if(virtualNodesGranularity == "typeAndNode" || virtualNodesGranularity == "type"){
                std::pair<int, int> keyRanks = std::make_pair(sourceRank, rank);
                if(ranksPairMappedVirtualNodesVectors.contains(keyRanks)){
                    MPI_Irecv(rankVirtualInputsBuffer[sourceRank], rankVirtualInputsSizes[sourceRank], MPI_DOUBLE, sourceRank, 0, MPI_COMM_WORLD, &request[sourceRank]);
                }
            } else {
                // OTHER CASES NOT IMPLEMENTED YET
            }
        }

    

        // send the virtual outputs to the other processes
        for(int targetRank = 0; targetRank < numProcesses; targetRank++){
            //sending virtual outputs to target cell
            // int targetStartIdx = j * workloadPerProcess;
            // int targetEndIdx = (j == numProcesses - 1) ? types.size() : (j + 1) * workloadPerProcess;
            // logger << "[LOG] sending virtual output from type " << types[startIdx] << " to type " << types[endIdx-1] << " from process " << rank << " to process " << j << " from type " << types[targetStartIdx] << " to type " << types[targetEndIdx-1] << std::endl;
            
            //synchronized communication will lead to deadlocks with this type of implementation
            if(virtualNodesGranularity == "typeAndNode" || virtualNodesGranularity == "type"){
                //send the subvectors of the virtual outputs for the combination of types and nodes
                std::pair<int, int> keyRanks = std::make_pair(rank, targetRank);
                if(ranksPairMappedVirtualNodesVectors.contains(keyRanks)){
                    try
                    {
                        MPI_Send(virtualOutputs.at(targetRank), rankVirtualOutputsSizes[targetRank], MPI_DOUBLE, targetRank, 0, MPI_COMM_WORLD);
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << e.what() << std::endl;
                        logger.printError("error in sending virtual outputs from process ") << rank << " to process " << targetRank << std::endl;
                        return 1;
                    }
                    logger.printLog(true,"sent virtual outputs from process ", rank, " to process ", targetRank);
                }
            } else {
                // send only the virtual outputs for the types granularity (v-out for each type
            }
        }

        // receive outputs from the other processes and update the input
        for(int sourceRank = 0; sourceRank < numProcesses; sourceRank++){
            std::pair<int, int> ranksPair = std::make_pair(sourceRank, rank);
            if(ranksPairMappedVirtualNodesVectors.contains(ranksPair)){
                logger.printLog(true,"receiving virtuals outputs from process ", sourceRank, " to process ", rank);
                try{
                    MPI_Wait(&request[sourceRank], MPI_STATUS_IGNORE);
                } catch(const std::exception& e){
                    std::cerr << e.what() << std::endl;
                    logger.printError("error in waiting for virtual outputs from process ") << sourceRank << " to process " << rank << std::endl;
                    return 1;
                }
                logger.printLog(true,"received virtual outputs from process ", sourceRank, " to process ", rank);
            }
            // source workload and virtual outputs decomposition on the target

            int sourceWorkload;
            if(sourceRank == (numProcesses-1)){
                sourceWorkload = types.size() - (sourceRank*workloadPerProcess);
            } else {
                sourceWorkload = workloadPerProcess;
            }
            if(virtualNodesGranularity == "type"){
                for(int isource = 0; isource < sourceWorkload; isource++){
                    for(int ilocal = 0; ilocal < finalWorkload; ilocal++){
                        int virtualInputPosition = ilocal + isource * finalWorkload;
                        int localTypePosition = ilocal + startIdx;
                        int sourceTypePosition = isource + sourceRank*workloadPerProcess;
                        std::pair<std::string,std::string> keyTypes = std::make_pair(types[localTypePosition], types[sourceTypePosition]);
                        if(quantizationMethod=="single"){
                            if(setDoubleContainsInterval(interactionBetweenTypesMap[keyTypes], iterationInterType* timestep, (iterationInterType + 1)* timestep)){
                                // logger << "[TEST] contact times for types " << types[localTypePosition] << " and " << types[sourceTypePosition] << " are ";
                                // for(auto time: interactionBetweenTypesMap[keyTypes]){
                                //     logger << time << ", ";
                                // } 
                                // logger << std::endl;
                                if(localTypePosition==sourceTypePosition){
                                    if(sameTypeCommunication) typeComputations[ilocal]->setInputVinForType(types[sourceTypePosition], rankVirtualInputsBuffer[sourceRank][virtualInputPosition]);
                                } else {
                                    typeComputations[ilocal]->setInputVinForType(types[sourceTypePosition], rankVirtualInputsBuffer[sourceRank][virtualInputPosition]);
                                }
                            }
                        }else if(quantizationMethod=="multiple"){
                            int countIntervalWidth = setDoubleIntervalWidth(interactionBetweenTypesMap[keyTypes], iterationInterType* timestep, (iterationInterType + 1)* timestep);
                            if(countIntervalWidth>0){
                                double newValue = rankVirtualInputsBuffer[sourceRank][virtualInputPosition]*countIntervalWidth;
                                if(localTypePosition==sourceTypePosition){
                                    if(sameTypeCommunication) typeComputations[ilocal]->setInputVinForType(types[sourceTypePosition], newValue);
                                } else {
                                    typeComputations[ilocal]->setInputVinForType(types[sourceTypePosition], newValue);
                                }
                            }
                        } else {
                            logger.printError("quantization method is not any of the types. quantization method available are single and multiple");
                            return 1;
                        }
                    }
                }
            } else if (virtualNodesGranularity == "node"){
                if(rank==0)logger.printError("virtual nodes granularity is not supported yet: aborting");
                return 1;
            } else if (virtualNodesGranularity == "typeAndNode"){
                // logic of reading the subvectors of the virtual inputs

                // use ranksPairMappedVirtualNodesVectors to get all the information needed to update the input
                int targetRank = rank;
                std::pair<int,int> ranksPair = std::make_pair(sourceRank,targetRank);

                if(ranksPairMappedVirtualNodesVectors.contains(ranksPair)){

                    for(uint i = 0; i < ranksPairMappedVirtualNodesVectors[ranksPair].size(); i++){
                        
                        std::pair<std::string, std::string> virtualNodes = ranksPairMappedVirtualNodesVectors[ranksPair][i];
                        std::string virtualOutputNodeName = virtualNodes.first;
                        std::string virtualInputNodeName = virtualNodes.second;
                        std::vector<std::string> virtualOutputNodeNameSplitted = splitVirtualNodeStringIntoVector(virtualOutputNodeName);
                        if(virtualOutputNodeNameSplitted.size()!=3) throw std::runtime_error("main:: virtual output node name is not in the correct format: " + virtualOutputNodeName);
                        std::string targetType = virtualOutputNodeNameSplitted[1];
                        std::string targetNodeName = virtualOutputNodeNameSplitted[2];

                        std::vector<std::string> virtualInputNodeNameSplitted = splitVirtualNodeStringIntoVector(virtualInputNodeName);
                        if(virtualInputNodeNameSplitted.size()!=3) throw std::runtime_error("main:: virtual input node name is not in the correct format: " + virtualInputNodeName);
                        std::string sourceType = virtualInputNodeNameSplitted[1];
                        std::string sourceNodeName = virtualInputNodeNameSplitted[2];

                        // get local target index for typeComputation
                        int targetTypeIndex = -1;
                        for(int i = 0; i < finalWorkload; i++){
                            if(types[i+startIdx] == targetType){
                                targetTypeIndex = i;
                                break;
                            }
                        }

                        if(targetTypeIndex == -1) throw std::runtime_error("main:: target type index not found for type: " + targetType);
                        std::tuple<std::string, std::string, std::string, std::string> interactionKey = std::make_tuple(sourceNodeName, targetNodeName, sourceType, targetType);
                        if(interactionBetweenTypesFinerMap.contains(interactionKey)){
                            if(quantizationMethod=="single"){
                                if(setDoubleContainsInterval(interactionBetweenTypesFinerMap[interactionKey], iterationInterType* timestep, (iterationInterType + 1)* timestep)){
                                    double newValue = rankVirtualInputsBuffer[sourceRank][i];
                                    try{
                                        if(sourceType == targetType){
                                            if(sameTypeCommunication) typeComputations[targetTypeIndex ]->setInputNodeValue(virtualInputNodeName, newValue);
                                        } else {
                                            typeComputations[targetTypeIndex]->setInputNodeValue(virtualInputNodeName, newValue);
                                        }
                                    } catch(const std::exception& e){
                                        std::cerr << e.what() << std::endl;
                                        logger.printError("error in setting input for virtual nodes from process: ")<< sourceRank<< " to process "<< targetRank << " for virtual nodes: " << virtualInputNodeName << " and " << virtualOutputNodeName << std::endl;
                                        return 1;
                                    }
                                } else {
                                    // // TESTING
                                    // std::cout << "[DEBUG] rank: " << rank << " interaction between nodes " << sourceNodeName << " and " << targetNodeName << " for types " << sourceType << " and " << targetType << " have no contact time for inter iteration "<< iterationInterType << std::endl;
                                    // // TESTING
                                }
                            } else if(quantizationMethod=="multiple"){
                                int countIntervalWidth = setDoubleIntervalWidth(interactionBetweenTypesFinerMap[interactionKey], iterationInterType* timestep, (iterationInterType + 1)* timestep);
                                if(countIntervalWidth>0){
                                    double newValue = rankVirtualInputsBuffer[sourceRank][i]*countIntervalWidth;
                                    try{
                                        if(sourceType == targetType){
                                            if(sameTypeCommunication) typeComputations[targetTypeIndex ]->setInputNodeValue(virtualInputNodeName, newValue);
                                        } else {
                                            typeComputations[targetTypeIndex]->setInputNodeValue(virtualInputNodeName, newValue);
                                        }
                                    } catch(const std::exception& e){
                                        std::cerr << e.what() << std::endl;
                                        logger.printError("error in setting input for virtual nodes from process: ")<< sourceRank<< " to process "<< targetRank << " for virtual nodes: " << virtualInputNodeName << " and " << virtualOutputNodeName << std::endl;
                                        return 1;
                                    }
                                } else {
                                    // // TESTING
                                    // std::cout << "[DEBUG] rank: " << rank << " interaction between nodes " << sourceNodeName << " and " << targetNodeName << " for types " << sourceType << " and " << targetType << " have no contact time for inter iteration "<< iterationInterType << std::endl;
                                    // // TESTING
                                }
                            } else {
                                logger.printError("quantization method is not any of the types. quantization method available are single and multiple");
                                return 1;
                            }
                        } else {
                            logger.printError("interaction between nodes: ") << sourceNodeName << " and " << targetNodeName << " for types " << sourceType << " and " << targetType << " is not present in the interactionBetweenTypesFinerMap" << std::endl;
                            logger.printError("aborting") << std::endl;
                            return 1;
                        }

                        
                    }
                }
            }
        }
    }

    // save the augmented graph for every type if the option was set
    if (saveAugmentedNetworks) {
        logger << "[LOG] saving the augmented graphs for types in rank " << rank<<std::endl;
        // create the output folder if it does not exist
        std::string outputFolderNameGraphs = outputFoldername + "/augmentedGraphs";
        if (!std::filesystem::exists(outputFolderNameGraphs)) {
            std::filesystem::create_directory(outputFolderNameGraphs);
        }
        // save all the augmented graphs in a single file for every type
        for(int i = 0; i < finalWorkload; i++){
            std::string type = types[i+startIdx];
            std::string filename = type + ".tsv";
            typeComputations[i]->getAugmentedGraph()->saveEdgesToFile(outputFolderNameGraphs, filename);
        }
    }

    // delete graphs objects
    // if the graph was homogenous (having specified a single graph file) the graph pointer is shared between all the types, so it needs to be deleted only once
    // if the graph was heterogeneous, the graph pointer is different for each type, so it needs to be deleted for each type
    if(vm.count("graphsFilesFolder") > 0){
        for(int i = 0; i < finalWorkload; i++){
            delete graphs[i];
            graphs[i] = nullptr;
        }
    } else {
        // delete the graph only once
        delete graphs[0];
        graphs[0] = nullptr;
    }
    // set the graph pointer to nullptr for all the types computations
    for(int i = 0; i < finalWorkload; i++){
        typeComputations[i]->setGraph(nullptr);
    }
    delete[] graphs;

    // delete typeComputations objects
    for(int i = 0; i < finalWorkload; i++){
        typeComputations[i]->freeFunctions();  // freeing propagation model, dissipation model and conservation model
        delete typeComputations[i];
    }
    delete conservationModel; //TO CHANGE, REMOVE
    delete dissipationModel; // TO CHANGE, REMOVE
    delete[] typeComputations;
    
    // delete the virtual outputs vector of arrays
    for(uint i = 0; i < virtualOutputs.size(); i++){
        if (virtualOutputs.at(i) != nullptr) delete[] virtualOutputs.at(i);
    }
    // delete the virtual inputs vector of arrays
    for(uint i = 0; i < rankVirtualInputsBuffer.size(); i++){
        if (rankVirtualInputsBuffer.at(i) != nullptr) delete[] rankVirtualInputsBuffer.at(i);
    }

    // save into the iterationMatrix files for every type if the option was set
    if (outputFormat == "iterationMatrix") {
        logger << "[LOG] saving the iteration matrices for types in rank " << rank<<std::endl;
        // create the output folder if it does not exist
        std::string outputFolderNameMatrices = outputFoldername + "/iterationMatrices";
        if (!std::filesystem::exists(outputFolderNameMatrices)) {
            std::filesystem::create_directory(outputFolderNameMatrices);
        }
        // save all the iteration values in a single file for every type
        for(int i = 0; i < finalWorkload; i++){
            std::string type = types[i+startIdx];
            saveOutputMatrix(outputFolderNameMatrices, outputMatrices[type], outputMatricesRowNames[type], intertypeIterations,intratypeIterations, timestep,type);
        }
    }

    logger.printLog(true,"computation ended for rank ", rank, " with no errors");
    MPI_Finalize();

    logger.printLog(true,"MPI finalized for rank ", rank);

    // take ending time after the computation
    auto end = std::chrono::steady_clock::now();
    if(rank == 0){
        if(vm.count("savePerformance")){
            logger << "[LOG] saving performance"<<std::endl;
            std::ofstream performanceFile;
            int numberProcesses = numProcesses;
            int numberTypes = types.size();
            int numberIterations = intratypeIterations * intertypeIterations;
            performanceFile.open (performanceFilename, std::ios::out | std::ios::app);
            if (performanceFile.fail())
                throw std::ios_base::failure(std::strerror(errno));
            //performanceFile << "inputFolderGraphs\t" << "numberProcesses" << "\t" << "numberTypes" << "\t" << "numberIterations" << "\t" << "time" << std::endl;
            performanceFile << graphsFilesFolder << "\t" << numberProcesses << "\t" << numberTypes << "\t" << numberIterations << "\t" << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
            performanceFile.close();
        }
    }

    logger.printLog(true,"Execution ended for rank ", rank, " with no errors");
    if(rank==0)logger << "[LOG] Execution ended with no problems" << std::endl;
    return 0;
}
