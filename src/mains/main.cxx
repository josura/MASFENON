/**
 * @file main.cxx
 * @ingroup Mains
 * @brief Main function for the MASFENON simulation framework. Without the use of MPI
 * @details This file contains the main function for running the MASFENON simulation framework without MPI. It handles command-line arguments, initializes the computation, and executes the simulation.
 * @note This version of the main function is designed non-MPI platforms.
 * @warning This version is deprecated and behind the MPI version. It is kept for backward compatibility and testing purposes.
 */
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



int main(int argc, char** argv ) {
    bool sameTypeCommunication=false; ///< boolean variable to indicate if the same type communication is used
    bool saturation=false;
    bool conservateInitialNorm=false;
    bool undirected = false;
    bool undirectedTypeEdges = false;
    bool resetVirtualOutputs = false;
    std::string logMode="";
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    std::string performanceFilename = "";
    desc.add_options()
        ("help", "() print help section")//<initialPerturbationPerType>.tsv [<subtypes>.txt] [<typesInteraction>.tsv]\nFILE STRUCTURE SCHEMA:\ngraph.tsv\nstart end weight\n<gene1> <gene2>  <0.something>\n...\n\n\ninitialPerturbationPerType.tsv\n type1 type2 ... typeN\ngene1 <lfc_type1:gene1> <lfc_type2:gene1> ... <lfc_typeN:gene1>\ngene1 <lfc_type1:gene2> <lfc_type2:gene2> ... <lfc_typeN:gene2>\n...\n\n\ntypesInteraction.tsv\nstartType:geneLigand endType:geneReceptor weight\n<type1:geneLigand> <type2:genereceptor>  <0.something>\n...\n\n\nsubtypes.txt\ntype1\ntype3\n...")
        ("fUniqueGraph", po::value<std::string>(), "(string) graph filename, for an example graph see in resources. NOTE: if this option is chosen, graphsFilesFolder cannot be used. For an example see in data data/testdata/testGraph/edges-Graph1-general.tsv")
        ("fInitialPerturbationPerType", po::value<std::string>(), "(string) initialPerturbationPerType matrix filename, for an example see in data data/testdata/testGraph/initialValues-general.tsv")
        ("subtypes", po::value<std::string>(), "subtypes filename, for an example see in data, see data/testdata/testGraph/subcelltypes.txt")
        ("initialPerturbationPerTypeFolder", po::value<std::string>(), "(string) initialPerturbationPerType folder, for an example see in data data/testdata/testGraph/initialValues")
        ("typeInteractionFolder", po::value<std::string>(), "(string) directory for the type interactions, for an example see in data data/testdata/testHeterogeneousGraph/interactions")
        ("nodeDescriptionFile", po::value<std::string>(), "(string) node description file, used to generate the output description, if not specified no names are used. for an example see in data resources/graphs/metapathwayNew/nodes.tsv")
        ("sameTypeCommunication",po::bool_switch(&sameTypeCommunication),"() use same type communication, since it is not permitted as the standard definition of the model, this adds a virtual node for the same type type")
        ("outputFolder",po::value<std::string>()->required(),"(string) output folder for output of the algorithm at each iteration")
        ("intertypeIterations",po::value<uint>(),"(positive integer) number of iterations for intertype communication")
        ("intratypeIterations",po::value<uint>(),"(positive integer) number of iterations for intratype communication")
        ("timestep",po::value<double>(),"timestep to use for the iteration, the final time is iterationIntracell*iterationIntercell*timestep")
        ("dissipationModel",po::value<std::string>(),"(string) the dissipation model for the computation, available models are: 'none (default)','power','random','periodic','scaled' and 'custom'")
        ("dissipationModelParameters",po::value<std::vector<double>>()->multitoken(),"(string) the parameters for the dissipation model, for the power dissipation indicate the base, for the random dissipation indicate the min and max value, for the periodic dissipation indicate the period")
        ("graphsFilesFolder",po::value<std::string>(),"(string) graphs (pathways or other types of graphs) file folder, for an example see in data data/testdata/testHeterogeneousGraph/graphsDifferentStructure")
        ("conservationModel",po::value<std::string>(),"(string) the conservation model used for the computation, available models are: 'none (default)','scaled','random' and 'custom' ")
        ("conservationModelParameters", po::value<std::vector<double>>()->multitoken(),"(vector<double>) the parameters for the dissipation model, for the scaled parameter the constant used to scale the conservation final results, in the case of random the upper and lower limit (between 0 and 1)")
        ("propagationModel",po::value<std::string>(),"(string) the propagation model used for the computation, available models are: 'default(pseudoinverse creation)','scaled (pseudoinverse * scale parameter)', neighbors(propagate the values only on neighbors at every iteration and scale parameter) and 'customScaling' (pseudoinverse*scalingFunction(parameters)), 'customScalingNeighbors' (neighbors propagation and scalingFunction(parameters)), 'customPropagation' (custom scaling function and custom propagation function defined in src/PropagationModelCustom) ")
        ("propagationModelParameters", po::value<std::vector<double>>()->multitoken(),"(vector<double>) the parameters for the propagation model, for the scaled parameter the constant used to scale the conservation final results")
        ("saturation",po::bool_switch(&saturation),"use saturation of values, default to 1, if another value is needed, use the saturationTerm")
        ("saturationTerm",po::value<double>(),"defines the limits of the saturation [-saturationTerm,saturationTerm]")
        ("conservateInitialNorm",po::bool_switch(&conservateInitialNorm), "conservate the initial euclidean norm of the perturbation values, that is ||Pn|| <= ||Initial||, default to false")
        ("undirectedEdges",po::bool_switch(&undirected), "edges in the graphs are undirected")
        ("undirectedTypeEdges",po::bool_switch(&undirectedTypeEdges), "edges between types are undirected")
        ("resetVirtualOutputs",po::bool_switch(&resetVirtualOutputs), "reset the virtual outputs to 0 at each iteration, default to false")
        ("virtualNodesGranularity", po::value<std::string>(), "(string) granularity of the virtual nodes, available options are: 'type', 'node', 'typeAndNode', default to type")
        ("virtualNodesGranularityParameters", po::value<std::vector<std::string>>()->multitoken(), "(vector<string>) parameters for the virtual nodes granularity, NOT USED for now")
        ("loggingOptions",po::value<std::string>(&logMode),"(string) logging options, available options are: 'all','none', default to all")
        ("savePerformance",po::value<std::string>(&performanceFilename), "(string) output performance (running time, number of total nodes, number of communities, number of total edges) to the defined file, if nothing is specified the performance are not saved")
    ;
    //TODO add additional boolean parameter to control if the graph names are not genes and the algorithm should use the graph names directly, no conversion or mapping

    

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    std::string filename,subtypesFilename,typesFilename,typesInteractionFoldername,typesInitialPerturbationMatrixFilename,graphsFilesFolder, typeInitialPerturbationFolderFilename,outputFoldername;
    uint intertypeIterations,intratypeIterations;
    DissipationModel* dissipationModel = nullptr;
    ConservationModel* conservationModel = nullptr;
    double timestep = 1;

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    //logging options
    auto& logger = Logger::getInstance();
    if(vm.count("loggingOptions")){
        if(logMode == "all"){
            std::cout << "[LOG] logging options set to all"<<std::endl;
            logger.enable();
        } else if (logMode == "none"){
            std::cout << "[LOG] logging options set to none"<<std::endl;
            logger.disable();
        } else {
            std::cout << "[LOG] logging options set to default (all)"<<std::endl;
            logger.enable();
        }
    } else {
        std::cout << "[LOG] logging options set to default (all)"<<std::endl;
        logger.enable();
    }


    //controls over impossible configurations
    if(vm.count("fUniqueGraph") == 0 && vm.count("graphsFilesFolder") == 0){
        //no unique graph of folder of the graphs was set
        std::cerr << "[ERROR] no unique graph filename or folder was set to get the graphs, set one "<<std::endl;
        return 1;
    }

    if(vm.count("fInitialPerturbationPerType") == 0 && vm.count("initialPerturbationPerTypeFolder") == 0){
        //no way of getting the initial perturbation values
        std::cerr << "[ERROR] no matrix for the initial values was passed as filename or single vector in files contained in the folder specified was set, set one "<<std::endl;
        return 1;
    }

    if(vm.count("fInitialPerturbationPerType") && vm.count("graphsFilesFolder")){
        //unstable configuration of different graphs and single matrix with the same nodes
        logger << "[WARNING] unstable configuration of different graphs and a single matrix with the initial perturbations"<<std::endl;
    }

    if(saturation && conservateInitialNorm){
        std::cerr << "[ERROR] saturation and conservateInitialNorm cannot be both true, aborting"<<std::endl;
        return 1;
    }


    if(vm.count("graphsFilesFolder") && vm.count("fUniqueGraph")){
        std::cerr << "[ERROR] fUniqueGraph and graphsFilesFolder were both set. Aborting\n";
        return 1;
    }
    if(vm.count("initialPerturbationPerTypeFolder") && vm.count("fInitialPerturbationPerType")){
        std::cerr << "[ERROR] fInitialPerturbationPerType and initialPerturbationPerTypeFolder were both set. Aborting\n";
        return 1;
    }

    // reading the parameters

    if (vm.count("intertypeIterations")) {
        logger << "[LOG] iterations intertype set to " 
    << vm["intertypeIterations"].as<uint>() << ".\n";
        intertypeIterations = vm["intertypeIterations"].as<uint>();
    } else {
        logger << "[LOG] iterations intertype not set, set to default: 10 iterations \n";
        intertypeIterations = 10;
    }

    if (vm.count("intratypeIterations")) {
        logger << "[LOG] iterations intratype set to " 
    << vm["intratypeIterations"].as<uint>() << ".\n";
        intratypeIterations = vm["intratypeIterations"].as<uint>();
    } else {
        logger << "[LOG] iterations intratype not set, set to default: 5 iterations \n";
        intratypeIterations = 5;
    }

    //logging timestep settings
    if(vm.count("timestep")){
        logger << "[LOG] timestep set to " 
    << vm["timestep"].as<double>() << ".\n";
        timestep = vm["timestep"].as<double>();
    } else {
        logger << "[LOG] timestep not set, set to default (1)"<<std::endl;
    }

    //logging edges direction in the graphs
    if(undirected){
        logger << "[LOG] undirectedEdges specified, undirected edges in the graphs"<<std::endl;
    } else {
        logger << "[LOG] undirectedEdges not specified, directed edges in the graphs(only the edges specified in the graph files will be added)"<<std::endl;
    }

    //logging edges direction in the graphs
    if(undirectedTypeEdges){
        logger << "[LOG] undirectedTypeEdges specified, undirected edges between types"<<std::endl;
    } else {
        logger << "[LOG] undirectedTypeEdges not specified, directed edges between types"<<std::endl;
    }

    //logging virtual nodes reset

    if(resetVirtualOutputs){
        logger << "[LOG] resetVirtualOutputs specified, virtual outputs will be reset to 0 after each inter-propagation"<<std::endl;
    } else {
        logger << "[LOG] resetVirtualOutputs not specified, virtual outputs will not be reset to 0 after each inter-propagation"<<std::endl;
    }



    if (vm.count("fUniqueGraph")) {
        logger << "[LOG] file for the graph was set to " 
    << vm["fUniqueGraph"].as<std::string>() << ".\n";
        filename = vm["fUniqueGraph"].as<std::string>();
        if(!fileExistsPath(filename)){
            std::cerr << "[ERROR] file for the graph do not exist: aborting"<<std::endl;
            return 1;
        }
    } else if(vm.count("graphsFilesFolder")){
        logger << "[LOG] folder for the graphs was set to " 
    << vm["graphsFilesFolder"].as<std::string>() << ".\n";
        graphsFilesFolder = vm["graphsFilesFolder"].as<std::string>();
        if(!folderExists(graphsFilesFolder)){
            std::cerr << "[ERROR] folder for the graphs do not exist: aborting"<<std::endl;
            return 1;
        }
    }
    if (vm.count("fInitialPerturbationPerType")) {
        logger << "[LOG] file for the initialPerturbationPerType matrix was set to " 
    << vm["fInitialPerturbationPerType"].as<std::string>() << ".\n";
        typesInitialPerturbationMatrixFilename = vm["fInitialPerturbationPerType"].as<std::string>();
        if(!fileExistsPath(typesInitialPerturbationMatrixFilename)){
            std::cerr << "[ERROR] file for the initialPerturbationPerType does not exist: aborting"<<std::endl;
            return 1;
        }
    } else if (vm.count("initialPerturbationPerTypeFolder")) {
        logger << "[LOG] folder for the initialPerturbationPerType was set to "
    << vm["initialPerturbationPerTypeFolder"].as<std::string>() << ".\n";
        typeInitialPerturbationFolderFilename = vm["initialPerturbationPerTypeFolder"].as<std::string>();
        if(!folderExists(typeInitialPerturbationFolderFilename)){
            std::cerr << "[ERROR] folder for the initialPerturbationPerType do not exist: aborting"<<std::endl;
            return 1;
        }
    }

    if (vm.count("typeInteractionFolder")) {
        logger << "[LOG] folder for the type interactions was set to " 
    << vm["typeInteractionFolder"].as<std::string>() << ".\n";
        typesInteractionFoldername = vm["typeInteractionFolder"].as<std::string>();
        if(!folderExists(typesInteractionFoldername)){
            std::cerr << "[ERROR] folder for the type interactions do not exist: aborting"<<std::endl;
            return 1;
        }
    } else {
        logger << "[LOG] typeInteractionFolder folder was not set. computing without taking into account type interactions\n";
    }
    if (vm.count("outputFolder")) {
        logger << "[LOG] output folder  was set to " 
    << vm["outputFolder"].as<std::string>() << ".\n";
        outputFoldername = vm["outputFolder"].as<std::string>();
        if(!folderExists(outputFoldername)){
            std::cerr << "[WARNING] folder for the output do not exist: creating the folder"<<std::endl;
            if(!createFolder(outputFoldername)){
                std::cerr << "[ERROR] folder for the output could not be created: aborting"<<std::endl;
                return 1;
            }
        }
    } else {
        std::cerr << "[ERROR] output folder was not set. aborting\n";
        return 1;
    }
    if (vm.count("dissipationModel")) {
        logger << "[LOG] dissipation model was set to "
    << vm["dissipationModel"].as<std::string>() << ".\n";
        std::string dissipationModelName = vm["dissipationModel"].as<std::string>();
        if(dissipationModelName == "none"){
            logger << "[LOG] dissipation model set to default (none)\n";
            dissipationModel = new DissipationModelScaled([](double time)->double{return 0;});
        } else if(dissipationModelName == "power"){
            if (vm.count("dissipationModelParameters")) {
                logger << "[LOG] dissipation model parameters for power dissipation were declared to be" << vm["dissipationModelParameters"].as<std::vector<double>>()[0] << ".\n";
                std::vector<double> dissipationModelParameters = vm["dissipationModelParameters"].as<std::vector<double>>();
                if(dissipationModelParameters.size() == 1){
                    dissipationModel = new DissipationModelPow(dissipationModelParameters[0]);
                } else {
                    std::cerr << "[ERROR] dissipation model parameters for power dissipation must be one: aborting"<<std::endl;
                    return 1;
                }
            } else {
                std::cerr << "[ERROR] dissipation model parameters for power dissipation was not set: setting to default (2)"<<std::endl;
                dissipationModel = new DissipationModelPow(2);
            }
        } else if(dissipationModelName == "random"){
            if (vm.count("dissipationModelParameters")) {
                logger << "[LOG] dissipation model parameters were declared to be "
            << vm["dissipationModelParameters"].as<std::vector<double>>()[0] << " & " << vm["dissipationModelParameters"].as<std::vector<double>>()[1] << ".\n";
                std::vector<double> dissipationModelParameters = vm["dissipationModelParameters"].as<std::vector<double>>();
                if(dissipationModelParameters.size() == 2){
                    dissipationModel = new DissipationModelRandom(dissipationModelParameters[0],dissipationModelParameters[1]);
                } else {
                    std::cerr << "[ERROR] dissipation model parameters for random dissipation must be two: aborting"<<std::endl;
                    return 1;
                }
            } else {
                std::cerr << "[ERROR] dissipation model parameters for random dissipation was not set: aborting"<<std::endl;
                return 1;
            }
        } else if(dissipationModelName == "scaled"){
            if (vm.count("dissipationModelParameters")) {
                logger << "[LOG] dissipation model parameters were declared to be "
            << vm["dissipationModelParameters"].as<std::vector<double>>()[0] << ".\n";
                std::vector<double> dissipationModelParameters = vm["dissipationModelParameters"].as<std::vector<double>>();
                if(dissipationModelParameters.size() == 1){
                    dissipationModel = new DissipationModelScaled([dissipationModelParameters](double time)->double{return dissipationModelParameters[0];});
                } else {
                    std::cerr << "[ERROR] dissipation model parameters for scaled dissipation must be one: aborting"<<std::endl;
                    return 1;
                }
            } else {
                std::cerr << "[ERROR] dissipation model parameters for scaled dissipation was not set: setting to default 0.5 costant"<<std::endl;
                dissipationModel = new DissipationModelScaled();
            }
        } else if(dissipationModelName == "periodic"){
            if (vm.count("dissipationModelParameters")) {
                std::vector<double> dissipationModelParameters = vm["dissipationModelParameters"].as<std::vector<double>>();
                if (dissipationModelParameters.size() == 3) {
                    logger << "[LOG] dissipation model parameters were set to Amplitude:"
                    << dissipationModelParameters[0] << " & period:" << dissipationModelParameters[1] << " & phase: " << dissipationModelParameters[2] << ".\n";
                    dissipationModel = new DissipationModelScaled([dissipationModelParameters](double time)->double{return dissipationModelParameters[0]*sin(2*arma::datum::pi/dissipationModelParameters[1]*time + dissipationModelParameters[2]);});
                } else {
                    std::cerr << "[ERROR] dissipation model parameters for periodic dissipation must be three for amplitude, period and phase: aborting"<<std::endl;
                    return 1;
                }
                
                
            } else {
                std::cerr << "[ERROR] dissipation model parameters for periodic dissipation was not set: aborting"<<std::endl;
                return 1;
            }
        } else if(dissipationModelName == "custom"){
            //control if custom function for dissipation returns double and takes a single parameter as double
            logger << "[LOG] dissipation model was set to custom, if the function is not correctly defined there will be errors\n " << std::endl;
            dissipationModel = new DissipationModelScaled(getDissipationScalingFunction());
        } else {
            std::cerr << "[ERROR] dissipation model scale function is not any of the types. Conservation model scale functions available are none(default), scaled, random and custom \n";
            return 1;
        }
    } else { //dissipation model set to default (none)
        logger << "[LOG] dissipation model was not set. set to default (none)\n";
        dissipationModel = new DissipationModelScaled([](double time)->double{return 0;});
    }


    if (vm.count("conservationModel")) {
        logger << "[LOG] conservation model was set to "
    << vm["conservationModel"].as<std::string>() << ".\n";
        std::string conservationModelName = vm["conservationModel"].as<std::string>();
        if(conservationModelName == "none"){
            logger << "[LOG] conservation model set to default (none)\n";
            conservationModel = new ConservationModel([](double time)->double{return 0;});
        } else if (conservationModelName == "scaled"){
            if (vm.count("conservationModelParameters")) {
                logger << "[LOG] conservation model parameters were declared to be "
            << vm["conservationModelParameters"].as<std::vector<double>>()[0] << ".\n";
                std::vector<double> conservationModelParameters = vm["conservationModelParameters"].as<std::vector<double>>();
                if(conservationModelParameters.size() == 1){
                    conservationModel = new ConservationModel([conservationModelParameters](double time)->double{return conservationModelParameters[0];});
                } else {
                    std::cerr << "[ERROR] conservation model parameters for scaled conservation must be one parameter: aborting"<<std::endl;
                    return 1;
                }
            } else {
                std::cerr << "[ERROR] conservation model parameters for scaled conservation was not set: setting to default 0.5 costant"<<std::endl;
                conservationModel = new ConservationModel();
            }
        } else if (conservationModelName == "random"){
            if (vm.count("conservationModelParameters")) {
                logger << "[LOG] conservation model parameters were declared to be "
            << vm["conservationModelParameters"].as<std::vector<double>>()[0] << " & " << vm["conservationModelParameters"].as<std::vector<double>>()[1] << ".\n";
                std::vector<double> conservationModelParameters = vm["conservationModelParameters"].as<std::vector<double>>();
                if(conservationModelParameters.size() == 2){
                    //control if lower and upper limits of the random values are within 0 and 1
                    if( (conservationModelParameters[0] < 0) || (conservationModelParameters[0] > 1) || (conservationModelParameters[1] < 0) || (conservationModelParameters[1] > 1) || (conservationModelParameters[0] > conservationModelParameters[1]) ){
                        std::cerr << "[ERROR] conservation model parameters for random conservation must be between 0 and 1 and must be a < b: aborting"<<std::endl;
                        return 1;
                    }
                    conservationModel = new ConservationModel([conservationModelParameters](double time)->double{return randomRealNumber(conservationModelParameters[0],conservationModelParameters[1]);});
                } else {
                    std::cerr << "[ERROR] conservation model parameters for random conservation must be two: aborting"<<std::endl;
                    return 1;
                }
            } else {
                std::cerr << "[ERROR] conservation model parameters for random conservation was not set: aborting"<<std::endl;
                return 1;
            }
        } else if(conservationModelName == "custom"){
            //control if custom function for conservation returns double and takes a single parameter as double
            logger << "[LOG] conservation model was set to custom, if the custom function defined for scaling is not correctly implemented, there will be errors\n " << std::endl;
            conservationModel = new ConservationModel(getConservationScalingFunction());
        } else {
            std::cerr << "[ERROR] conservation model scale function is not any of the types. Conservation model scale functions available are none(default), scaled, random and custom \n";
            return 1;
        }
    } else {
        logger << "[LOG] conservation model was not set. set to default (none)\n";
        conservationModel = new ConservationModel([](double time)->double{return 0;});
    }

    //logging if saturation is set and saturation parameters are set
    if (saturation) {
        if(vm.count("saturationTerm") == 0){
            logger << "[LOG] saturation term not specified, using the interval [-1,1]"<<std::endl;
        } else if(vm.count("saturationTerm") == 1){
            double saturationTerm = vm["saturationTerm"].as<double>();
            logger << "[LOG] saturation term specified, using the interval [-" << saturationTerm << "," << saturationTerm << "]"<<std::endl;
        } else {
            std::cerr << "[ERROR] saturation term specified more than once, possibility of using more values not yet implemented: aborting"<<std::endl;
            return 1;
        }
    }
    //end program options section

    std::string nodesDescriptionFilename="";
    if(vm.count("nodeDescriptionFile")){
        logger <<"[LOG] using node description file to get the names of the nodes in the graphs"<<std::endl;
        nodesDescriptionFilename = vm["nodeDescriptionFile"].as<std::string>();
    } else {
        logger <<"[LOG] no nodes description"<<std::endl;
    }


    //take the types from the matrix file or from the folder
    std::vector<std::string> types;
    if(vm.count("fUniqueGraph")){
        if(vm.count("fInitialPerturbationPerType")){
            types = getTypesFromMatrixFile(typesInitialPerturbationMatrixFilename);

        } else if (vm.count("initialPerturbationPerTypeFolder")){
            types = getTypesFromFolderFileNames(typeInitialPerturbationFolderFilename);
        } else {
            std::cerr << "[ERROR] no initial perturbation file or folder specified: aborting"<<std::endl;
            return 1;
        }
    } else if (vm.count("graphsFilesFolder")) {
        types = getTypesFromFolderFileNames(graphsFilesFolder);
    } else {
        std::cerr << "[ERROR] no graph file or folder specified: aborting"<<std::endl;
        return 1;
    }

    std::vector<std::string> subtypes;
    if(vm.count("subtypes")){
        logger << "[LOG] subtypes filename set to "
    << vm["subtypes"].as<std::string>() << ".\n";
        subtypesFilename = vm["subtypes"].as<std::string>();
        subtypes = getVectorFromFile<std::string>(subtypesFilename);
    }else{
        logger << "[LOG] subtypes filename not set, set to default: all types \n";
        subtypes = types;
    }
    
    //filter types with the subtypes
    std::vector<std::string> typesFiltered = vectorsIntersection(types, subtypes);
    if (typesFiltered.size() == 0) {
        std::cerr << "[ERROR] no types in common between the types and subtypes: aborting"<<std::endl;
        return 1;
    }
    
    
    //use the number of types to allocate an array of pointers to contain the graph for every type
    WeightedEdgeGraph **graphs = new WeightedEdgeGraph*[types.size()];
    std::vector<std::vector<std::string>> graphsNodes;
    std::vector<std::pair<std::vector<std::string>,std::vector<std::tuple<std::string,std::string,double>>>> namesAndEdges;
    if(vm.count("fUniqueGraph")){
        namesAndEdges.push_back(edgesFileToEdgesListAndNodesByName(filename));
        graphsNodes.push_back(namesAndEdges[0].first);
        graphs[0] = new WeightedEdgeGraph(graphsNodes[0]);
        for(uint i = 1; i < types.size(); i++){
            namesAndEdges.push_back(namesAndEdges[0]);
            graphsNodes.push_back(namesAndEdges[0].first);
            graphs[i] = graphs[0];
        }
    } else if (vm.count("graphsFilesFolder")) {
        auto allGraphs = edgesFileToEdgesListAndNodesByNameFromFolder(graphsFilesFolder);
        auto typesFromFolder = allGraphs.first;
        if(typesFromFolder.size() != types.size()){
            std::cerr << "[ERROR] types from folder and types from file do not match: aborting"<<std::endl;
            return 1;
        }
        for (uint i = 0; i<typesFromFolder.size(); i++){ //TODO map the types from the folder to the types from the file
            if(typesFromFolder[i] != types[i]){
                std::cerr << "[ERROR] types from folder and types from file do not match: aborting"<<std::endl;
                return 1;
            }
        }
        namesAndEdges = allGraphs.second;
        for(uint i = 0; i < types.size(); i++){
            graphsNodes.push_back(namesAndEdges[i].first);
            graphs[i] = new WeightedEdgeGraph(graphsNodes[i]);
        }
    } 

    //add the edges to the graphs
    if(vm.count("fUniqueGraph")){
        for(auto edge = namesAndEdges[0].second.cbegin() ; edge != namesAndEdges[0].second.cend(); edge++ ){
            graphs[0]->addEdge(std::get<0> (*edge), std::get<1> (*edge) ,std::get<2>(*edge) ,!undirected);
        }
    } else if (vm.count("graphsFilesFolder")) {
        for(uint i = 0; i < types.size(); i++){
            for(auto edge = namesAndEdges[i].second.cbegin() ; edge != namesAndEdges[i].second.cend(); edge++ ){
                graphs[i]->addEdge(std::get<0> (*edge), std::get<1> (*edge) ,std::get<2>(*edge) ,!undirected);
            }
        }
    }

    //get initial input values
    std::tuple<std::vector<std::string>, std::vector<std::string>, std::vector<std::vector<double>>> initialValues;
    std::vector<std::vector<double>> inputInitials;
    if(vm.count("fInitialPerturbationPerType")){
        logger << "[LOG] initial perturbation per type specified, using the file "<<typesInitialPerturbationMatrixFilename<<std::endl;
        initialValues = valuesMatrixToTypeVectors(typesInitialPerturbationMatrixFilename,graphsNodes[0],subtypes);
    } else if (vm.count("initialPerturbationPerTypeFolder")){
        logger << "[LOG] initial perturbation per type specified, using the folder "<<typeInitialPerturbationFolderFilename<<std::endl;
        initialValues = valuesVectorsFromFolder(typeInitialPerturbationFolderFilename,types,graphsNodes,subtypes);
    } else {
        std::cerr << "[ERROR] no initial perturbation file or folder specified: aborting"<<std::endl;
        return 1;
    }
    std::vector<std::string> initialNames = std::get<0>(initialValues);
    inputInitials = std::get<2>(initialValues);
    std::vector<std::string> typesFromValues = std::get<1>(initialValues);
    //this condition should take into account the intersection of the types and the subtypes
    if(typesFromValues.size() == 0){
        std::cerr << "[ERROR] types from the initial values folder are 0, control if the types are the same to the one specified in the matrix, in the graphs folder and in the subtypes: aborting"<<std::endl;
        std::cerr << "[ERROR] types specified(subtypes): ";
        for(auto type: subtypes)
            std::cerr << type << " ";
        std::cerr << std::endl;
        std::cerr << "[ERROR] types from file(from graphs folder or from matrix): ";
        for(auto type: types)
            std::cerr << type << " ";
        std::cerr << std::endl;
        std::cerr << "[ERROR] types from values(from initial values folder or from values matrix) intersected with subtypes: ";
        for(auto type: typesFromValues)
            std::cerr << type << " ";
        std::cerr << std::endl;
        return 1;
    }
    auto indexMapGraphTypesToValuesTypes = get_indexmap_vector_values_full(types, typesFromValues);
    if(indexMapGraphTypesToValuesTypes.size() == 0){
        std::cerr << "[ERROR] types from folder and types from file do not match even on one instance: aborting"<<std::endl;
        return 1;
    }

    Computation** typeComputations = new Computation*[typesFiltered.size()];
    int indexComputation = 0;
    std::vector<int> typesIndexes = std::vector<int>(types.size(),-1); 
    std::vector<int> invertedTypesIndexes = std::vector<int>(typesFiltered.size(),-1); 
    for(uint i = 0; i < types.size();i++){
        if(vectorContains(typesFiltered, types[i])){
            if(indexMapGraphTypesToValuesTypes[i] == -1){
                logger << "[LOG] type "<<types[i]<<" not found in the initial perturbation files, using zero vector as input"<<std::endl;
                std::vector<double> input = std::vector<double>(graphsNodes[i].size(),0);
                Computation* tmpCompPointer = new Computation(types[i],input,graphs[i],graphsNodes[i]);   
                tmpCompPointer->setDissipationModel(dissipationModel);
                tmpCompPointer->setConservationModel(conservationModel);
                typeComputations[indexComputation] = tmpCompPointer;
                //No inverse computation with the augmented graph since virtual nodes edges are not yet inserted
                typeComputations[indexComputation]->augmentGraphNoComputeInverse(typesFiltered,std::vector<std::pair<std::string,std::string>>(),std::vector<double>(), true); //self included for uniformation with the code in MPI
            } else {
                int index = indexMapGraphTypesToValuesTypes[i];
                std::vector<double> input = inputInitials[index];
                Computation* tmpCompPointer = new Computation(types[i],input,graphs[i],graphsNodes[i]); 
                tmpCompPointer->setDissipationModel(dissipationModel);
                tmpCompPointer->setConservationModel(conservationModel);
                typeComputations[indexComputation] = tmpCompPointer;
                //No inverse computation with the augmented graph since virtual nodes edges are not yet inserted
                typeComputations[indexComputation]->augmentGraphNoComputeInverse(typesFiltered,std::vector<std::pair<std::string,std::string>>(),std::vector<double>(), true); //self included for uniformation with the code in MPI
            }
            typesIndexes[i] = indexComputation;
            invertedTypesIndexes[indexComputation] = i;
            indexComputation++;
        }
    }
    std::vector<std::vector<std::string>> typeToNodeNames = std::vector<std::vector<std::string>>(typesFiltered.size(),std::vector<std::string>());
    for(uint i = 0; i < typesFiltered.size();i++ ){
        typeToNodeNames[i] = typeComputations[i]->getAugmentedGraph()->getNodeNames();    
    }
    auto allFilesInteraction = get_all(typesInteractionFoldername,".tsv");
    // define the map for the type interactions, an hash function should be defined for the pair of strings used as the identifier of the interaction
    std::unordered_map<std::pair<std::string, std::string>, std::unordered_set<int>, hash_pair_strings> interactionBetweenTypesMap;
    for(auto typeInteractionFilename = allFilesInteraction.cbegin() ; typeInteractionFilename != allFilesInteraction.cend() ; typeInteractionFilename++){
        std::pair<std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>>,std::vector<std::tuple<std::string, std::string, std::string, std::string, std::unordered_set<int>, double>>> typeInteractionsEdges;
        if (subtypes.size() == 0) {
            typeInteractionsEdges  = interactionContactsFileToEdgesListAndNodesByName(*typeInteractionFilename, types, intertypeIterations);
        } else {
            typeInteractionsEdges = interactionContactsFileToEdgesListAndNodesByName(*typeInteractionFilename, subtypes, intertypeIterations);
        }
        #pragma omp parallel for
        for (uint i = 0; i < types.size();i++) {
            if(typeInteractionsEdges.first.contains(types[i]) && typesIndexes[i] != -1){
                //TODO maybe do not compute pseudoinverse since the computation was moved into the PropagationModels classes
                typeComputations[typesIndexes[i]]->addEdges(typeInteractionsEdges.first[types[i]], undirectedTypeEdges, false); // no inverse computation since it is done in the propagation model
                //typeComputations[i]->freeAugmentedGraphs();
            }
        }
        for(auto edge = typeInteractionsEdges.second.cbegin() ; edge != typeInteractionsEdges.second.cend(); edge++ ){
            // first two types are the nodes in the two networks/types ,types are the third and fourth element of the tuple, while the fifth is the set of contact times
            std::pair<std::string,std::string> keyTypes = std::make_pair(std::get<2> (*edge), std::get<3> (*edge));
            if(interactionBetweenTypesMap.contains(keyTypes)){
                interactionBetweenTypesMap[keyTypes].insert(std::get<4>(*edge).begin(),std::get<4>(*edge).end()); // directly inserting means the union of the two sets
            } else {
                interactionBetweenTypesMap[keyTypes] = std::get<4>(*edge);
            }
        }
    }

    // setting propagation model in this moment since in the case of the original model, the pseudoinverse should be computed for the augmented pathway

    std::function<double(double)> propagationScalingFunction = [](double time)->double{return 1;};
    if(vm.count("propagationModel")){
        logger << "[LOG] propagation model was set to "
    << vm["propagationModel"].as<std::string>() << ".\n";
        std::string propagationModelName = vm["propagationModel"].as<std::string>();
        if(propagationModelName == "none"){
            logger << "[LOG] propagation model set to default (none)\n";
            for(uint i = 0; i < typesFiltered.size();i++ ){
                typeComputations[i]->setPropagationModel(new PropagationModelOriginal(typeComputations[i]->getAugmentedGraph(),propagationScalingFunction));
            }
            //nothing to do, default propagation scaling function is the identity
        } else if (propagationModelName == "scaled"){
            if (vm.count("propagationModelParameters")) {
                logger << "[LOG] propagation model parameters were declared to be "
            << vm["propagationModelParameters"].as<std::vector<double>>()[0] << ".\n";
                std::vector<double> propagationModelParameters = vm["propagationModelParameters"].as<std::vector<double>>();
                if(propagationModelParameters.size() == 1){
                    propagationScalingFunction = [propagationModelParameters](double time)->double{return propagationModelParameters[0];};
                    for(uint i = 0; i < typesFiltered.size();i++ ){
                        PropagationModel* tmpPropagationModel = new PropagationModelOriginal(typeComputations[i]->getAugmentedGraph(),propagationScalingFunction);
                        typeComputations[i]->setPropagationModel(tmpPropagationModel);
                    }
                } else {
                    std::cerr << "[ERROR] propagation model parameters for scaled propagation must be one parameter: aborting"<<std::endl;
                    return 1;
                }
            } else {
                std::cerr << "[ERROR] propagation model parameters for scaled propagation was not set: setting to default 1 costant"<<std::endl;
                for(uint i = 0; i < typesFiltered.size();i++ ){
                    PropagationModel* tmpPropagationModel = new PropagationModelOriginal(typeComputations[i]->getAugmentedGraph(),propagationScalingFunction);
                    typeComputations[i]->setPropagationModel(tmpPropagationModel);
                }
                //nothing to do, default propagation scaling function is the identity
            }
        } else if (propagationModelName == "neighbors"){
            if (vm.count("propagationModelParameters")) {
                logger << "[LOG] propagation model parameters were declared to be "
            << vm["propagationModelParameters"].as<std::vector<double>>()[0] << ".\n";
                std::vector<double> propagationModelParameters = vm["propagationModelParameters"].as<std::vector<double>>();
                if(propagationModelParameters.size() == 1){
                    propagationScalingFunction = [propagationModelParameters](double time)->double{return propagationModelParameters[0];};
                    for(uint i = 0; i < typesFiltered.size();i++ ){
                        PropagationModel* tmpPropagationModel = new PropagationModelNeighbors(typeComputations[i]->getAugmentedGraph(),propagationScalingFunction);
                        typeComputations[i]->setPropagationModel(tmpPropagationModel);
                    }
                } else {
                    std::cerr << "[ERROR] propagation model parameters for scaled propagation must be one parameter: aborting"<<std::endl;
                    return 1;
                }
            } else {
                std::cerr << "[ERROR] propagation model parameters for scaled propagation was not set: setting to default 1 costant"<<std::endl;
                for(uint i = 0; i < typesFiltered.size();i++ ){
                    PropagationModel* tmpPropagationModel = new PropagationModelNeighbors(typeComputations[i]->getAugmentedGraph(),propagationScalingFunction);
                    typeComputations[i]->setPropagationModel(tmpPropagationModel);
                }
                //nothing to do, default propagation scaling function is the identity
            }
        }  else if(propagationModelName == "customScaling"){ 
            if(vm.count("propagationModelParameters")){
                logger << "[LOG] propagation model parameters were declared to be "
                << vm["propagationModelParameters"].as<std::vector<double>>()[0] << ", these parameters are not used since the propagation scaling function was set to custom.\n";
            }
            propagationScalingFunction = getPropagationScalingFunction();
            for(uint i = 0; i < typesFiltered.size();i++ ){
                PropagationModel* tmpPropagationModel = new PropagationModelOriginal(typeComputations[i]->getAugmentedGraph(),propagationScalingFunction);
                typeComputations[i]->setPropagationModel(tmpPropagationModel);
            }
        } else if(propagationModelName == "customScalingNeighbors"){ 
            if(vm.count("propagationModelParameters")){
                logger << "[LOG] propagation model parameters were declared to be "
                << vm["propagationModelParameters"].as<std::vector<double>>()[0] << ", these parameters are not used since the propagation scaling function was set to custom.\n";
            }
            propagationScalingFunction = getPropagationScalingFunction();
            for(uint i = 0; i < typesFiltered.size();i++ ){
                PropagationModel* tmpPropagationModel = new PropagationModelNeighbors(typeComputations[i]->getAugmentedGraph(),propagationScalingFunction);
                typeComputations[i]->setPropagationModel(tmpPropagationModel);
            }
            
        } else if(propagationModelName == "customPropagation"){
            if(vm.count("propagationModelParameters")){
                logger << "[LOG] propagation model parameters were declared to be "
                << vm["propagationModelParameters"].as<std::vector<double>>()[0] << ", these parameters are not used since the propagation scaling function was set to custom.\n";
            }
            propagationScalingFunction = getPropagationScalingFunction();
            for(uint i = 0; i < typesFiltered.size();i++ ){
                PropagationModel* tmpPropagationModel = new PropagationModelCustom(typeComputations[i]->getAugmentedGraph(),propagationScalingFunction);
                typeComputations[i]->setPropagationModel(tmpPropagationModel);
            }
        
        } else {
            std::cerr << "[ERROR] propagation model is not any of the types. propagation model scale functions available are default, scaled, neighbors and custom \n";
            return 1;
        }
    } else {
        logger << "[LOG] propagation model was not set. set to default (none)\n";
        for(uint i = 0; i < typesFiltered.size();i++ ){
            PropagationModel* tmpPropagationModel = new PropagationModelOriginal(typeComputations[i]->getAugmentedGraph(),propagationScalingFunction);
            typeComputations[i]->setPropagationModel(tmpPropagationModel);
        }
    }


    //TESTING
    // logger<< "[DEBUG] adjacency matrix for type \"2\":"<<std::endl;
    // int index = -1;
    // for(int i = 0; i < SizeToInt(types.size());i++){
    //     if(types[i] == "2"){
    //         index = i;
    //         break;
    //     }
    // }
    // typeComputations[index]->getAugmentedGraph()->adjMatrix.printMatrix();
    // logger<< "[DEBUG] input vector for type \"2\":"<<std::endl;
    // std::vector<double> input = typeComputations[index]->getInputAugmented();
    // for(int i = 0; i < SizeToInt(input.size());i++){
    //     //logger << typeComputations[index]->getInput()[i] << ", ";
    //     logger << input[i] << ", ";
    // }
    // logger <<  ")\n";
    //TESTING


    //freeing some data structures inside computation to consume less RAM
    // std::vector<std::vector<std::string>> typeToNodeNames = std::vector<std::vector<std::string>>(types.size(),std::vector<std::string>());
    // for(uint i = 0; i < types.size();i++ ){
    //     typeToNodeNames[i] = typeComputations[i]->getAugmentedGraph()->getNodeNames();  
    //     typeComputations[i]->freeAugmentedGraphs();  
    // }

    // EndTypetype -> (sourceTypeType -> value)

    for(uint iterationInterType = 0; iterationInterType < intertypeIterations; iterationInterType++){
        for(uint iterationIntraType = 0; iterationIntraType < intratypeIterations; iterationIntraType++){
            #pragma omp parallel for
            for(uint i = 0; i < typesFiltered.size(); i++){
                std::vector<std::string> nodeNames = typeToNodeNames[i];
                logger << "[LOG] computation of perturbation for iteration intertype-intratype ("+ std::to_string(iterationInterType) + "<->"+ std::to_string(iterationIntraType) + ") for type (" + types[i]<<std::endl; 
                // TODO use stateful scaling function to consider previous times
                if (saturation) {
                    if(vm.count("saturationTerm") == 0){
                        // std::vector<double> outputValues = typeComputations[i]->computeAugmentedPerturbationEnhanced2((iterationIntertype*intratypeIterations + iterationIntratype)*timestep, saturation = true);
                        //std::vector<double> outputValues = typeComputations[i]->computeAugmentedPerturbationEnhanced3((iterationIntertype*intratypeIterations + iterationIntratype)*timestep, saturation = true, std::vector<double>(), std::vector<double>(), propagationScalingFunction);
                        std::vector<double> outputValues = typeComputations[i]->computeAugmentedPerturbationEnhanced4((iterationInterType*intratypeIterations + iterationIntraType)*timestep, saturation = true);
                    } else if (vm.count("saturationTerm") >= 1) {
                        double saturationTerm = vm["saturationTerm"].as<double>();
                        std::vector<double> saturationVector = std::vector<double>(graphsNodes[invertedTypesIndexes[i]].size(),saturationTerm);
                        // std::vector<double> outputValues = typeComputations[i]->computeAugmentedPerturbationEnhanced2((iterationIntertype*intratypeIterations + iterationIntratype)*timestep, saturation = true, saturationVector);
                        //std::vector<double> outputValues = typeComputations[i]->computeAugmentedPerturbationEnhanced3((iterationIntertype*intratypeIterations + iterationIntratype)*timestep, saturation = true, saturationVector, std::vector<double>(), propagationScalingFunction); 
                        std::vector<double> outputValues = typeComputations[i]->computeAugmentedPerturbationEnhanced4((iterationInterType*intratypeIterations + iterationIntraType)*timestep, saturation = true, saturationVector);
                    }
                } else{
                    // std::vector<double> outputValues = typeComputations[i]->computeAugmentedPerturbationEnhanced2((iterationIntertype*intratypeIterations + iterationIntratype)*timestep, saturation = false);
                    //std::vector<double> outputValues = typeComputations[i]->computeAugmentedPerturbationEnhanced3((iterationIntertype*intratypeIterations + iterationIntratype)*timestep, saturation = false, std::vector<double>(), std::vector<double>(), propagationScalingFunction);
                    std::vector<double> outputValues = typeComputations[i]->computeAugmentedPerturbationEnhanced4((iterationInterType*intratypeIterations + iterationIntraType)*timestep, saturation = false);
                }
            }
            //save output values
            for(uint i = 0; i < typesFiltered.size(); i++){
                std::vector<std::string> nodeNames = typeToNodeNames[i];
                //TODO change how to save files to get more information about intratype and intertype iterations
                saveNodeValues(outputFoldername, iterationInterType*intratypeIterations + iterationIntraType, typesFiltered[i], typeComputations[i]->getOutputAugmented(), nodeNames, nodesDescriptionFilename);
            }
            // logger<< "[DEBUG] output values before updating input"<<std::endl;
            // for(uint i = 0; i < types.size(); i++){
            //     logger << "[DEBUG] type " << types[i] << " values: ";
            //     for(uint j = 0; j < typeComputations[i]->getOutputAugmented().size(); j++){
            //         logger << typeComputations[i]->getOutputAugmented()[j] << " ";
            //     }
            //     logger << std::endl;
            // }

            //update input
            for(uint i = 0; i < typesFiltered.size(); i++){
                //If conservation of the initial values is required, the input is first updated with the initial norm value
                if (conservateInitialNorm) {
                    std::vector<double> inputInitial = inputInitials[i];
                    double initialNorm = vectorNorm(inputInitial);
                    double outputNorm = vectorNorm(typeComputations[i]->getOutputAugmented());
                    double normRatio = initialNorm/outputNorm;
                    std::vector<double> newInput = vectorScalarMultiplication(typeComputations[i]->getOutputAugmented(),normRatio);
                    logger << "[LOG] update input with conservation of the initial perturbation for iteration intertype-intratype ("+ std::to_string(iterationInterType) + "<->"+ std::to_string(iterationIntraType) + ") for type (" + types[i]<<std::endl;
                    typeComputations[i]->updateInput(newInput,true);
                } else {
                    logger << "[LOG] update input for iteration intertype-intratype ("+ std::to_string(iterationInterType) + "<->"+ std::to_string(iterationIntraType) + ") for type (" + types[i]<<std::endl;
                    typeComputations[i]->updateInput(std::vector<double>(),true);
                }
                
            }
            // logger<< "[DEBUG] input values after updating input"<<std::endl;
            // for(uint i = 0; i < types.size(); i++){
            //     logger << "[DEBUG] type " << types[i] << " values: ";
            //     for(uint j = 0; j < typeComputations[i]->getInputAugmented().size(); j++){
            //         logger << typeComputations[i]->getInputAugmented()[j] << " ";
            //     }
            //     logger << std::endl;
            // }
        }




        //update input with virtual node values update
        
        // logger<< "[DEBUG] input values before updating with virtual"<<std::endl;
        // for(uint i = 0; i < types.size(); i++){
        //     logger << "[DEBUG] type " << types[i] << " values: ";
        //     for(uint j = 0; j < typeComputations[i]->getInputAugmented().size(); j++){
        //         logger << typeComputations[i]->getInputAugmented()[j] << " ";
        //     }
        //     logger << std::endl;
        // }

        for (uint i = 0; i < typesFiltered.size(); i++) {
            for(uint j = 0; j < typesFiltered.size(); j++){
                std::pair<std::string,std::string> keyTypes = std::make_pair(typesFiltered[i], typesFiltered[j]);
                if(interactionBetweenTypesMap.contains(keyTypes)){
                    std::unordered_set<int> interactionTimes = interactionBetweenTypesMap[keyTypes];
                    if(interactionTimes.contains(iterationInterType)){
                        if(i==j){
                            if(sameTypeCommunication) typeComputations[i]->setInputVinForType(typesFiltered[j], typeComputations[j]->getVirtualOutputForType(typesFiltered[i]));
                        } else {
                            typeComputations[i]->setInputVinForType(typesFiltered[j], typeComputations[j]->getVirtualOutputForType(typesFiltered[i]));
                        }
                    }
                }
            }
        }
        // logger<< "[DEBUG] input values after updating with virtual"<<std::endl;
        // for(uint i = 0; i < types.size(); i++){
        //     logger << "[DEBUG] type " << types[i] << " values: ";
        //     for(uint j = 0; j < typeComputations[i]->getInputAugmented().size(); j++){
        //         logger << typeComputations[i]->getInputAugmented()[j] << " ";
        //     }
        //     logger << std::endl;
        // }


        // reset virtual outputs if specified
        if (resetVirtualOutputs) {
            for(uint i = 0; i < typesFiltered.size(); i++){
                typeComputations[i]->resetVirtualOutputs();
            }
        }

    }
    


    //cleaning memory
    // for(uint i = 0; i< types.size(); i++){
    //     delete typeComputations[i];
    // }
    // delete [] typeComputations;
    //delete graph
    

    
    return 0;
}