/**
 * @file  utilities.hxx
 * @ingroup Core
 * @brief  This file contains the declaration of the utilities functions
 * @details  The functions are used to work on files, strings, vectors and matrices
 */
#pragma  once

#include <array>
#include <cstddef>
#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <vector>
#include <bitset>
#include <random>
#include <chrono>
#include <algorithm>
#include <limits>    // for std::numeric_limits
#include <stdexcept> // for std::overflow_error
#include <sys/stat.h>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <cmath>

// hash function for unordered_map
#include <bits/stdc++.h>

// my libraries
#include "data_structures/Matrix.hxx"
#include "utils/mathUtilities.hxx"
#include "utils/stringUtilities.hxx"
#include "CustomFunctions.hxx"

/**
 * @brief print a vector of any type
 * @param vec the vector to print
 * @details the vector is printed in the format 1,2,3
 */
template<typename T>
void printVector(std::vector<T> vec){
    for (uint i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << " , ";
    }
    std::cout << std::endl;
}

/**
 * @brief control for duplicates in a vector
 * @param v the vector to control
 * @return true if there are duplicates, false otherwise
 * @details the vector is sorted and then the unique function is used to remove duplicates
 */
template<typename T>
bool controlForDuplicates(std::vector<T> v){
    std::vector<T> v2 = v;
    std::sort(v2.begin(), v2.end());
    auto last = std::unique(v2.begin(), v2.end());
    v2.erase(last, v2.end());
    if(v.size()!=v2.size()){
        return true;
    }
    return false;
}


/**
 * @brief   Control if the file exists in the current directory
 * @param name the name of the file
 * @return  true if the file exists, false otherwise
 * @details  The file is checked using the stat function
 */
bool file_exists (const std::string& name);

/**
 * @brief   Control if the file exists in the specified path
 * @param filePath the path of the file
 * @return  true if the file exists, false otherwise
 * @details  The file is checked using the stat function
 * @note also check if the file is a regular file
 */
bool fileExistsPath(const std::string& filePath);
/**
 * @brief   Control if the folder exists in the specified path
 * @param folderPath the path of the folder
 * @return  true if the folder exists, false otherwise
 * @details  The folder is checked using the stat function
 */
bool folderExists(const std::string& folderPath);
/**
 * @brief   Create the folder in the specified path
 * @return  true if the folder is created, false otherwise
 * @param folderPath the path of the folder
 * @details  The folder is created using the create_directory function
 * @warning Sometimes the creation of the folder fails and the function still returns true
 * @note the function will return false if the folder already exists and also if the folder is not created, so this needs to be controlled (and probably the function should be changed)
 */
bool createFolder(const std::string& folderPath);
/**
 * @brief   List the files in the specified folder
 * @return  the vector of the files in the folder
 * @param folderPath the path of the folder
 * @param noHiddenFiles if true, the hidden files are not included in the list
 * @param noFolders if true, the folders are not included in the list
 * @details  The files are listed using the directory_iterator function
 */
std::vector<std::string> listFiles(const std::string& folderPath,bool noHiddenFiles=true, bool noFolders=true);

// reading from files
/**
 * @brief   Returns the edges list by index from a file
 * @param filename the name of the file
 * @return  the edges list of indices
 * @details  The file is read using the ifstream function
 * @note    The file must be in the format: node1 node2 weight
 */
std::pair<std::vector<int>,std::vector<std::tuple<int,int,double>>> edgesFileToEdgesListByIndex(std::string filename);
/**
 * @brief   Returns the edges list and nodes from a file
 * @param filename the name of the file
 * @return  the edges list and nodes as a pair of vectors (vector of strings for the names, vector of tuples for the edges)
 * @details  The file is read using the ifstream function
 * @note    The file must contain the following columns:
 *         - start/source
 *         - end/target
 *         - weight
 * @note Other columns are ignored
 * @throw std::invalid_argument if the file does not exist
 * @throw std::invalid_argument if the file does not contain the start/source, end/target or weight columns
 */
std::pair<std::vector<std::string>,std::vector<std::tuple<std::string,std::string,double>>> edgesFileToEdgesListAndNodesByName(std::string filename);
/**
 * @brief   Returns the edges list and nodes from the files in a folder
 * @param filename the name of the folder
 * @return  the edges list and nodes as a pair of vectors (vector of strings for the names, vector of tuples for the edges)
 * @details  The files are read using the function edgesFileToEdgesListAndNodesByName
 * @note    The files must contain the following columns:
 *        - start/source
 *        - end/target
 *        - weight
 * @see edgesFileToEdgesListAndNodesByName
 */
std::pair<std::vector<std::string>,std::vector<std::pair<std::vector<std::string>,std::vector<std::tuple<std::string,std::string,double>>>>> edgesFileToEdgesListAndNodesByNameFromFolder(std::string filename);
/**
 * @brief Convert a file where there is a matrix of values into a vector of vectors(each vector representing a type)
 * @param filename the name of the file
 * @param finalNames the names of the nodes
 * @param subTypes the names of the subtypes
 * @return  the vector of vectors of values
 * @details  The file is read using the ifstream function
 * @note    The file must be a matrix of values, where the first line is the header and contains the names of the types, first column is the names of the nodes
 *         and the rest of the columns are the values
 */
std::tuple<std::vector<std::string>,std::vector<std::string>,std::vector<std::vector<double>>> valuesMatrixToTypeVectors(std::string filename, const std::vector<std::string>& finalNames,std::vector<std::string> subTypes);
/**
 * @brief Read the files in a folder and return the values as a vector of vectors
 * @param folderPath the path of the folder
 * @param allTypes the names of the types
 * @param finalNames the names of the final types
 * @param subType the names of the subtypes
 * @return  the vector of vectors of values
 * @details The files contain the following columns: name value
 * @throw std::invalid_argument if the file does not exist
 * @throw std::invalid_argument if the file does not contain the name or value columns
 */
std::tuple<std::vector<std::string>,std::vector<std::string>,std::vector<std::vector<double>>> valuesVectorsFromFolder(std::string folderPath,const std::vector<std::string>& allTypes, const std::vector<std::vector<std::string>>& finalNames,std::vector<std::string> subType = std::vector<std::string>());
/**
 * @brief   Read the graph edges from a file and return the edges list and nodes as a pair of vectors (vector of strings for the names, vector of tuples for the edges)
 * @param filename the name of the file
 * @return  the edges list and nodes as a pair of vectors (vector of strings for the names, vector of tuples for the edges)
 * @details  The file is read using the ifstream function
 * @note   The file must contain the following columns: startType, endType, startNodeName, endNodeName, weight
 * @throw std::invalid_argument if the file does not exist
 * @throw std::invalid_argument if the file does not contain the startType, endType, startNodeName, endNodeName or weight columns
 * @note Other columns are ignored
 */
std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>> interactionFileToEdgesListAndNodesByName(std::string filename);
/**
 * @brief   Read the graph edges from a file and return the edges list and nodes as a pair of vectors (vector of strings for the names, vector of tuples for the edges), considering the subtypes
 * @param filename the name of the file
 * @param subtypes the subtypes to filter the edges
 * @return  the edges list and nodes as a pair of vectors (vector of strings for the names, vector of tuples for the edges)
 * @details  The file is read using the ifstream function
 * @note   The file must contain the following columns: startType, endType, startNodeName, endNodeName, weight
 * @throw std::invalid_argument if the file does not exist
 * @throw std::invalid_argument if the file does not contain the startType, endType, startNodeName, endNodeName or weight columns
 * @note Other columns are ignored
 */
std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>> interactionFileToEdgesListAndNodesByName(std::string filename, std::vector<std::string> subtypes);

/**
 * @brief Read the node names from the names in a folder
 * @param folderPath the path of the folder
 * @return  the map of the node names <type, vector of node names>
 * @note The files must contain the column : name
 * @throw std::invalid_argument if the file does not exist
 * @throw std::invalid_argument if the file does not contain the name column
 * @note Other columns are ignored
 */
std::map<std::string,std::vector<std::string>> nodeNamesFromFolder(std::string folderPath);

/**
 * @brief   Returns the new virtual nodes associated with a type along the edges in the augmented graph, it also return the graph of the interactions between types(as a tuple of:
 *         - the start type/agent
 *         - the end type/agent
 *         - the contact times of the interaction
 *         granularity needs to be specified as an argument
 * @return the pair (map of the new virtual nodes associated with a type, graph of the interactions between types)
 * @param filename the name of the file
 * @param subtypes the subtypes to filter the edges
 * @param maximumIntertypeTime the maximum time between two interactions
 * @param granularity the granularity of the interactions, can be "type" or "node"
 * @param typeToNodeNames the map of the node names <type, vector of node names>
 * @param undirectedTypeEdges if true, the edges are undirected
 * @note The file must contain the following columns: startType, endType, startNodeName, endNodeName, weight
 * @note File could also contain the column contactTimes, which is a vector of times
 * @warning This function is deprecated, use interactionContinuousContactsFileToEdgesListAndNodesByName instead
 * @throw std::invalid_argument if the file does not exist
 * @throw std::invalid_argument if the file does not contain the startType, endType, startNodeName, endNodeName or weight columns
 */
std::pair<std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>>,std::vector<std::tuple<std::string, std::string, std::string, std::string, std::unordered_set<int>, double>>> interactionContactsFileToEdgesListAndNodesByName(std::string filename, std::vector<std::string> subtypes,int maximumIntertypeTime=INT32_MAX, std::string granularity="", std::unordered_map<std::string,std::vector<std::string>> typeToNodeNames = std::unordered_map<std::string,std::vector<std::string>>(), bool undirectedTypeEdges = false);
/**
 * @brief   Returns the new virtual nodes associated with a type along the edges in the augmented graph, it also return the graph of the interactions between types(as a tuple of:
 *         - the start type/agent
 *         - the end type/agent
 *         - the contact times of the interaction, as an unordered set of doubles
 *         granularity needs to be specified as an argument
 * @return the pair (map of the new virtual nodes associated with a type, graph of the interactions between types)
 * @param filename the name of the file
 * @param subtypes the subtypes to filter the edges
 * @param maximumIntertypeTime the maximum time between two interactions
 * @param granularity the granularity of the interactions, can be "type" or "node"
 * @param typeToNodeNames the map of the node names <type, vector of node names>
 * @param undirectedTypeEdges if true, the edges are undirected
 * @param timestep the time step of the simulation
 * @note The file must contain the following columns: startType, endType, startNodeName, endNodeName, weight
 * @note File could also contain the column contactTimes, which is a vector of times
 * @throw std::invalid_argument if the file does not exist
 * @throw std::invalid_argument if the file does not contain the startType, endType, startNodeName, endNodeName or weight columns
 * @note Other columns are ignored
 * @warning This function is also technically deprecated, since the version with double as the maximum intertype time is used
 */
std::pair<std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>>,std::vector<std::tuple<std::string, std::string, std::string, std::string, std::set<double>, double>>> interactionContinuousContactsFileToEdgesListAndNodesByName(std::string filename, std::vector<std::string> subtypes,int maximumIntertypeTime=INT32_MAX, std::string granularity="", std::unordered_map<std::string,std::vector<std::string>> typeToNodeNames = std::unordered_map<std::string,std::vector<std::string>>(), bool undirectedTypeEdges = false, double timestep=1.0);
/**
 * @brief   Returns the new virtual nodes associated with a type along the edges in the augmented graph, uses doubles in principle for contacts it also return the graph of the interactions between types(as a tuple of:
 *         - the start type/agent
 *         - the end type/agent
 *         - the contact times of the interaction, as an unordered set of doubles
 *         granularity needs to be specified as an argument
 * @return the pair (map of the new virtual nodes associated with a type, graph of the interactions between types)
 * @param filename the name of the file
 * @param subtypes the subtypes to filter the edges
 * @param maximumIntertypeTime the maximum time between two interactions
 * @param granularity the granularity of the interactions, can be "type" or "node"
 * @param typeToNodeNames the map of the node names <type, vector of node names>
 * @param undirectedTypeEdges if true, the edges are undirected
 * @param timestep the time step of the simulation
 * @note The file must contain the following columns: startType, endType, startNodeName, endNodeName, weight
 * @note File could also contain the column contactTimes, which is a vector of times
 * @throw std::invalid_argument if the file does not exist
 * @throw std::invalid_argument if the file does not contain the startType, endType, startNodeName, endNodeName or weight columns
 * @note Other columns are ignored
 */
std::pair<std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>>,std::vector<std::tuple<std::string, std::string, std::string, std::string, std::set<double>, double>>> interactionContinuousContactsFileToEdgesListAndNodesByName(std::string filename, std::vector<std::string> subtypes,double maximumIntertypeTime=DBL_MAX, std::string granularity="", std::unordered_map<std::string,std::vector<std::string>> typeToNodeNames = std::unordered_map<std::string,std::vector<std::string>>(), bool undirectedTypeEdges = false, double timestep=1.0);
/**
 * @brief Read the saturation file and return the saturation values as a vector of doubles
 * @param filename the name of the file
 * @param nodeToIndexMap the map of the node names to the indices
 * @return  the vector of saturation values
 * @details  The file is read using the ifstream function
 * @note    The file must contain the following columns: name, saturation
 * @warning Function is not implemented yet
 * @warning The function is currently not used, TODO add the function to the main program
 */
std::vector<double> saturationFileToVector(std::string filename,const std::map<std::string, int>& nodeToIndexMap);
/**
 * @brief Read the dissipation scaling function from a file
 * @param filename the name of the file
 * @return  the vectorized dissipation scaling function as a lambda function
 * @details  The file is read using the ifstream function
 * @note    The file must contain the following columns: node, <parameters>
 * @note This function returns the custom dissipation scaling function that is defined in \ref CustomFunctions.hxx
 * @note The nodes that are not in the orderedNodeNames vector will be ignored
 * @note The nodes that are not seen in the file will have a scaling function defined with the default one in CustomFunctions.hxx (getDissipationScalingFunction())
 * @throw std::invalid_argument if the file does not exist
 * @throw std::invalid_argument if the file does not contain the node or parameters columns
 * @throw std::runtime_error if the parameters are not valid for the dissipation scaling function (the function expects a specific format for the parameters)
 */
std::function<arma::Col<double>(double)> dissipationScalingFunctionFromFile(std::string filename, std::vector<std::string> orderedNodeNames);
/**
 * @brief Returns a vector of vectorized dissipation scaling functions from a folder
 * @param folderPath the path of the folder
 * @param typeToOrderedNodeNames the map of the node names <type, vector of node names>
 * @return  the vector of dissipation scaling functions
 * @details  The files are read using the dissipationScalingFunctionFromFile function
 * @details Also handles the case where the file for a specific type does not exist, in which case the default dissipation scaling function is used
 * @see dissipationScalingFunctionFromFile
 */
std::map<std::string, std::function<arma::Col<double>(double)>> dissipationScalingFunctionsFromFolder(std::string folderPath, std::map<std::string, std::vector<std::string>> typeToOrderedNodeNames);
/**
 * @brief   Return the types taken from the file names in a folder with the extension .tsv
 *          that is if the folder contains the files: A.tsv, B.tsv, C.tsv, D.tsv, E.tsv
 *         the function will return the vector {"A","B","C","D","E"}
 * @param folderPath the path of the folder
 * @return  the vector of types
 * @details  The files are listed using the directory_iterator function
 */
std::vector<std::string> getTypesFromFolderFileNames(std::string folderPath);
/**
 * @brief   Return the types taken from the first line of a file
 *          that is if the first line contains: name, A, B, C, D, E
 *         the function will return the vector {"A","B","C","D","E"}
 * @param matrixFilepath the path to the file
 * @return  the vector of types
 * @details  The file is read using the ifstream function
 * @note    The file must contain the following columns: name, \<type1\>, \<type2\>, \<type3\>, ...
 */
std::vector<std::string> getTypesFromMatrixFile(std::string matrixFilepath);

/**
 * @brief   Return the vector of values from a file
 * @param filename the name of the file
 * @return  the vector of values
 * @details  The file is read using the ifstream function
 */
template<typename T>
std::vector<T> getVectorFromFile(std::string filename){
    std::vector<T> retVec;
    std::ifstream inFile(filename);
    std::string line;
    while (std::getline(inFile, line))
    {
        std::stringstream ss(line);
        T temp;
        ss >> temp;
        retVec.push_back(temp);
    }
    inFile.close();
    return retVec;
}

/**
 * @brief   Return the full nodes description from a file
 * @param filename the name of the file
 * @return  the map of the full nodes description, that is <node name, vector of node attributes>
 * @details  The file is read using the ifstream function
 * @throw std::invalid_argument if the file does not exist
 * @note The file must contain the following columns: Id	Name	Type	Aliases
 * @warning This function is deprecated and not used anymore
 */
std::map<std::string, std::vector<std::string>> getFullNodesDescription(std::string filename); 
/**
 * @brief   Return the filenames of all files that have the specified extension
 *          in the specified directory and all subdirectories.
 * @param root the path of the folder
 * @param ext the extension of the files
 * @return  the vector of filenames
 * @details  The files are listed using the directory_iterator function
 * @note    The files are listed using the recursive_directory_iterator function
 * @note    The files are listed using the path function
 */
std::vector<std::string> get_all(std::string const & root, std::string const & ext);
/**
 * @brief   Return map of the vector values from the first vector to the second vector
 * @param origin the vector to map from
 * @param toMap the vector to map to
 * @return  the map of the vector values
 * @details  The function uses the std::find function to find the values in the first vector
 * @note    The function throws an exception if the value is not found in the first vector
 * @warning This function is deprecated and not used anymore
 */
template<typename T>
std::vector<int> get_indexmap_vector_values(std::vector<T> const & origin, std::vector<T> const & toMap){
    std::vector<int> retVec;
    for (int i = 0; i < toMap.size(); ++i) {
        auto it = std::find(origin.begin(), origin.end(), toMap[i]);
        if (it != origin.end()) {
            retVec.push_back(std::distance(origin.begin(), it));
        }
        else {
            Logger::getInstance().printError("utilities::get_indexmap_vector_values : " + toMap[i] + " not found in the origin vector");
            throw std::invalid_argument( "utilities::get_indexmap_vector_values : " + toMap[i] + " not found in the origin vector" );
        }
    }
    return retVec;
}

/**
 * @brief   Return map of the vector values from the second vector to the first vector, if the value is not found in the first vector the value -1 is added to the map
 * @param origin the vector to map from
 * @param toMap the vector to map to
 * @return  the map of the vector values
 * @details  The function uses the std::find function to find the values in the first vector
 * @note    The function throws an exception if all the values are not found in the first vector
 * @throw std::invalid_argument if all the values are not found in the first vector
 */
template<typename T>
std::vector<int> get_indexmap_vector_values_full(std::vector<T> const & origin, std::vector<T> const & toMap){
    std::vector<int> retVec;
    std::vector<T> notFoundValuesVector;
    uint notFoundValues = 0;
    for (uint i = 0; i < origin.size(); ++i) {
        auto itorigin = std::find(toMap.begin(), toMap.end(), origin[i]);
        if (itorigin != toMap.end()) {
            retVec.push_back(std::distance(toMap.begin(), itorigin));
        }
        else {
            retVec.push_back(-1);
            notFoundValuesVector.push_back(origin[i]);
            notFoundValues++;
        }
        
    }
    if(notFoundValues == origin.size()){
        Logger::getInstance().printError("utilities::get_indexmap_vector_values_full : all values not found in the origin vector");
        throw std::invalid_argument( "all values not found in the origin vector" );
    }
    if(notFoundValues>0){
        Logger::getInstance().printWarning("utilities::get_indexmap_vector_values_full : " + std::to_string(notFoundValues) + " values not found in the origin vector");
        printVector(notFoundValuesVector);
    }
    return retVec;
}

/**
 * @brief   save node values in folder
 * @param  folderName the folder where the values are saved
 * @param  iteration the iteration number
 * @param  cellName the name of the cell
 * @param  nodeValues the vector of node values
 * @param  nodeNames the vector of node names
 * @param  nodesDescriptionFile the file with the nodes description
 * @warning This function is deprecated and not used anymore
 */
void saveNodeValues(std::string folderName,int iteration, std::string cellName, std::vector<double> nodeValues,std::vector<std::string> nodeNames, std::string nodesDescriptionFile="");

/**
 * @brief   save node values in folder
 *         better version of the above function
 * @warning This function is deprecated and not used anymore
*/
void saveNodeValues(std::string folderName,int iterationOuter, int intraIteration, std::string cellName, std::vector<double> nodeValues,std::vector<std::string> nodeNames, std::string nodesDescriptionFile="");

/**
 * @brief   save node values in folder
 *         add times as an additional feature
 * @warning This function is deprecated and not used anymore
*/
void saveNodeValuesWithTime(std::string folderName,int iterationOuter, int intraIteration, std::string cellName, std::vector<double> nodeValues,std::vector<std::string> nodeNames, std::string nodesDescriptionFile="", double timestep=1.0);

/**
 * @brief   save node values in folder, no info about intra-iteration and inter-iteration is passed
 *         add times as an additional feature
 * @param  folderName the folder where the values are saved
 * @param  currentIteration the current iteration number
 * @param  currentTime the current time
 * @param  typeName the type name
 * @param  nodeValues the vector of node values
 * @param  nodeNames the vector of node names
 * @param  nodesDescriptionFile the file with the nodes description
*/
void saveNodeValuesWithTimeSimple(std::string folderName, int currentIteration, double currentTime, std::string typeName, std::vector<double> nodeValues,std::vector<std::string> nodeNames, std::string nodesDescriptionFile="");


/**
 * @brief   save the iteration values passed as matrices in the folder
 * @param  outputFolderNameMatrices the folder where the matrices are saved
 * @param  outputMatrix the matrix to save 
 * @param  outputMatricesRowNames the names of the rows of the matrix
 * @param  intertypeIterations the number of intertype iterations
 * @param  intratypeIterations the number of intratype iterations
 * @param  timestep the time step of the simulation
 * @param  typeName the type name
 * @details the matrices are saved in the folder with the name outputFolderNameMatrices
 *         the matrices are saved in the format: outputFolderNameMatrices/typeName.tsv
 */
void saveOutputMatrix(std::string outputFolderNameMatrices, Matrix<double>* outputMatrix, std::vector<std::string> outputMatricesRowNames, int intertypeIterations, int intratypeIterations, double timestep, std::string typeName);

