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

//custom types
typedef std::vector<int> NodeList;
using NodeBitList = std::vector<bool>;
using NodeBitArray = bool*;
using NodeSet = std::unordered_set<int>; 

std::ostream& operator<< (std::ostream &out, NodeBitList const& data);
std::ostream& operator<< (std::ostream &out, NodeList const& data);
std::ostream& operator<< (std::ostream &out, NodeSet const& data);

NodeList* nodeBitArrayToList(NodeBitArray const& nodeArray,int arraySize);
NodeSet* nodeBitArrayToSet(NodeBitArray const& nodeArray,int arraySize);

void printNodeBitArray(NodeBitArray nodeArray,int size);

template<typename T>
void printVector(std::vector<T> vec){
    for (uint i = 0; i < vec.size(); ++i) {
        std::cout << vec[i] << " , ";
    }
    std::cout << std::endl;
}

void printUsage(std::string execName);

std::string nodeBitArrayToString(NodeBitArray nodeArray,int size);

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
 * \brief   Control if the file exists in the current directory
 * \return  true if the file exists, false otherwise
 */
bool file_exists (const std::string& name);

/**
 * \brief   Control if the file exists in the specified path
 * \return  true if the file exists, false otherwise
 */
bool fileExistsPath(const std::string& filePath);
/**
 * \brief   Control if the folder exists in the specified path
 * \return  true if the folder exists, false otherwise
 */
bool folderExists(const std::string& folderPath);
/**
 * \brief   Create the folder in the specified path
 * \return  true if the folder is created, false otherwise
 */
bool createFolder(const std::string& folderPath);
/**
 * \brief   List the files in the specified folder
 * \return  the vector of the files in the folder
 */
std::vector<std::string> listFiles(const std::string& folderPath,bool noHiddenFiles=true, bool noFolders=true);

std::pair<std::vector<int>,std::vector<std::tuple<int,int,double>>> edgesFileToEdgesListByIndex(std::string filename);
std::pair<std::vector<std::string>,std::vector<std::tuple<std::string,std::string,double>>> edgesFileToEdgesListAndNodesByName(std::string filename);
std::pair<std::vector<std::string>,std::vector<std::pair<std::vector<std::string>,std::vector<std::tuple<std::string,std::string,double>>>>> edgesFileToEdgesListAndNodesByNameFromFolder(std::string filename);
std::tuple<std::vector<std::string>,std::vector<std::string>,std::vector<std::vector<double>>> valuesMatrixToTypeVectors(std::string filename, const std::vector<std::string>& finalNames,std::vector<std::string> subTypes);
std::tuple<std::vector<std::string>,std::vector<std::string>,std::vector<std::vector<double>>> valuesVectorsFromFolder(std::string folderPath,const std::vector<std::string>& allTypes, const std::vector<std::vector<std::string>>& finalNames,std::vector<std::string> subType = std::vector<std::string>());
std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>> interactionFileToEdgesListAndNodesByName(std::string filename);
std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>> interactionFileToEdgesListAndNodesByName(std::string filename, std::vector<std::string> subtypes);

std::map<std::string,std::vector<std::string>> nodeNamesFromFolder(std::string folderPath);

/**
 * \brief   Returns the new virtual nodes associated with a type along the edges in the augmented graph, it also return the graph of the interactions between types(as a tuple of:
 *         - the start type/agent
 *         - the end type/agent
 *         - the contact times of the interaction
 *         granularity needs to be specified as an argument
 * \return the pair (map of the new virtual nodes associated with a type, graph of the interactions between types)
 */
std::pair<std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>>,std::vector<std::tuple<std::string, std::string, std::string, std::string, std::unordered_set<int>, double>>> interactionContactsFileToEdgesListAndNodesByName(std::string filename, std::vector<std::string> subtypes,int maximumIntertypeTime=INT32_MAX, std::string granularity="", std::unordered_map<std::string,std::vector<std::string>> typeToNodeNames = std::unordered_map<std::string,std::vector<std::string>>(), bool undirectedTypeEdges = false);
/**
 * \brief   Returns the new virtual nodes associated with a type along the edges in the augmented graph, it also return the graph of the interactions between types(as a tuple of:
 *         - the start type/agent
 *         - the end type/agent
 *         - the contact times of the interaction, as an unordered set of doubles
 *         granularity needs to be specified as an argument
 * \return the pair (map of the new virtual nodes associated with a type, graph of the interactions between types)
 */
std::pair<std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>>,std::vector<std::tuple<std::string, std::string, std::string, std::string, std::set<double>, double>>> interactionContinuousContactsFileToEdgesListAndNodesByName(std::string filename, std::vector<std::string> subtypes,int maximumIntertypeTime=INT32_MAX, std::string granularity="", std::unordered_map<std::string,std::vector<std::string>> typeToNodeNames = std::unordered_map<std::string,std::vector<std::string>>(), bool undirectedTypeEdges = false, double timestep=1.0);
/**
 * \brief   Returns the new virtual nodes associated with a type along the edges in the augmented graph, uses doubles in principle for contacts it also return the graph of the interactions between types(as a tuple of:
 *         - the start type/agent
 *         - the end type/agent
 *         - the contact times of the interaction, as an unordered set of doubles
 *         granularity needs to be specified as an argument
 * \return the pair (map of the new virtual nodes associated with a type, graph of the interactions between types)
 */
std::pair<std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>>,std::vector<std::tuple<std::string, std::string, std::string, std::string, std::set<double>, double>>> interactionContinuousContactsFileToEdgesListAndNodesByName(std::string filename, std::vector<std::string> subtypes,double maximumIntertypeTime=DBL_MAX, std::string granularity="", std::unordered_map<std::string,std::vector<std::string>> typeToNodeNames = std::unordered_map<std::string,std::vector<std::string>>(), bool undirectedTypeEdges = false, double timestep=1.0);
std::vector<double> saturationFileToVector(std::string filename,const std::map<std::string, int>& ensembleToIndexMap);
/**
 * \brief   Return the types taken from the file names in a folder with the extension .tsv
 *          that is if the folder contains the files: A.tsv, B.tsv, C.tsv, D.tsv, E.tsv
 *         the function will return the vector {"A","B","C","D","E"}
 */
std::vector<std::string> getTypesFromFolderFileNames(std::string folderPath);
/**
 * \brief   Return the types taken from the first line of a file
 *          that is if the first line contains: name, A, B, C, D, E
 *         the function will return the vector {"A","B","C","D","E"}
 */
std::vector<std::string> getTypesFromMatrixFile(std::string matrixFilepath);

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

std::map<std::string, std::vector<std::string>> getFullNodesDescription(std::string filename); 
/**
 * \brief   Return the filenames of all files that have the specified extension
 *          in the specified directory and all subdirectories.
 */
std::vector<std::string> get_all(std::string const & root, std::string const & ext);
/**
 * \brief   Return map of the vector values from the first vector to the second vector
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
            std::cout << "[ERROR] utilities::get_indexmap_vector_values : " << toMap[i] << " not found in the origin vector" << std::endl;
            throw std::invalid_argument( "utilities::get_indexmap_vector_values : " + toMap[i] + " not found in the origin vector" );
        }
    }
    return retVec;
}

/**
 * \brief   Return map of the vector values from the second vector to the first vector, if the value is not found in the first vector the value -1 is added to the map
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
        std::cout << "[ERROR] utilities::get_indexmap_vector_values_full : all values not found in the origin vector" << std::endl;
        throw std::invalid_argument( "all values not found in the origin vector" );
    }
    if(notFoundValues>0){
        std::cout << "[WARNING] utilities::get_indexmap_vector_values_full : " << notFoundValues << " values not found in the origin vector" << std::endl;
        std::cout << "[WARNING] utilities::get_indexmap_vector_values_full : " << "values not found in the origin vector: ";
        printVector(notFoundValuesVector);
    }
    return retVec;
}

void saveNodeValues(std::string folderName,int iteration, std::string cellName, std::vector<double> nodeValues,std::vector<std::string> nodeNames, std::string nodesDescriptionFile="");

/**
 * \brief   save node values in folder
 *         better version of the above function
*/
void saveNodeValues(std::string folderName,int iterationOuter, int intraIteration, std::string cellName, std::vector<double> nodeValues,std::vector<std::string> nodeNames, std::string nodesDescriptionFile="");

/**
 * \brief   save node values in folder
 *         add times as an additional feature
*/
void saveNodeValuesWithTime(std::string folderName,int iterationOuter, int intraIteration, std::string cellName, std::vector<double> nodeValues,std::vector<std::string> nodeNames, std::string nodesDescriptionFile="", double timestep=1.0);

/**
 * \brief   save node values in folder, no info about intra-iteration and inter-iteration is passed
 *         add times as an additional feature
*/
void saveNodeValuesWithTimeSimple(std::string folderName, int currentIteration, double currentTime, std::string typeName, std::vector<double> nodeValues,std::vector<std::string> nodeNames, std::string nodesDescriptionFile="");


/**
 * \brief   save the iteration values passed as matrices in the folder
 * \param  folderName the folder where the values are saved
 * \param  outputMatrices a map (typeName)->(Matrix of values)
 * \param  nodeNames a map (typeName)->(vector of node names)
 * \param  interIteration the inter-iteration number
 * \param  intraIteration the intra-iteration number
 * \param  timestep the timestep of the simulation
 * \return void
 */
void saveOutputMatrix(std::string outputFolderNameMatrices, Matrix<double>* outputMatrix, std::vector<std::string> outputMatricesRowNames, int intertypeIterations, int intratypeIterations, double timestep, std::string typeName);

