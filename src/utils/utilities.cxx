/**
 * @file utilities.cxx
 * @ingroup Core
 * @brief  This file contains the implementation of the utilities functions
 * @details  The functions are used to work on files, strings, vectors and matrices
 */
#include "utils/utilities.hxx"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <iterator>
#include <map>
#include <math.h>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;


bool file_exists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

bool fileExistsPath(const std::string& filePath)
{
    struct stat info;
    
    // Call the stat function to check if the file exists
    if (stat(filePath.c_str(), &info) != 0) {
        // The stat function returned an error, so the file does not exist
        return false;
    }
    
    // Check if the path corresponds to a regular file
    return (info.st_mode & S_IFREG) != 0;
}

bool folderExists(const std::string& folderPath)
{
    // Check if the folder exists
    bool exists = std::filesystem::is_directory(folderPath);
    
    // Return the result
    return exists;
}

bool createFolder(const std::string& folderPath)
{
    // Check if the folder already exists
    if (folderExists(folderPath)) {
        // The folder already exists
        return true;
    }
    
    // Create the folder
    bool success = std::filesystem::create_directory(folderPath);
    
    // Return the result
    return success;
}


std::vector<std::string> listFiles(const std::string& folderPath,bool noHiddenFiles, bool noFolders){
    std::vector<std::string> files;
    for (const auto & entry : std::filesystem::directory_iterator(folderPath)){
        if(noHiddenFiles && entry.path().filename().string().at(0)=='.'){
            continue;
        }
        if(noFolders && entry.is_directory()){
            continue;
        }
        files.push_back(entry.path().string());
    }
    return files;
}

std::pair<std::vector<int>,std::vector<std::tuple<int,int,double>>> edgesFileToEdgesListByIndex(std::string filename){
    string line;
    std::vector<std::tuple<int,int,double>> ret;
    std::vector<int> nameRet;
    std::unordered_set<int> presentNames;
    if(file_exists(filename)){
        ifstream myfile (filename);
        if (myfile.is_open())
        {
            while ( getline (myfile,line) )
            {
                std::vector<std::string> entries = splitStringIntoVector(line, "\t");
                if(entries.size()==3){
                    int node1 = std::stoi( entries[0]);
                    int node2 = std::stoi(entries[1]);
                    double weight = std::stod( entries[2]);
                    std::tuple<int,int,double> edge(node1,node2,weight);
                    ret.push_back(edge);
                    if(!presentNames.contains(node1)){
                        nameRet.push_back(node1);
                        presentNames.insert(node1);
                    }
                    if(!presentNames.contains(node2)){
                        nameRet.push_back(node2);
                        presentNames.insert(node2);

                    }
                }
            }
            myfile.close();
        }
    } else {
        throw std::invalid_argument("utilities::edgeFileEdgesListByIndex: file does not exists");
    }
    return std::pair<std::vector<int>,std::vector<std::tuple<int,int,double>>> (nameRet,ret);
}

std::pair<std::vector<std::string>,std::vector<std::tuple<std::string,std::string,double>>> edgesFileToEdgesListAndNodesByName(std::string filename){
    string line;
    std::vector<std::tuple<std::string,std::string,double>> ret;
    std::vector<std::string> nameRet;
    std::unordered_set<std::string> presentNames;
    if(file_exists(filename)){
        ifstream myfile (filename);
        if (myfile.is_open())
        {
            getline (myfile,line);  // first line is header IMPORTANT
            std::vector<std::string> entriesHeader = splitStringIntoVector(line, "\t");
            int indexStart=-1,indexEnd=-1,indexWeight=-1;
            for(uint i = 0; i < entriesHeader.size(); i++){
                if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("start") != std::string::npos || boost::algorithm::to_lower_copy(entriesHeader[i]).find("source") != std::string::npos) {
                    indexStart = i;
                }
                else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("end") != std::string::npos || boost::algorithm::to_lower_copy(entriesHeader[i]).find("target") != std::string::npos){
                    indexEnd = i;
                } else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("weight") != std::string::npos) {
                    indexWeight = i;
                }
            }

            if(indexStart < 0 || indexEnd < 0 || indexWeight < 0){
                if(entriesHeader.size()==3){
                    indexStart = 0;
                    indexEnd = 1;
                    indexWeight = 2;
                    Logger::getInstance().printWarning("using the first, second and third column as start, end and weight in the graph file:" + filename);
                } else {
                    std::string error = "utilities::edgesFileToEdgesListAndNodesByName: header of file" + filename + " does not contain start, end or weight";
                    throw std::invalid_argument(error);
                }
            }
            while ( getline (myfile,line) )
            {
                std::vector<std::string> entries = splitStringIntoVector(line, "\t");
                if(entries.size()==entriesHeader.size()){
                    std::string node1 = entries[indexStart];
                    std::string node2 = entries[indexEnd];
                    double weight = std::stod( entries[indexWeight]);
                    std::tuple<std::string,std::string,double> edge(node1,node2,weight);
                    ret.push_back(edge);
                    if(!presentNames.contains(node1)){
                        nameRet.push_back(node1);
                        presentNames.insert(node1);
                    }
                    if(!presentNames.contains(node2)){
                        nameRet.push_back(node2);
                        presentNames.insert(node2);

                    }
                } else {
                    Logger::getInstance().printError("utilities::edgesFileToEdgesListAndNodesByName: header doesn't have the same amount of columns as the data for file " + filename);
                    Logger::getInstance().printError("line: " + line);
                    Logger::getInstance().printError("header size: " + std::to_string(entriesHeader.size()));
                    Logger::getInstance().printError("line size: " + std::to_string(entries.size()));
                    throw std::invalid_argument("utilities::edgesFileToEdgesListAndNodesByName: header doesn't have the same amount of columns as the data " + filename);
                }
            }
            // control if resulting edges vector is empty
            if(ret.size()==0){
                Logger::getInstance().printWarning("edgesFileToEdgesListAndNodesByName: no edges found in the file " + filename + " .Use the nodeDescriptionFolder parameter to pass the graphs nodes, otherwise an error will occur");
            }
            myfile.close();
        }
    } else {
        throw std::invalid_argument("utilities::edgeFileEdgesListByIndex: file does not exists " + filename);
    }
    return std::pair<std::vector<std::string>,std::vector<std::tuple<std::string,std::string,double>>> (nameRet,ret);
}

std::pair<std::vector<std::string>,std::vector<std::pair<std::vector<std::string>,std::vector<std::tuple<std::string,std::string,double>>>>> edgesFileToEdgesListAndNodesByNameFromFolder(std::string filename){
    std::vector<std::pair<std::vector<std::string>,std::vector<std::tuple<std::string,std::string,double>>>> ret;
    std::vector<std::string> graphNames;
    std::vector<std::string> files = get_all(filename,".tsv");
    for(auto iter = files.cbegin(); iter!=files.cend();iter++){
        std::pair<std::vector<std::string>,std::vector<std::tuple<std::string,std::string,double>>> edgesListAndNodesByName = edgesFileToEdgesListAndNodesByName(*iter);
        std::vector<std::string> splitted = splitStringIntoVector(*iter, "/"); //split the path
        std::string filename = splitted[splitted.size()-1]; //last element
        std::vector<std::string> splittedFilename = splitStringIntoVector(filename, "."); //split the extension
        graphNames.push_back(splittedFilename[0]);
        ret.push_back(edgesListAndNodesByName);
    }
    return std::pair<std::vector<std::string>,std::vector<std::pair<std::vector<std::string>,std::vector<std::tuple<std::string,std::string,double>>>>>(graphNames,ret);
}

std::vector<std::string> getTypesFromFolderFileNames(std::string folderPath){
    std::vector<std::string> ret;
    std::vector<std::string> files = get_all(folderPath,".tsv");
    for(auto iter = files.cbegin(); iter!=files.cend();iter++){
        std::vector<std::string> splitted = splitStringIntoVector(*iter, "/"); //split the path
        std::string filename = splitted[splitted.size()-1]; //last element
        std::vector<std::string> splittedFilename = splitStringIntoVector(filename, "."); //split the extension
        ret.push_back(splittedFilename[0]);
    }
    return ret;
}

std::vector<std::string> getTypesFromMatrixFile(std::string matrixFilepath){
    std::vector<std::string> typeNames;
    std::string line;
    if(file_exists(matrixFilepath)){
        ifstream myfile (matrixFilepath);
        if (myfile.is_open())
        {
            getline (myfile,line);  // first line is header IMPORTANT
            std::vector<std::string> splittedHeader = splitStringIntoVector(line, "\t");  //could already be used as the cellnames vector,
            for (int i = 1; i < SizeToInt( splittedHeader.size()); i++) {
                typeNames.push_back(splittedHeader[i]);
            }
            myfile.close();
        }
        //check for duplicates
        if(controlForDuplicates(typeNames)){
            throw std::invalid_argument("utilities::getTypesFromMatrixFile: duplicate types in the matrix file, aborting " + matrixFilepath);
        }
    } else {
        throw std::invalid_argument("utilities::getTypesFromMatrixFile: file does not exists " + matrixFilepath);
    }
    return typeNames;
}

std::tuple<std::vector<std::string>,std::vector<std::string>,std::vector<std::vector<double>>> valuesMatrixToTypeVectors(std::string filename, const std::vector<std::string>& finalNames,std::vector<std::string> subTypes){
    string line;
    std::vector<std::vector<double>> ret;
    std::vector<std::string> typeNames;
    std::vector<std::string> nodeNames;
    std::vector<std::string> discardedGenes;
    std::map<std::string, int> finalNodesToIndex;
    for(int i = 0 ; i < SizeToInt(finalNames.size()); i++){
        finalNodesToIndex[finalNames[i]] = i;
    }
    if(file_exists(filename)){
        ifstream myfile (filename);
        if (myfile.is_open())
        {
            getline (myfile,line);  // first line is header IMPORTANT
            std::vector<std::string> splittedHeader = splitStringIntoVector(line, "\t");  //could already be used as the typenames vector,
            std::vector<int> subTypeIndexes;
            for (int i = 1; i < SizeToInt( splittedHeader.size()); i++) {
                if(vectorContains(subTypes,splittedHeader[i])){
                    typeNames.push_back(splittedHeader[i]);
                    subTypeIndexes.push_back(i);
                    ret.push_back(std::vector<double>(finalNames.size(),0));
                }
            }
            while ( getline (myfile,line) )
            {
                std::vector<std::string> entries = splitStringIntoVector(line, "\t");
                if(entries.size()==splittedHeader.size()){
                    nodeNames.push_back(entries[0]);
                    for(uint i = 0; i<subTypeIndexes.size();i++){
                        ret[i][finalNodesToIndex[entries[0]]] = std::stod(entries[subTypeIndexes[i]]);
                    }
                } else {
                    throw std::invalid_argument("utilities::valuesMatrixToTypeVectors: header doesn't have the same amount of columns as the data " + filename);
                }
            }
            myfile.close();
            Logger::getInstance().printLog(" No nodes in the graph for nodes: ");
            for(auto iter = discardedGenes.cbegin();iter!=discardedGenes.cend();iter++){
                std::cout << "," << *iter;
            }
            std::cout << std::endl;
            Logger::getInstance().printLog("discarding values for the nodes not in the graph");
            
        }
    } else {
        throw std::invalid_argument("utilities::valuesMatrixToTypeVectors: file does not exists " + filename);
    }
    return std::tuple<std::vector<std::string>,std::vector<std::string>,std::vector<std::vector<double>>> (nodeNames,typeNames,ret);

}

std::tuple<std::vector<std::string>,std::vector<std::string>,std::vector<std::vector<double>>> valuesVectorsFromFolder(std::string folderPath,const std::vector<std::string>& allTypes, const std::vector<std::vector<std::string>>& finalNames,std::vector<std::string> subType){
    std::vector<std::string> typeNames;
    std::vector<std::string> nodeNames;
    std::vector<std::vector<double>> ret;
    std::map<std::string ,std::map<std::string, int>> finalNodesToIndex;
    for(int i = 0 ; i < SizeToInt(finalNames.size()); i++){
        std::map<std::string, int> tmp;
        for(int j = 0 ; j < SizeToInt(finalNames[i].size()); j++){
            tmp[finalNames[i][j]] = j;
        }
        finalNodesToIndex[allTypes[i]]= tmp;
    }
    auto files = get_all(folderPath,".tsv");
    if(files.size()==0){
        throw std::invalid_argument("utilities::valuesVectorsFromFolder: no files found in the folder " + folderPath);
    }
    //default argument for subtype is empty, if empty, use all files in the folder
    if(subType.size()==0){
        for(auto iter = files.cbegin();iter!=files.cend();iter++){
            std::vector<std::string> splitted = splitStringIntoVector(*iter, "/"); //split the path
            std::string filename = splitted[splitted.size()-1]; //last element
            subType.push_back(splitStringIntoVector(filename, ".")[0]);
        }
    }
    //filter files from subtypes (first part of the filename before the extension)
    std::vector<std::string> filteredFiles;
    for(auto iter = files.cbegin();iter!=files.cend();iter++){
        std::vector<std::string> fileSplitPath = splitStringIntoVector(*iter, "/");
        std::string filename = fileSplitPath[fileSplitPath.size()-1];
        std::string type = splitStringIntoVector(filename, ".")[0];
        if(vectorContains(subType,type)){
            filteredFiles.push_back(*iter);
        } else {
            Logger::getInstance().printError("discarding file " + *iter + " since it is not in the subtypes");
        }
    }
    if(filteredFiles.size()==0){
        throw std::invalid_argument("utilities::valuesVectorsFromFolder: no files found in the folder that are similar to the subtypes " + folderPath);
    }
    for(auto iter = filteredFiles.cbegin();iter!=filteredFiles.cend();iter++){
        std::vector<std::string> splitted = splitStringIntoVector(*iter, "/"); //split the path
        std::string filenameNoPath = splitted[splitted.size()-1]; //last element
        std::string cellName = splitStringIntoVector(filenameNoPath, ".")[0];
        typeNames.push_back(cellName);
        std::string filename = *iter;
        if(file_exists(filename)){
            // discarded nodes for the single type
            std::vector<std::string> discardedNodes;
            //first line is the header, the first column is the gene, the second column is the value
            ifstream myfile (filename);
            string line;
            std::vector<double> cellValues(finalNodesToIndex[cellName].size(),0);
            std::string lineHeader;
            int indexName = -1, indexValue = -1;
            getline (myfile,lineHeader);  // first line is header IMPORTANT
            std::vector<std::string> splittedHeader = splitStringIntoVector(lineHeader, "\t");
            //check if the header is correct, meaning that it should have the right names for some columns
            for(uint i = 0; i < splittedHeader.size(); i++){
                if (boost::algorithm::to_lower_copy(splittedHeader[i]).find("name") != std::string::npos) {
                    indexName = i;
                }
                else if (boost::algorithm::to_lower_copy(splittedHeader[i]).find("value") != std::string::npos) {
                    indexValue = i;
                }
            }
            if(indexName < 0 || indexValue < 0){
                if(splittedHeader.size()==2){
                    indexName = 0;
                    indexValue = 1;
                    Logger::getInstance().printWarning("using the first and second column as name and value in the graph file:" + filename);
                } else {
                    std::string error = "utilities::valuesVectorsFromFolder: header of file" + filename + " does not contain name and value";
                    throw std::invalid_argument(error);
                }
            }
            // if(splittedHeader.size()!=2){
            //     throw std::invalid_argument("utilities::valuesVectorsFromFolder: header doesn't have the same amount of columns as the data " + filename);
            // }
            // if(splittedHeader[0]!="name" || splittedHeader[1] != "value"){
            //     throw std::invalid_argument("utilities::valuesVectorsFromFolder: header doesn't have the name and value columns or it does not have  an header" + filename);
            // }
            //get file contents
            while ( getline (myfile,line) )
            {
                std::vector<std::string> entries = splitStringIntoVector(line, "\t");
                //check if the line has the same amount of columns as the header
                if(entries.size()==splittedHeader.size()){
                    //check if the node is in the graph
                    if(finalNodesToIndex[cellName].contains(entries[indexName])){
                        cellValues[finalNodesToIndex[cellName][entries[indexName]]] = std::stod(entries[indexValue]);
                        nodeNames.push_back(entries[indexName]);
                    } else {
                        discardedNodes.push_back(entries[indexName]);
                    }
                } else {
                    Logger::getInstance().printError("utilities::valuesVectorsFromFolder: header doesn't have the same amount of columns as the data for file " + filename);
                    Logger::getInstance().printError("header: " + lineHeader);
                    Logger::getInstance().printError("line: " + line);
                    Logger::getInstance().printError("header size: " + std::to_string(splittedHeader.size()));
                    Logger::getInstance().printError("line size: " + std::to_string(entries.size()));
                    throw std::invalid_argument("utilities::valuesVectorsFromFolder: header doesn't have the same amount of columns as the data " + filename);
                }
                // if(entries.size()==2){
                //     if(finalNodesToIndex[cellName].contains(entries[0])){
                //         cellValues[finalNodesToIndex[cellName][entries[0]]] = std::stod(entries[1]);
                //         nodeNames.push_back(entries[0]);
                //     } else{
                //         discardedNodes.push_back(entries[0]);
                //     }
                // } else {
                //     throw std::invalid_argument("utilities::valuesVectorsFromFolder: header doesn't have the same amount of columns as the data " + filename);
                // }
            }
            myfile.close();
            Logger::getInstance().printLog("discarding values for the nodes not in the graph for type " + cellName + ", the nodes discarded are:");
            for(auto iter = discardedNodes.cbegin();iter!=discardedNodes.cend();iter++){
                std::cout << "," << *iter;
            }
            std::cout << std::endl;
            ret.push_back(cellValues);

        
        } else {
            throw std::invalid_argument("utilities::valuesVectorsFromFolder: file does not exists " + filename);
        }
    }
    return std::tuple<std::vector<std::string>,std::vector<std::string>,std::vector<std::vector<double>>> (nodeNames,typeNames,ret);
}


std::map<std::string,std::vector<std::string>> nodeNamesFromFolder(std::string folderPath){
    std::map<std::string,std::vector<std::string>> ret;
    auto files = get_all(folderPath,".tsv");
    if(files.size()==0){
        throw std::invalid_argument("utilities::nodeNamesFromFolder: no files found in the folder " + folderPath);
    }
    for(auto iter = files.cbegin();iter!=files.cend();iter++){
        std::vector<std::string> splitted = splitStringIntoVector(*iter, "/"); //split the path
        std::string filename = splitted[splitted.size()-1]; //last element
        std::vector<std::string> splittedFilename = splitStringIntoVector(filename, "."); //split the extension
        std::string type = splittedFilename[0];
        std::vector<std::string> nodeNames;
        if(file_exists(*iter)){
            ifstream myfile (*iter);
            string header, line;
            getline (myfile,header);  // first line is header
            // search for name column
            std::vector<std::string> splittedHeader = splitStringIntoVector(header, "\t");
            int indexName=-1;
            for(uint i = 0; i < splittedHeader.size(); i++){
                if (boost::algorithm::to_lower_copy(splittedHeader[i]).find("name") != std::string::npos) {
                    indexName = i;
                }
            }
            if(indexName < 0){
                throw std::invalid_argument("utilities::nodeNamesFromFolder: invalid file, the header does not contain a name feature");
            }
            while ( getline (myfile,line) )
            {
                std::vector<std::string> entries = splitStringIntoVector(line, "\t");
                if(entries.size()==splittedHeader.size()){
                    nodeNames.push_back(entries[indexName]);
                } else {
                    Logger::getInstance().printError("utilities::nodeNamesFromFolder: header doesn't have the same amount of columns as the data for file " + *iter);
                    Logger::getInstance().printError("header: " + header);
                    Logger::getInstance().printError("line: " + line);
                    Logger::getInstance().printError("header size: " + splittedHeader.size());
                    Logger::getInstance().printError("line size: " + entries.size());
                    throw std::invalid_argument("utilities::nodeNamesFromFolder: header doesn't have the same amount of columns as the data " + *iter);
                }
            }
            myfile.close();
            ret[type] = nodeNames;
        } else {
            throw std::invalid_argument("utilities::nodeNamesFromFolder: file does not exists " + *iter);
        }
    }
    return ret;

}



std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>> interactionFileToEdgesListAndNodesByName(std::string filename){
    string line;
    std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>> ret;
    if(file_exists(filename)){
        ifstream myfile (filename);
        if (myfile.is_open())
        {
            getline (myfile,line);  // first line is header IMPORTANT
            std::vector<std::string> entriesHeader = splitStringIntoVector(line, "\t");
            int indexTypeStart=-1,indexTypeEnd=-1,indexStartNode=-1,indexEndNode=-1,indexWeight=-1;
            //TODO change attributes names to be more general
            for(uint i = 0; i < entriesHeader.size(); i++){
                if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("starttype") != std::string::npos) {
                    indexTypeStart = i;
                }
                else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("endtype") != std::string::npos) {
                    indexTypeEnd = i;
                }else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("startnodename") != std::string::npos) {
                    indexStartNode = i;
                }else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("endnodename") != std::string::npos) {
                    indexEndNode = i;
                } else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("weight") != std::string::npos) {
                    indexWeight = i;
                }
                //startType	startNodeName	endType	endNodeName	weight
            }
            if(indexTypeStart < 0 || indexTypeEnd < 0 || indexStartNode < 0 || indexEndNode < 0 || indexWeight < 0){
                throw std::invalid_argument("utilities::interactionFileToEdgesListAndNodesByName: invalid file, the header does not contain a startType, or an endType, or a startNodeName, or a endNodeName, or a weight feature");
            }
            while ( getline (myfile,line) )
            {
                std::vector<std::string> entries = splitStringIntoVector(line, "\t");
                if(entries.size()==5){
                    std::string startNodeName,endNodeName;
                    startNodeName = entries[indexStartNode];
                    endNodeName = entries[indexEndNode];    

                    std::string startType = entries[indexTypeStart];
                    std::string endType = entries[indexTypeEnd];
                    double weight = std::stod( entries[indexWeight]);
                    std::string virtualInputEndType = "v-in:" + startType;
                    std::string virtualOutputstartType = "v-out:" + endType;
                    std::tuple<std::string,std::string,double> edgestartType(startNodeName, virtualOutputstartType,weight);
                    std::tuple<std::string,std::string,double> edgeEndType(virtualInputEndType, endNodeName,weight);
                    if(ret.contains(startType)){
                        ret[startType].push_back(edgestartType);
                    }else{
                        ret[startType] = std::vector<std::tuple<std::string,std::string,double>>();
                        ret[startType].push_back(edgestartType);
                    }

                    if(ret.contains(endType)){
                        ret[endType].push_back(edgeEndType);
                    }else{
                        ret[endType] = std::vector<std::tuple<std::string,std::string,double>>();
                        ret[endType].push_back(edgeEndType);
                    }
                    
                }
            }
            myfile.close();
        }
    } else {
        throw std::invalid_argument("utilities::interactionFileToEdgesListAndNodesByName: file does not exists " + filename);
    }
    return ret;
}

std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>> interactionFileToEdgesListAndNodesByName(std::string filename,std::vector<std::string> subtypes){
    string line;
    std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>> ret;
    if(file_exists(filename)){
        ifstream myfile (filename);
        if (myfile.is_open())
        {
            getline (myfile,line);  // first line is header IMPORTANT
            std::vector<std::string> entriesHeader = splitStringIntoVector(line, "\t");
            int indexTypeStart=-1,indexTypeEnd=-1,indexStartNode=-1,indexEndNode=-1,indexWeight=-1;
            for(uint i = 0; i < entriesHeader.size(); i++){ //TODO change names in the header to be more general
                if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("starttype") != std::string::npos) {
                    indexTypeStart = i;
                }
                else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("endtype") != std::string::npos) {
                    indexTypeEnd = i;
                }else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("startnodename") != std::string::npos) {
                    indexStartNode = i;
                }else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("endnodename") != std::string::npos) {
                    indexEndNode = i;
                } else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("weight") != std::string::npos) {
                    indexWeight = i;
                }
                //startType	startNodeName	endType	endNodeName	weight
            }
            if(indexTypeStart < 0 || indexTypeEnd < 0 || indexStartNode < 0 || indexEndNode < 0 || indexWeight < 0){
                throw std::invalid_argument("utilities::interactionFileToEdgesListAndNodesByName: invalid file, the header does not contain a startType, or an endType, or a Ligand gene, or a receptor gene, or a weight feature");
            }
            while ( getline (myfile,line) )
            {
                std::vector<std::string> entries = splitStringIntoVector(line, "\t");
                if(entries.size()==5){
                    std::string startNodeName,endNodeName;
                    startNodeName = entries[indexStartNode];
                    endNodeName = entries[indexEndNode];    

                    std::string startType = entries[indexTypeStart];
                    std::string endType = entries[indexTypeEnd];
                    if(vectorContains(subtypes, startType) && vectorContains(subtypes, endType)){
                        double weight = std::stod( entries[indexWeight]);
                        std::string virtualInputEndType = "v-in:" + startType;
                        std::string virtualOutputstartType = "v-out:" + endType;
                        std::tuple<std::string,std::string,double> edgestartType(startNodeName, virtualOutputstartType,weight);
                        std::tuple<std::string,std::string,double> edgeEndType(virtualInputEndType, endNodeName,weight);
                        if(ret.contains(startType)){
                            ret[startType].push_back(edgestartType);
                        }else{
                            ret[startType] = std::vector<std::tuple<std::string,std::string,double>>();
                            ret[startType].push_back(edgestartType);
                        }

                        if(ret.contains(endType)){
                            ret[endType].push_back(edgeEndType);
                        }else{
                            ret[endType] = std::vector<std::tuple<std::string,std::string,double>>();
                            ret[endType].push_back(edgeEndType);
                        }
                    } else {
                        //ignored because not in the subtypes
                    }
                }
            }
            myfile.close();
        }
    } else {
        throw std::invalid_argument("utilities::interactionFileToEdgesListAndNodesByName: file does not exists " + filename);
    }
    return ret;
}

std::pair<std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>>,std::vector<std::tuple<std::string, std::string, std::string, std::string, std::unordered_set<int>, double>>> interactionContactsFileToEdgesListAndNodesByName(std::string filename, std::vector<std::string> subtypes, int maximumIntertypeTime, std::string granularity,std::unordered_map<std::string,std::vector<std::string>> typeToNodeNames, bool undirectedTypeEdges){
    string line;
    std::pair<std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>>,std::vector<std::tuple<std::string, std::string, std::string, std::string, std::unordered_set<int>, double>>> ret;
    // control if the granularity is valid
    if(granularity != "" && granularity != "type" && granularity != "node" && granularity != "typeAndNode"){
        throw std::invalid_argument("utilities::interactionContactsFileToEdgesListAndNodesByName: invalid granularity, it must be typeAndNode(finer) or type(coarser), or only node(no types)");
    }
    if(granularity == ""){
        granularity = "type";
    }
    if(file_exists(filename)){
        ifstream myfile (filename);
        if (myfile.is_open())
        {
            getline (myfile,line);  // first line is header IMPORTANT
            std::vector<std::string> entriesHeader = splitStringIntoVector(line, "\t");
            int indexTypeStart=-1, indexTypeEnd=-1, indexStartNode=-1, indexEndNode=-1, indexWeight=-1, indexContactTimes=-1;
            for(uint i = 0; i < entriesHeader.size(); i++){ //TODO change names in the header to be more general
                if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("starttype") != std::string::npos) {
                    indexTypeStart = i;
                }
                else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("endtype") != std::string::npos) {
                    indexTypeEnd = i;
                }else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("startnodename") != std::string::npos) {
                    indexStartNode = i;
                }else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("endnodename") != std::string::npos) {
                    indexEndNode = i;
                } else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("weight") != std::string::npos) {
                    indexWeight = i;
                } else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("contacttimes") != std::string::npos) {
                    indexContactTimes = i;
                }
                //startType	startNodeName	endType	endNodeName	weight
            }
            bool noContactTimes = false;
            if(indexTypeStart < 0 || indexTypeEnd < 0 || indexStartNode < 0 || indexEndNode < 0 || indexWeight < 0){
                throw std::invalid_argument("utilities::interactionFileToEdgesListAndNodesByName: invalid file, the header does not contain a startType, or an endType, or a start node, or an end node, or a weight feature");
            }
            if(indexContactTimes < 0){
                noContactTimes = true;
            }
            while ( getline (myfile,line) )
            {
                std::vector<std::string> entries = splitStringIntoVector(line, "\t");
                if(entries.size()==5 || entries.size()==6){
                    std::string startNodeName,endNodeName;
                    startNodeName = entries[indexStartNode];
                    endNodeName = entries[indexEndNode];
                    std::string startType = entries[indexTypeStart];
                    std::string endType = entries[indexTypeEnd];
                    if(typeToNodeNames.size() != 0){
                        if(!vectorContains(typeToNodeNames[startType],startNodeName)){
                            Logger::getInstance().printError("start node <" + startNodeName + "> for type: " + startType + " is not in the specified network, aborting ");
                            throw std::invalid_argument("utilities::interactionContactsFileToEdgesListAndNodesByName: invalid file, the start node " + startNodeName + " is not in the type specified, aborting");
                        }

                        if(!vectorContains(typeToNodeNames[endType],endNodeName)){
                            Logger::getInstance().printError("end node <" + endNodeName + "> for type: " + endType + " is not in the specified network, aborting ");
                            throw std::invalid_argument("utilities::interactionContactsFileToEdgesListAndNodesByName: invalid file, the end node " + endNodeName + " is not in the type specified, aborting");
                        }
                    }

                    std::unordered_set<int> contactTimes;
                    double weight = std::stod( entries[indexWeight]);
                    if(noContactTimes){
                        // if no contact times are specified, then every time is a contact time, so all contact times from 0 to maximumIntertypeTime are added
                        for(int i = 0; i < maximumIntertypeTime; i++){
                            contactTimes.insert(i);
                        }
                    } else {
                        std::string contactTimesString = entries[indexContactTimes];
                        std::vector<std::string> splittedContactTimes = splitStringIntoVector(contactTimesString, ",");
                        for(auto iter = splittedContactTimes.cbegin(); iter!=splittedContactTimes.cend(); iter++){
                            int contactTime = std::stoi(*iter);
                            if(contactTime <= maximumIntertypeTime){
                                contactTimes.insert(contactTime);
                            }
                        }
                    }
                    // add the edge to the augmented graph, only if the startType and the endType are in the subtypes
                    if(vectorContains(subtypes, startType) && vectorContains(subtypes, endType)){
                        std::string virtualInputEndType = "";
                        std::string virtualOutputStartType = "";
                        // when undirectedTypeEdges is true, virtual input for the startType and virtual output for the endType are added to the respective graphs 
                        std::string virtualInputStartType = "";
                        std::string virtualOutputEndType = "";
                        if(granularity == "typeAndNode"){
                            virtualInputEndType = "v-in:" + startType + "_" + startNodeName;
                            virtualOutputStartType = "v-out:" + endType + "_" + endNodeName;
                            virtualInputStartType = "v-in:" + endType + "_" + endNodeName;
                            virtualOutputEndType = "v-out:" + startType + "_" + startNodeName;
                        } else if (granularity == "type"){
                            virtualInputEndType = "v-in:" + startType;
                            virtualOutputStartType = "v-out:" + endType;
                            virtualInputStartType = "v-in:" + endType;
                            virtualOutputEndType = "v-out:" + startType;
                        } else {
                            virtualInputEndType = "v-in:" + startNodeName;
                            virtualOutputStartType = "v-out:" + endNodeName;
                            virtualInputStartType = "v-in:" + endNodeName;
                            virtualOutputEndType = "v-out:" + startNodeName;
                        }
                        std::tuple<std::string,std::string,double> edgestartType(startNodeName, virtualOutputStartType,weight);
                        std::tuple<std::string,std::string,double> undirectedEdgestartType(virtualInputStartType, startNodeName,weight);
                        std::tuple<std::string,std::string,double> edgeEndType(virtualInputEndType, endNodeName,weight);
                        std::tuple<std::string,std::string,double> undirectedEdgeEndType(endNodeName, virtualOutputEndType,weight);
                        // add the edge to the startType
                        if(!ret.first.contains(startType)){
                            ret.first[startType] = std::vector<std::tuple<std::string,std::string,double>>();
                        }
                        ret.first[startType].push_back(edgestartType);
                        if(undirectedTypeEdges){
                            ret.first[startType].push_back(undirectedEdgestartType);
                        }

                        // add the edge to the endType
                        if(!ret.first.contains(endType)){
                            ret.first[endType] = std::vector<std::tuple<std::string,std::string,double>>();
                        }
                        ret.first[endType].push_back(edgeEndType);
                        if(undirectedTypeEdges){
                            ret.first[endType].push_back(undirectedEdgeEndType);
                        }
                    } else {
                        //ignored because not in the subtypes
                    }
                    // add the edge with the contact times to the second vector in ret
                    ret.second.push_back(std::tuple<std::string, std::string, std::string, std::string, std::unordered_set<int>, double>(startNodeName, endNodeName, startType, endType, contactTimes, weight));
                    if(undirectedTypeEdges){
                        ret.second.push_back(std::tuple<std::string, std::string, std::string, std::string, std::unordered_set<int>, double>(endNodeName, startNodeName, endType, startType, contactTimes, weight));
                    }
                } else {
                    Logger::getInstance().printError("columns detected: " + std::to_string(entries.size()) + " columns ");
                    throw std::invalid_argument("utilities::interactionFileToEdgesListAndNodesByName: header doesn't have the right amount of columns(5 or 6 when considering interaction times) ");
                }
            }
            myfile.close();
        }
    } else {
        throw std::invalid_argument("utilities::interactionFileToEdgesListAndNodesByName: file does not exists " + filename);
    }
    return ret;
}

// considering a timestep, the function is almost the same as above but generate double contact times, based upon the timestep
std::pair<std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>>,std::vector<std::tuple<std::string, std::string, std::string, std::string, std::set<double>, double>>> interactionContinuousContactsFileToEdgesListAndNodesByName(std::string filename, std::vector<std::string> subtypes, int maximumIntertypeTime, std::string granularity,std::unordered_map<std::string,std::vector<std::string>> typeToNodeNames, bool undirectedTypeEdges, double timestep){
    string line;
    std::pair<std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>>,std::vector<std::tuple<std::string, std::string, std::string, std::string, std::set<double>, double>>> ret;
    // control if the granularity is valid
    if(granularity != "" && granularity != "type" && granularity != "node" && granularity != "typeAndNode"){
        throw std::invalid_argument("utilities::interactionContinuousContactsFileToEdgesListAndNodesByName: invalid granularity, it must be typeAndNode(finer) or type(coarser), or only node(no types)");
    }
    if(granularity == ""){
        granularity = "type";
    }
    if(file_exists(filename)){
        ifstream myfile (filename);
        if (myfile.is_open())
        {
            getline (myfile,line);  // first line is header IMPORTANT
            std::vector<std::string> entriesHeader = splitStringIntoVector(line, "\t");
            int indexTypeStart=-1, indexTypeEnd=-1, indexStartNode=-1, indexEndNode=-1, indexWeight=-1, indexContactTimes=-1;
            for(uint i = 0; i < entriesHeader.size(); i++){ //TODO change names in the header to be more general
                if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("starttype") != std::string::npos) {
                    indexTypeStart = i;
                }
                else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("endtype") != std::string::npos) {
                    indexTypeEnd = i;
                }else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("startnodename") != std::string::npos) {
                    indexStartNode = i;
                }else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("endnodename") != std::string::npos) {
                    indexEndNode = i;
                } else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("weight") != std::string::npos) {
                    indexWeight = i;
                } else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("contacttimes") != std::string::npos) {
                    indexContactTimes = i;
                }
                //startType	startNodeName	endType	endNodeName	weight
            }
            bool noContactTimes = false;
            if(indexTypeStart < 0 || indexTypeEnd < 0 || indexStartNode < 0 || indexEndNode < 0 || indexWeight < 0){
                throw std::invalid_argument("utilities::interactionContinuousContactsFileToEdgesListAndNodesByName: invalid file, the header does not contain a startType, or an endType, or a start node, or an end node, or a weight feature");
            }
            if(indexContactTimes < 0){
                noContactTimes = true;
            }
            while ( getline (myfile,line) )
            {
                std::vector<std::string> entries = splitStringIntoVector(line, "\t");
                if(entries.size()==5 || entries.size()==6){
                    std::string startNodeName,endNodeName;
                    startNodeName = entries[indexStartNode];
                    endNodeName = entries[indexEndNode];    
                    std::string startType = entries[indexTypeStart];
                    std::string endType = entries[indexTypeEnd];
                    if(typeToNodeNames.size() != 0){
                        if(!vectorContains(typeToNodeNames[startType],startNodeName)){
                            Logger::getInstance().printError("start node <" + startNodeName + "> for type: " + startType + " is not in the specified network, aborting ");
                            throw std::invalid_argument("utilities::interactionContinuousContactsFileToEdgesListAndNodesByName: invalid file, the start node " + startNodeName + " is not in the type specified, aborting");
                        }

                        if(!vectorContains(typeToNodeNames[endType],endNodeName)){
                            Logger::getInstance().printError("end node <" + endNodeName + "> for type: " + endType + " is not in the specified network, aborting ");
                            throw std::invalid_argument("utilities::interactionContinuousContactsFileToEdgesListAndNodesByName: invalid file, the end node " + endNodeName + " is not in the type specified, aborting");
                        }
                    }

                    std::set<double> contactTimes;
                    double weight = std::stod( entries[indexWeight]);
                    if(noContactTimes){
                        // if no contact times are specified, then every time is a contact time, so all contact times from 0 to maximumIntertypeTime are added
                        for(int i = 0; i < maximumIntertypeTime; i++){
                            contactTimes.insert(i*timestep);
                        }
                    } else {
                        std::string contactTimesString = entries[indexContactTimes];
                        std::vector<std::string> splittedContactTimes = splitStringIntoVector(contactTimesString, ",");
                        for(auto iter = splittedContactTimes.cbegin(); iter!=splittedContactTimes.cend(); iter++){
                            double contactTime = std::stod(*iter);
                            // TODO change interTypeTime to be a double since it is a time, and I am now using the timestep to define the times and quantize the contacts
                            if(contactTime <= maximumIntertypeTime){
                                contactTimes.insert(contactTime);
                            }
                        }
                    }
                    // add the edge to the augmented graph, only if the startType and the endType are in the subtypes
                    if(vectorContains(subtypes, startType) && vectorContains(subtypes, endType)){
                        std::string virtualInputEndType = "";
                        std::string virtualOutputStartType = "";
                        // when undirectedTypeEdges is true, virtual input for the startType and virtual output for the endType are added to the respective graphs 
                        std::string virtualInputStartType = "";
                        std::string virtualOutputEndType = "";
                        if(granularity == "typeAndNode"){
                            virtualInputEndType = "v-in:" + startType + "_" + startNodeName;
                            virtualOutputStartType = "v-out:" + endType + "_" + endNodeName;
                            virtualInputStartType = "v-in:" + endType + "_" + endNodeName;
                            virtualOutputEndType = "v-out:" + startType + "_" + startNodeName;
                        } else if (granularity == "type"){
                            virtualInputEndType = "v-in:" + startType;
                            virtualOutputStartType = "v-out:" + endType;
                            virtualInputStartType = "v-in:" + endType;
                            virtualOutputEndType = "v-out:" + startType;
                        } else {
                            virtualInputEndType = "v-in:" + startNodeName;
                            virtualOutputStartType = "v-out:" + endNodeName;
                            virtualInputStartType = "v-in:" + endNodeName;
                            virtualOutputEndType = "v-out:" + startNodeName;
                        }
                        std::tuple<std::string,std::string,double> edgestartType(startNodeName, virtualOutputStartType,weight);
                        std::tuple<std::string,std::string,double> undirectedEdgestartType(virtualInputStartType, startNodeName,weight);
                        std::tuple<std::string,std::string,double> edgeEndType(virtualInputEndType, endNodeName,weight);
                        std::tuple<std::string,std::string,double> undirectedEdgeEndType(endNodeName, virtualOutputEndType,weight);
                        // add the edge to the startType
                        if(!ret.first.contains(startType)){
                            ret.first[startType] = std::vector<std::tuple<std::string,std::string,double>>();
                        }
                        ret.first[startType].push_back(edgestartType);
                        if(undirectedTypeEdges){
                            ret.first[startType].push_back(undirectedEdgestartType);
                        }

                        // add the edge to the endType
                        if(!ret.first.contains(endType)){
                            ret.first[endType] = std::vector<std::tuple<std::string,std::string,double>>();
                        }
                        ret.first[endType].push_back(edgeEndType);
                        if(undirectedTypeEdges){
                            ret.first[endType].push_back(undirectedEdgeEndType);
                        }
                    } else {
                        //ignored because not in the subtypes
                    }
                    // add the edge with the contact times to the second vector in ret
                    ret.second.push_back(std::tuple<std::string, std::string, std::string, std::string, std::set<double>, double>(startNodeName, endNodeName, startType, endType, contactTimes, weight));
                    if(undirectedTypeEdges){
                        ret.second.push_back(std::tuple<std::string, std::string, std::string, std::string, std::set<double>, double>(endNodeName, startNodeName, endType, startType, contactTimes, weight));
                    }
                } else {
                    Logger::getInstance().printError("columns detected: " + std::to_string(entries.size()) + " columns ");
                    throw std::invalid_argument("utilities::interactionContinuousContactsFileToEdgesListAndNodesByName: header doesn't have the right amount of columns(5 or 6 when considering interaction times) ");
                }
            }
            myfile.close();
        }
    } else {
        throw std::invalid_argument("utilities::interactionContinuousContactsFileToEdgesListAndNodesByName: file does not exists " + filename);
    }
    return ret;
}

std::pair<std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>>,std::vector<std::tuple<std::string, std::string, std::string, std::string, std::set<double>, double>>> interactionContinuousContactsFileToEdgesListAndNodesByName(std::string filename, std::vector<std::string> subtypes,double maximumIntertypeTime, std::string granularity, std::unordered_map<std::string,std::vector<std::string>> typeToNodeNames , bool undirectedTypeEdges, double timestep){
    string line;
    std::pair<std::map<std::string,std::vector<std::tuple<std::string,std::string,double>>>,std::vector<std::tuple<std::string, std::string, std::string, std::string, std::set<double>, double>>> ret;
    // control if the granularity is valid
    if(granularity != "" && granularity != "type" && granularity != "node" && granularity != "typeAndNode"){
        throw std::invalid_argument("utilities::interactionContinuousContactsFileToEdgesListAndNodesByName: invalid granularity, it must be typeAndNode(finer) or type(coarser), or only node(no types)");
    }
    if(granularity == ""){
        granularity = "type";
    }

    if(file_exists(filename)){
        ifstream myfile (filename);
        if (myfile.is_open())
        {
            getline (myfile,line);  // first line is header IMPORTANT
            std::vector<std::string> entriesHeader = splitStringIntoVector(line, "\t");
            int indexTypeStart=-1, indexTypeEnd=-1, indexStartNode=-1, indexEndNode=-1, indexWeight=-1, indexContactTimes=-1;
            for(uint i = 0; i < entriesHeader.size(); i++){ //TODO change names in the header to be more general
                if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("starttype") != std::string::npos) {
                    indexTypeStart = i;
                }
                else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("endtype") != std::string::npos) {
                    indexTypeEnd = i;
                }else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("startnodename") != std::string::npos) {
                    indexStartNode = i;
                }else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("endnodename") != std::string::npos) {
                    indexEndNode = i;
                } else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("weight") != std::string::npos) {
                    indexWeight = i;
                } else if (boost::algorithm::to_lower_copy(entriesHeader[i]).find("contacttimes") != std::string::npos) {
                    indexContactTimes = i;
                }
                //startType	startNodeName	endType	endNodeName	weight
            }
            bool noContactTimes = false;
            if(indexTypeStart < 0 || indexTypeEnd < 0 || indexStartNode < 0 || indexEndNode < 0 || indexWeight < 0){
                Logger::getInstance().printError("invalid file, the header does not contain one of the following: startType, endType, start node, end node, weight feature");
                Logger::getInstance().printError("values found for startType: " + std::to_string(indexTypeStart) + " endType: " + std::to_string(indexTypeEnd) + " start node: " + std::to_string(indexStartNode) + " end node: " + std::to_string(indexEndNode) + " weight: " + std::to_string(indexWeight));
                throw std::invalid_argument("utilities::interactionContinuousContactsFileToEdgesListAndNodesByName: invalid file, the header does not contain a startType, or an endType, or a start node, or an end node, or a weight feature");
            }
            if(indexContactTimes < 0){
                noContactTimes = true;
            }
            while ( getline (myfile,line) )
            {
                std::vector<std::string> entries = splitStringIntoVector(line, "\t");
                if (entries.size() == entriesHeader.size()) {
                    std::string startNodeName,endNodeName;
                    startNodeName = entries[indexStartNode];
                    endNodeName = entries[indexEndNode];
                    std::string startType = entries[indexTypeStart];
                    std::string endType = entries[indexTypeEnd];
                    if(typeToNodeNames.size() != 0 && vectorContains(subtypes, startType) && vectorContains(subtypes, endType)){
                        if(!vectorContains(typeToNodeNames[startType],startNodeName)){
                            Logger::getInstance().printError("start node <" + startNodeName + "> for type: " + startType + " is not in the specified network, aborting ");
                            throw std::invalid_argument("utilities::interactionContinuousContactsFileToEdgesListAndNodesByName: invalid file " + filename +" , the start node " + startNodeName + " is not in the type specified, aborting");
                        }

                        if(!vectorContains(typeToNodeNames[endType],endNodeName)){
                            Logger::getInstance().printError("end node <" + endNodeName + "> for type: " + endType + " is not in the specified network, aborting ");
                            throw std::invalid_argument("utilities::interactionContinuousContactsFileToEdgesListAndNodesByName: invalid file " + filename +" , the end node " + endNodeName + " is not in the type specified, aborting");
                        }
                    }

                    std::set<double> contactTimes;
                    double weight = std::stod( entries[indexWeight]);
                    if(noContactTimes){
                        // if no contact times are specified, then every time is a contact time, so all contact times from 0 to maximumIntertypeTime are added
                        for(int i = 0; i < maximumIntertypeTime; i++){
                            contactTimes.insert(i*timestep);
                        }
                    } else {
                        std::string contactTimesString = entries[indexContactTimes];
                        std::vector<std::string> splittedContactTimes = splitStringIntoVector(contactTimesString, ",");
                        for(auto iter = splittedContactTimes.cbegin(); iter!=splittedContactTimes.cend(); iter++){
                            double contactTime = std::stod(*iter);
                            // TODO change interTypeTime to be a double since it is a time, and I am now using the timestep to define the times and quantize the contacts
                            if(contactTime <= maximumIntertypeTime){
                                contactTimes.insert(contactTime);
                            } else {
                                Logger::getInstance().printWarning("contact time: " + std::to_string(contactTime) + " is greater than the maximumIntertypeTime: " + std::to_string(maximumIntertypeTime) + " ignoring it");
                            }
                        }
                    }
                    // add the edge to the augmented graph, only if the startType and the endType are in the subtypes
                    if(vectorContains(subtypes, startType) && vectorContains(subtypes, endType)){
                        std::string virtualInputEndType = "";
                        std::string virtualOutputStartType = "";
                        // when undirectedTypeEdges is true, virtual input for the startType and virtual output for the endType are added to the respective graphs 
                        std::string virtualInputStartType = "";
                        std::string virtualOutputEndType = "";
                        if(granularity == "typeAndNode"){
                            virtualInputEndType = "v-in:" + startType + "_" + startNodeName;
                            virtualOutputStartType = "v-out:" + endType + "_" + endNodeName;
                            virtualInputStartType = "v-in:" + endType + "_" + endNodeName;
                            virtualOutputEndType = "v-out:" + startType + "_" + startNodeName;
                        } else if (granularity == "type"){
                            virtualInputEndType = "v-in:" + startType;
                            virtualOutputStartType = "v-out:" + endType;
                            virtualInputStartType = "v-in:" + endType;
                            virtualOutputEndType = "v-out:" + startType;
                        } else {
                            virtualInputEndType = "v-in:" + startNodeName;
                            virtualOutputStartType = "v-out:" + endNodeName;
                            virtualInputStartType = "v-in:" + endNodeName;
                            virtualOutputEndType = "v-out:" + startNodeName;
                        }
                        std::tuple<std::string,std::string,double> edgestartType(startNodeName, virtualOutputStartType,weight);
                        std::tuple<std::string,std::string,double> undirectedEdgestartType(virtualInputStartType, startNodeName,weight);
                        std::tuple<std::string,std::string,double> edgeEndType(virtualInputEndType, endNodeName,weight);
                        std::tuple<std::string,std::string,double> undirectedEdgeEndType(endNodeName, virtualOutputEndType,weight);
                        // add the edge to the startType
                        if(!ret.first.contains(startType)){
                            ret.first[startType] = std::vector<std::tuple<std::string,std::string,double>>();
                        }
                        ret.first[startType].push_back(edgestartType);
                        if(undirectedTypeEdges){
                            ret.first[startType].push_back(undirectedEdgestartType);
                        }

                        // add the edge to the endType
                        if(!ret.first.contains(endType)){
                            ret.first[endType] = std::vector<std::tuple<std::string,std::string,double>>();
                        }
                        ret.first[endType].push_back(edgeEndType);
                        if(undirectedTypeEdges){
                            ret.first[endType].push_back(undirectedEdgeEndType);
                        }
                    } else {
                        //ignored because not in the subtypes
                    }
                    // add the edge with the contact times to the second vector in ret
                    ret.second.push_back(std::tuple<std::string, std::string, std::string, std::string, std::set<double>, double>(startNodeName, endNodeName, startType, endType, contactTimes, weight));
                    if(undirectedTypeEdges){
                        ret.second.push_back(std::tuple<std::string, std::string, std::string, std::string, std::set<double>, double>(endNodeName, startNodeName, endType, startType, contactTimes, weight));
                    }
                } else {
                    Logger::getInstance().printError("entries detected: " + std::to_string(entries.size()) + " != " + std::to_string(entriesHeader.size()) + "for file " + filename);
                    throw std::invalid_argument("utilities::interactionContinuousContactsFileToEdgesListAndNodesByName: entries.size() != entriesHeader.size() " + std::to_string(entries.size()) + " != " + std::to_string(entriesHeader.size()) + " meaning one of the entries of file " + filename + " has not the same amount of features as the header");    
                }
            }
            myfile.close();
        }
    } else {
        throw std::invalid_argument("utilities::interactionContinuousContactsFileToEdgesListAndNodesByName: file does not exists " + filename);
    }

    return ret; 
}

std::function<arma::Col<double>(double)> dissipationScalingFunctionFromFile(std::string filename, std::vector<std::string> orderedNodeNames){
    if(!file_exists(filename)){
        throw std::invalid_argument("utilities::dissipationScalingFunctionFromFile: file does not exists " + filename);
    }
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        throw std::invalid_argument("utilities::dissipationScalingFunctionFromFile: unable to open file " + filename);
    }
    std::function<arma::Col<double>(double)> ret;
    //read the first line to get the header
    std::string line;
    getline(infile, line);
    std::vector<std::string> entriesHeader = splitStringIntoVector(line, "\t");
    // check if the header is valid
    if(entriesHeader.size() < 2 || (boost::algorithm::to_lower_copy(entriesHeader[0]) != "name" || boost::algorithm::to_lower_copy(entriesHeader[1]) != "parameters")){
        throw std::invalid_argument("utilities::dissipationScalingFunctionFromFile: invalid header in file " + filename + ", expected first column to be name, and second column to be parameters");
    }
    // read the rest of the file
    std::vector<std::function<double(double)>> orderedFunctions = std::vector<std::function<double(double)>>(orderedNodeNames.size(), getDissipationScalingFunction());
    while (getline(infile, line)) {
        std::vector<std::string> entries = splitStringIntoVector(line, "\t");
        if(entries.size() != 2){
            throw std::invalid_argument("utilities::dissipationScalingFunctionFromFile: invalid entry in file " + filename + ", expected two columns, got " + std::to_string(entries.size()));
        }
        std::string name = entries[0];
        std::string parameters = entries[1];
        std::vector<std::string> parametersVector = splitStringIntoVector(parameters, ",");
        std::vector<double> parametersDouble;
        for (const auto& param : parametersVector) {
            try {
                parametersDouble.push_back(std::stod(param));
            } catch (const std::invalid_argument& e) {
                throw std::invalid_argument("utilities::dissipationScalingFunctionFromFile: invalid parameter in file " + filename + ", expected a real number, got " + param);
            }
        }
        // find the index of the name in the orderedNodeNames vector
        auto it = std::find(orderedNodeNames.begin(), orderedNodeNames.end(), name);
        if (it != orderedNodeNames.end()) {
            size_t index = std::distance(orderedNodeNames.begin(), it);
            // create the function and store it in the orderedFunctions vector
            // try to create the function, if it fails it means that the parameters are not valid for the function
            try {
                orderedFunctions[index] = getDissipationScalingFunction(parametersDouble);
            } catch (const std::invalid_argument& e) {
                throw std::invalid_argument("utilities::dissipationScalingFunctionFromFile: probably invalid parameters for function " + name + " in file " + filename + ", " + e.what());
            }
        } else { // if the name is not found in the orderedNodeNames vector, ignore it and print a warning
            Logger::getInstance().printWarning("utilities::dissipationScalingFunctionFromFile: name " + name + " not found in orderedNodeNames vector, ignoring it");
            //throw std::invalid_argument("utilities::dissipationScalingFunctionFromFile: name " + name + " not found in orderedNodeNames vector");
        }
    }
    infile.close();
    // create the function that takes a double and returns a column vector with the values of the functions for each node
    ret = [orderedFunctions](double t) -> arma::Col<double> {
        arma::Col<double> result(orderedFunctions.size());
        for (size_t i = 0; i < orderedFunctions.size(); ++i) {
            result(i) = orderedFunctions[i](t);
        }
        return result;
    };
        
    return ret;
}

std::map<std::string, std::function<arma::Col<double>(double)>> dissipationScalingFunctionsFromFolder(std::string folderPath, std::map<std::string, std::vector<std::string>> typeToOrderedNodeNames){
    std::map<std::string, std::function<arma::Col<double>(double)>> ret;
    std::vector<std::string> typesFromMap;
    for(const auto& [type, nodeNames] : typeToOrderedNodeNames){
        typesFromMap.push_back(type);
    }
    std::vector<std::string> files = get_all(folderPath, ".tsv");
    std::vector<std::string> expectedFiles;
    for(const auto& type : typesFromMap){
        expectedFiles.push_back(folderPath + "/" + type + ".tsv");
    }
    if(files.size() == 0){
        Logger::getInstance().printWarning("utilities::dissipationScalingFunctionsFromFolder: no files found in folder " + folderPath + ", using default functions for all types");
        // if no files are found, use the default function for all types
        for(const auto& type : typesFromMap){
            auto orderedNames = typeToOrderedNodeNames[type];
            ret[type] = [orderedNames](double t) -> arma::Col<double> {
                arma::Col<double> result(orderedNames.size());
                #pragma omp parallel for
                for (size_t i = 0; i < orderedNames.size(); ++i) {
                    result(i) = getDissipationScalingFunction()(t); // using the default function with the size of the ordered names
                }
                return result;
            };
                
        }
        return ret;
    }
    // check if the files are present, if a file is not present, use the default function with the correct number of returned length for the scaling function
    for(const auto& type : typesFromMap){
        if(std::find(files.begin(), files.end(), folderPath + "/" + type + ".tsv") == files.end()){
            Logger::getInstance().printWarning("utilities::dissipationScalingFunctionsFromFolder: file " + folderPath + "/" + type + ".tsv not found, using default function for type " + type);
            // if the file is not found, use the default function with the correct number of returned length for the scaling function
            auto orderedNames = typeToOrderedNodeNames[type];
            ret[type] = [orderedNames](double t) -> arma::Col<double> {
                arma::Col<double> result(orderedNames.size());
                #pragma omp parallel for
                for (size_t i = 0; i < orderedNames.size(); ++i) {
                    result(i) = getDissipationScalingFunction()(t); // using the default function with the size of the ordered names
                }
                return result;
            };
        } else {
            ret[type] = dissipationScalingFunctionFromFile(folderPath + "/" + type + ".tsv", typeToOrderedNodeNames[type]);
        }
    }
    return ret;
}

std::function<arma::Col<double>(double)> conservationScalingFunctionFromFile(std::string filename, std::vector<std::string> orderedNodeNames){
    if(!file_exists(filename)){
        throw std::invalid_argument("utilities::conservationScalingFunctionFromFile: file does not exists " + filename);
    }
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        throw std::invalid_argument("utilities::conservationScalingFunctionFromFile: unable to open file " + filename);
    }
    std::function<arma::Col<double>(double)> ret;
    //read the first line to get the header
    std::string line;
    getline(infile, line);
    std::vector<std::string> entriesHeader = splitStringIntoVector(line, "\t");
    // check if the header is valid
    if(entriesHeader.size() < 2 || (boost::algorithm::to_lower_copy(entriesHeader[0]) != "name" || boost::algorithm::to_lower_copy(entriesHeader[1]) != "parameters")){
        throw std::invalid_argument("utilities::conservationScalingFunctionFromFile: invalid header in file " + filename + ", expected first column to be name, and second column to be parameters");
    }
    // read the rest of the file
    std::vector<std::function<double(double)>> orderedFunctions = std::vector<std::function<double(double)>>(orderedNodeNames.size(), getConservationScalingFunction());
    while (getline(infile, line)) {
        std::vector<std::string> entries = splitStringIntoVector(line, "\t");
        if(entries.size() != 2){
            throw std::invalid_argument("utilities::conservationScalingFunctionFromFile: invalid entry in file " + filename + ", expected two columns, got " + std::to_string(entries.size()));
        }
        std::string name = entries[0];
        std::string parameters = entries[1];
        std::vector<std::string> parametersVector = splitStringIntoVector(parameters, ",");
        std::vector<double> parametersDouble;
        for (const auto& param : parametersVector) {
            try {
                parametersDouble.push_back(std::stod(param));
            } catch (const std::invalid_argument& e) {
                throw std::invalid_argument("utilities::conservationScalingFunctionFromFile: invalid parameter in file " + filename + ", expected a real number, got " + param);
            }
        }
        // find the index of the name in the orderedNodeNames vector
        auto it = std::find(orderedNodeNames.begin(), orderedNodeNames.end(), name);
        if (it != orderedNodeNames.end()) {
            size_t index = std::distance(orderedNodeNames.begin(), it);
            // create the function and store it in the orderedFunctions vector
            // try to create the function, if it fails it means that the parameters are not valid for the function
            try {
                orderedFunctions[index] = getConservationScalingFunction(parametersDouble);
            } catch (const std::invalid_argument& e) {
                throw std::invalid_argument("utilities::conservationScalingFunctionFromFile: probably invalid parameters for function " + name + " in file " + filename + ", " + e.what());
            }
        } else { // if the name is not found in the orderedNodeNames vector, ignore it and print a warning
            Logger::getInstance().printWarning("utilities::conservationScalingFunctionFromFile: name " + name + " not found in orderedNodeNames vector, ignoring it");
            //throw std::invalid_argument("utilities::conservationScalingFunctionFromFile: name " + name + " not found in orderedNodeNames vector");
        }
    }
    infile.close();
    // create the function that takes a double and returns a column vector with the values of the functions for each node
    ret = [orderedFunctions](double t) -> arma::Col<double> {
        arma::Col<double> result(orderedFunctions.size());
        for (size_t i = 0; i < orderedFunctions.size(); ++i) {
            result(i) = orderedFunctions[i](t);
        }
        return result;
    };
        
    return ret;
}

std::map<std::string, std::function<arma::Col<double>(double)>> conservationScalingFunctionsFromFolder(std::string folderPath, std::map<std::string, std::vector<std::string>> typeToOrderedNodeNames){
    std::map<std::string, std::function<arma::Col<double>(double)>> ret;
    std::vector<std::string> typesFromMap;
    for(const auto& [type, nodeNames] : typeToOrderedNodeNames){
        typesFromMap.push_back(type);
    }
    std::vector<std::string> files = get_all(folderPath, ".tsv");
    std::vector<std::string> expectedFiles;
    for(const auto& type : typesFromMap){
        expectedFiles.push_back(folderPath + "/" + type + ".tsv");
    }
    if(files.size() == 0){
        Logger::getInstance().printWarning("utilities::conservationScalingFunctionsFromFolder: no files found in folder " + folderPath + ", using default functions for all types");
        // if no files are found, use the default function for all types
        for(const auto& type : typesFromMap){
            auto orderedNames = typeToOrderedNodeNames[type];
            ret[type] = [orderedNames](double t) -> arma::Col<double> {
                arma::Col<double> result(orderedNames.size());
                #pragma omp parallel for
                for (size_t i = 0; i < orderedNames.size(); ++i) {
                    result(i) = getConservationScalingFunction()(t); // using the default function with the size of the ordered names
                }
                return result;
            };
                
        }
        return ret;
    }
    // check if the files are present, if a file is not present, use the default function with the correct number of returned length for the scaling function
    for(const auto& type : typesFromMap){
        if(std::find(files.begin(), files.end(), folderPath + "/" + type + ".tsv") == files.end()){
            Logger::getInstance().printWarning("utilities::conservationScalingFunctionsFromFolder: file " + folderPath + "/" + type + ".tsv not found, using default function for type " + type);
            // if the file is not found, use the default function with the correct number of returned length for the scaling function
            auto orderedNames = typeToOrderedNodeNames[type];
            ret[type] = [orderedNames](double t) -> arma::Col<double> {
                arma::Col<double> result(orderedNames.size());
                #pragma omp parallel for
                for (size_t i = 0; i < orderedNames.size(); ++i) {
                    result(i) = getConservationScalingFunction()(t); // using the default function with the size of the ordered names
                }
                return result;
            };
        } else {
            ret[type] = conservationScalingFunctionFromFile(folderPath + "/" + type + ".tsv", typeToOrderedNodeNames[type]);
        }
    }
    return ret;
}

std::function<arma::Col<double>(double)> propagationScalingFunctionFromFile(std::string filename, std::vector<std::string> orderedNodeNames){
    if(!file_exists(filename)){
        throw std::invalid_argument("utilities::propagationScalingFunctionFromFile: file does not exists " + filename);
    }
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        throw std::invalid_argument("utilities::propagationScalingFunctionFromFile: unable to open file " + filename);
    }
    std::function<arma::Col<double>(double)> ret;
    //read the first line to get the header
    std::string line;
    getline(infile, line);
    std::vector<std::string> entriesHeader = splitStringIntoVector(line, "\t");
    // check if the header is valid
    if(entriesHeader.size() < 2 || (boost::algorithm::to_lower_copy(entriesHeader[0]) != "name" || boost::algorithm::to_lower_copy(entriesHeader[1]) != "parameters")){
        throw std::invalid_argument("utilities::propagationScalingFunctionFromFile: invalid header in file " + filename + ", expected first column to be name, and second column to be parameters");
    }
    // read the rest of the file
    std::vector<std::function<double(double)>> orderedFunctions = std::vector<std::function<double(double)>>(orderedNodeNames.size(), getPropagationScalingFunction());
    while (getline(infile, line)) {
        std::vector<std::string> entries = splitStringIntoVector(line, "\t");
        if(entries.size() != 2){
            throw std::invalid_argument("utilities::propagationScalingFunctionFromFile: invalid entry in file " + filename + ", expected two columns, got " + std::to_string(entries.size()));
        }
        std::string name = entries[0];
        std::string parameters = entries[1];
        std::vector<std::string> parametersVector = splitStringIntoVector(parameters, ",");
        std::vector<double> parametersDouble;
        for (const auto& param : parametersVector) {
            try {
                parametersDouble.push_back(std::stod(param));
            } catch (const std::invalid_argument& e) {
                throw std::invalid_argument("utilities::propagationScalingFunctionFromFile: invalid parameter in file " + filename + ", expected a real number, got " + param);
            }
        }
        // find the index of the name in the orderedNodeNames vector
        auto it = std::find(orderedNodeNames.begin(), orderedNodeNames.end(), name);
        if (it != orderedNodeNames.end()) {
            size_t index = std::distance(orderedNodeNames.begin(), it);
            // create the function and store it in the orderedFunctions vector
            // try to create the function, if it fails it means that the parameters are not valid for the function
            try {
                orderedFunctions[index] = getPropagationScalingFunction(parametersDouble);
            } catch (const std::invalid_argument& e) {
                throw std::invalid_argument("utilities::propagationScalingFunctionFromFile: probably invalid parameters for function " + name + " in file " + filename + ", " + e.what());
            }
        } else { // if the name is not found in the orderedNodeNames vector, ignore it and print a warning
            Logger::getInstance().printWarning("utilities::propagationScalingFunctionFromFile: name " + name + " not found in orderedNodeNames vector, ignoring it");
            //throw std::invalid_argument("utilities::conservationScalingFunctionFromFile: name " + name + " not found in orderedNodeNames vector");
        }
    }
    infile.close();
    // create the function that takes a double and returns a column vector with the values of the functions for each node
    ret = [orderedFunctions](double t) -> arma::Col<double> {
        arma::Col<double> result(orderedFunctions.size());
        for (size_t i = 0; i < orderedFunctions.size(); ++i) {
            result(i) = orderedFunctions[i](t);
        }
        return result;
    };
        
    return ret;
}


std::map<std::string, std::vector<std::string>> getFullNodesDescription(std::string filename){
    string line;
    // schema is #Id	Name	Type	Aliases
    std::map<std::string,std::vector<std::string>> ret;
    if(file_exists(filename)){
        ifstream myfile (filename);
        if (myfile.is_open())
        {
            getline (myfile,line);  // first line is header IMPORTANT
            while ( getline (myfile,line) )
            {
                std::vector<std::string> entries = splitStringIntoVector(line, "\t");
                if(entries.size()==4){
                    std::string Id = entries[0];
                    std::string Name = entries[1];
                    std::string Type = entries[2];
                    std::string Aliases = entries[3];
                    ret[Id] = std::vector<std::string>{Id,Name,Type,Aliases};
                    
                }
            }
            myfile.close();
        }
    } else {
        throw std::invalid_argument("utilities::getFullNodesDescription: file does not exists " + filename);
    }
    return ret;
}


std::vector<std::string> get_all(std::string const & root, std::string const & ext)
{
    std::vector<std::string> paths;
    for (auto &p : std::filesystem::recursive_directory_iterator(root))
    {
        if (p.path().extension() == ext)
            paths.push_back(root + "/" + p.path().stem().string() + ext);
    }
    return paths;
} 

void saveNodeValues(std::string folderName, int iteration, std::string cellName, std::vector<double> nodeValues,std::vector<std::string> nodeNames, std::string nodesDescriptionFile){
    std::string outputFilename = folderName + "/" + cellName + "--"+std::to_string(iteration) + ".tsv";
    std::ofstream outfile(outputFilename,ios::out|ios::trunc);

    if (!outfile.is_open()) {
        Logger::getInstance().printError("Unable to open file " + outputFilename );
        return;
    }
    if(nodesDescriptionFile.length()==0){
        Logger::getInstance().printWarning("no nodes description file provided, using nothing to get the nodes description");
    } else {
        if(!file_exists(nodesDescriptionFile)){
            throw std::invalid_argument("utilities::saveNodeValues: file does not exists " + nodesDescriptionFile);
        }
    }
    //header
    outfile << "nodeID\tnodeName\ttype\talias\tnodeValue\n";
    //body
    if(nodesDescriptionFile.length()!=0){
        auto mapToEverything = getFullNodesDescription(nodesDescriptionFile);
        for(uint i = 0; i < nodeValues.size(); i++){
            if(mapToEverything.contains(nodeNames[i]))
                outfile<<mapToEverything.at(nodeNames[i])[0]<<"\t"<<mapToEverything.at(nodeNames[i])[1]<<"\t"<<mapToEverything.at(nodeNames[i])[2]<<"\t"<<mapToEverything.at(nodeNames[i])[3]<<"\t"<< std::to_string(nodeValues[i]);
            else {
                auto splittedVirtual = splitStringIntoVector(nodeNames[i], ":");
                if(splittedVirtual[0]=="v-in"){
                    outfile<<nodeNames[i]<<"\t"<<nodeNames[i]<<"\t"<<"virtual-input\t"<<splittedVirtual[1]<<"\t"<<std::to_string(nodeValues[i]);
                } else if(splittedVirtual[0]=="v-out"){
                    outfile<<nodeNames[i]<<"\t"<<nodeNames[i]<<"\t"<<"virtual-output\t"<<splittedVirtual[1]<<"\t"<<std::to_string(nodeValues[i]);
                } else{ //when the node names are not genes but something else
                    outfile<<nodeNames[i]<<"\t"<<nodeNames[i]<<"\t"<<"nodes in the graph\t"<<nodeNames[i]<<"\t"<<std::to_string(nodeValues[i]);
                }
            }
            outfile << std::endl;
        }
    }  
    else{
        for(uint i = 0; i < nodeValues.size(); i++){
            auto splittedVirtual = splitStringIntoVector(nodeNames[i], ":");
            if(splittedVirtual[0]=="v-in"){
                outfile<<nodeNames[i]<<"\t"<<nodeNames[i]<<"\t"<<"virtual-input\t"<<splittedVirtual[1]<<"\t"<<std::to_string(nodeValues[i]);
            } else if(splittedVirtual[0]=="v-out"){
                outfile<<nodeNames[i]<<"\t"<<nodeNames[i]<<"\t"<<"virtual-output\t"<<splittedVirtual[1]<<"\t"<<std::to_string(nodeValues[i]);
            } else{ //when the node names are not genes but something else
                outfile<<nodeNames[i]<<"\t"<<nodeNames[i]<<"\t"<<"nodes in the graph\t"<<nodeNames[i]<<"\t"<<std::to_string(nodeValues[i]);
            }
            outfile << std::endl;
        }
    }

    // for (const auto& row : data) {
    //     for (const auto& item : row) {
    //         outfile << item << ",";
    //     }
    //     outfile << "\n";
    // }

    outfile.close();
}

void saveNodeValues(std::string folderName, int iterationOuter, int intraIteration, std::string cellName, std::vector<double> nodeValues,std::vector<std::string> nodeNames, std::string nodesDescriptionFile){
    std::string outputFilename = folderName + "/" + cellName + "--"+std::to_string(iterationOuter + intraIteration) + ".tsv";
    std::ofstream outfile(outputFilename,ios::out|ios::trunc);

    if (!outfile.is_open()) {
        Logger::getInstance().printError("Unable to open file " + outputFilename);
        throw std::invalid_argument("utilities::saveNodeValues: unable to open output file " + outputFilename);
    }

    if(nodesDescriptionFile.length()!=0 && !file_exists(nodesDescriptionFile)){
        throw std::invalid_argument("utilities::saveNodeValues: file does not exists " + nodesDescriptionFile);
    }

    std::map<std::string, std::vector<std::string>> mapToEverything;
    if(nodesDescriptionFile.length()!=0){
        mapToEverything = getFullNodesDescription(nodesDescriptionFile);
    } else {
        mapToEverything = std::map<std::string, std::vector<std::string>>();
    }

    //header
    outfile << "nodeID\tnodeName\ttype\talias\tnodeValue\n";
    //body
    for(uint i = 0; i < nodeValues.size(); i++){
        if(mapToEverything.size() !=0 && mapToEverything.contains(nodeNames[i]))
            outfile<<mapToEverything.at(nodeNames[i])[0]<<"\t"<<mapToEverything.at(nodeNames[i])[1]<<"\t"<<mapToEverything.at(nodeNames[i])[2]<<"\t"<<mapToEverything.at(nodeNames[i])[3]<<"\t"<< std::to_string(nodeValues[i]);
        else {
            auto splittedVirtual = splitStringIntoVector(nodeNames[i], ":");
            if(splittedVirtual[0]=="v-in"){
                outfile<<nodeNames[i]<<"\t"<<nodeNames[i]<<"\t"<<"virtual-input\t"<<splittedVirtual[1]<<"\t"<<std::to_string(nodeValues[i]);
            } else if(splittedVirtual[0]=="v-out"){
                outfile<<nodeNames[i]<<"\t"<<nodeNames[i]<<"\t"<<"virtual-output\t"<<splittedVirtual[1]<<"\t"<<std::to_string(nodeValues[i]);
            } else{ //when the node names are not genes but something else
                outfile<<nodeNames[i]<<"\t"<<nodeNames[i]<<"\t"<<"nodes in the graph\t"<<nodeNames[i]<<"\t"<<std::to_string(nodeValues[i]);
            }
        }
        outfile << std::endl;
    }
}

void saveNodeValuesWithTime(std::string folderName,int iterationOuter, int intraIteration, std::string cellName, std::vector<double> nodeValues,std::vector<std::string> nodeNames, std::string nodesDescriptionFile, double timestep){
    std::string outputFilename = folderName + "/" + cellName + "--"+std::to_string(iterationOuter + intraIteration) + ".tsv";
    std::ofstream outfile(outputFilename,ios::out|ios::trunc);

    if (!outfile.is_open()) {
        Logger::getInstance().printError("Unable to open file " + outputFilename);
        throw std::invalid_argument("utilities::saveNodeValues: unable to open output file " + outputFilename);
    }

    if(nodesDescriptionFile.length()!=0 && !file_exists(nodesDescriptionFile)){
        throw std::invalid_argument("utilities::saveNodeValues: file does not exists " + nodesDescriptionFile);
    }

    std::map<std::string, std::vector<std::string>> mapToEverything;
    if(nodesDescriptionFile.length()!=0){
        mapToEverything = getFullNodesDescription(nodesDescriptionFile);
    } else {
        mapToEverything = std::map<std::string, std::vector<std::string>>();
    }

    //header
    outfile << "nodeID\tnodeName\ttype\talias\tnodeValue\ttime\n";
    //body
    for(uint i = 0; i < nodeValues.size(); i++){
        if(mapToEverything.size() !=0 && mapToEverything.contains(nodeNames[i]))
            outfile<<mapToEverything.at(nodeNames[i])[0]<<"\t"<<mapToEverything.at(nodeNames[i])[1]<<"\t"<<mapToEverything.at(nodeNames[i])[2]<<"\t"<<mapToEverything.at(nodeNames[i])[3]<<"\t"<< std::to_string(nodeValues[i]) << "\t" << std::to_string((iterationOuter + intraIteration) *timestep);
        else {
            auto splittedVirtual = splitStringIntoVector(nodeNames[i], ":");
            if(splittedVirtual[0]=="v-in"){
                outfile<<nodeNames[i]<<"\t"<<nodeNames[i]<<"\t"<<"virtual-input\t"<<splittedVirtual[1]<<"\t"<<std::to_string(nodeValues[i])<< "\t" << std::to_string((iterationOuter + intraIteration) *timestep);
            } else if(splittedVirtual[0]=="v-out"){
                outfile<<nodeNames[i]<<"\t"<<nodeNames[i]<<"\t"<<"virtual-output\t"<<splittedVirtual[1]<<"\t"<<std::to_string(nodeValues[i])<< "\t" << std::to_string((iterationOuter + intraIteration) *timestep);
            } else{ //when the node names are not genes but something else
                outfile<<nodeNames[i]<<"\t"<<nodeNames[i]<<"\t"<<"nodes in the graph\t"<<nodeNames[i]<<"\t"<<std::to_string(nodeValues[i])<< "\t" << std::to_string((iterationOuter + intraIteration) *timestep);
            }
        }
        outfile << std::endl;
    }
}


void saveNodeValuesWithTimeSimple(std::string folderName,int currentIteration, double currentTime, std::string typeName, std::vector<double> nodeValues,std::vector<std::string> nodeNames, std::string nodesDescriptionFile){
    std::string outputFilename = folderName + "/" + typeName + "--"+std::to_string(currentIteration) + ".tsv";
    std::ofstream outfile(outputFilename,ios::out|ios::trunc);

    if (!outfile.is_open()) {
        Logger::getInstance().printError("Unable to open file " + outputFilename);
        throw std::invalid_argument("utilities::saveNodeValues: unable to open output file " + outputFilename);
    }

    if(nodesDescriptionFile.length()!=0 && !file_exists(nodesDescriptionFile)){
        throw std::invalid_argument("utilities::saveNodeValues: file does not exists " + nodesDescriptionFile);
    }

    std::map<std::string, std::vector<std::string>> mapToEverything;
    if(nodesDescriptionFile.length()!=0){
        mapToEverything = getFullNodesDescription(nodesDescriptionFile);
    } else {
        mapToEverything = std::map<std::string, std::vector<std::string>>();
    }

    //header
    outfile << "nodeID\tnodeName\ttype\talias\tnodeValue\ttime\n";
    //body
    for(uint i = 0; i < nodeValues.size(); i++){
        if(mapToEverything.size() !=0 && mapToEverything.contains(nodeNames[i]))
            outfile<<mapToEverything.at(nodeNames[i])[0]<<"\t"<<mapToEverything.at(nodeNames[i])[1]<<"\t"<<mapToEverything.at(nodeNames[i])[2]<<"\t"<<mapToEverything.at(nodeNames[i])[3]<<"\t"<< std::to_string(nodeValues[i]) << "\t" << std::to_string(currentTime);
        else {
            auto splittedVirtual = splitStringIntoVector(nodeNames[i], ":");
            if(splittedVirtual[0]=="v-in"){
                outfile<<nodeNames[i]<<"\t"<<nodeNames[i]<<"\t"<<"virtual-input\t"<<splittedVirtual[1]<<"\t"<<std::to_string(nodeValues[i])<< "\t" << std::to_string(currentTime);
            } else if(splittedVirtual[0]=="v-out"){
                outfile<<nodeNames[i]<<"\t"<<nodeNames[i]<<"\t"<<"virtual-output\t"<<splittedVirtual[1]<<"\t"<<std::to_string(nodeValues[i])<< "\t" << std::to_string(currentTime);
            } else{ //when the node names are not genes but something else
                outfile<<nodeNames[i]<<"\t"<<nodeNames[i]<<"\t"<<"nodes in the graph\t"<<nodeNames[i]<<"\t"<<std::to_string(nodeValues[i])<< "\t" << std::to_string(currentTime);
            }
        }
        outfile << std::endl;
    }
}

void saveOutputMatrix(std::string outputFolderNameMatrices, Matrix<double>* outputMatrix, std::vector<std::string> outputMatricesRowNames, int intertypeIterations, int intratypeIterations, double timestep, std::string typeName){
    std::string outputFilename = outputFolderNameMatrices + "/" + typeName + ".tsv";
    // write the header with the times
    std::ofstream outfile(outputFilename,ios::out|ios::trunc);
    if (!outfile.is_open()) {
        Logger::getInstance().printError("Unable to open file " + outputFilename);
        throw std::invalid_argument("utilities::saveOutputMatrix: unable to open output file " + outputFilename);
    }
    // first attribute is nodeNames, representing the node names
    outfile << "nodeNames\t";
    for(int i = 0; i < intertypeIterations; i++){
        for(int j = 0; j < intratypeIterations; j++){
            int currentIteration = i*intratypeIterations + j;
            outfile << currentIteration*(timestep/intratypeIterations) << "\t";
        }
    }
    outfile << std::endl;
    // write the body of the matrix
    for(int i = 0; i < outputMatrix->getRows(); i++){
        outfile << outputMatricesRowNames[i];
        for(int j = 0; j < outputMatrix->getCols(); j++){
            outfile << "\t" << outputMatrix->getValue(i,j);
        }
        outfile << std::endl;
    }
    outfile.close();
}


