/**
 * @file WeightedEdgeGraph.cxx
 * @ingroup Core
 * @brief Implements the WeightedEdgeGraph class used for managing weighted edge graphs.
 * @details The WeightedEdgeGraph class provides methods for creating and manipulating weighted edge graphs.
 */ 
#include "data_structures/WeightedEdgeGraph.hxx"
#include "data_structures/Matrix.hxx"
#include "utils/mathUtilities.hxx"
#include <algorithm>
#include <iterator>
#include <map>
#include <numeric>
#include <ostream>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <unordered_set>
#include <iostream>
#include <vector>
#include <utility>


WeightedEdgeGraph::WeightedEdgeGraph(){
    this->numberOfNodes = 0;
    this->nodeValues = nullptr;
    this->adjList = std::vector<std::unordered_set<int>>();
    this->adjMatrix = Matrix<double>(0,0);
    this->nodeToIndex = std::map<std::string, int>();
    
}

WeightedEdgeGraph::WeightedEdgeGraph(int numNodes){
    this->numberOfNodes = numNodes;
    this->nodeValues = new double[numNodes];
    this->adjList = std::vector<std::unordered_set<int>>(numNodes);
    //edgesVector = new std::vector<std::pair<int, int>>;

    for (int i = 0; i < numNodes; i++) {
        nodeValues[i]=0;
    }
    this->adjMatrix = Matrix<double>(numNodes,numNodes);

    this->nodeToIndex = std::map<std::string, int>();
    // ADDING NODE NAMES AS INTEGERS CAST TO STRING?

    for(int i = 0; i < numNodes; i++){
        nodeToIndex[std::to_string(i)] = i;
        nameVector.push_back(std::to_string(i));
    }
    
}


WeightedEdgeGraph::WeightedEdgeGraph(const Matrix<double>& _adjMatrix){
    if (_adjMatrix.getCols()==_adjMatrix.getRows()) {
        int numNodes = _adjMatrix.getCols();
        this->numberOfNodes = numNodes;
        this->nodeValues = new double[numNodes];
        for (int i = 0; i < numNodes; i++) {
            nodeValues[i]=0;
        }
        this->adjList = std::vector<std::unordered_set<int>>(numNodes);
        this->adjMatrix = Matrix<double>(numNodes,numNodes);
        for (int i = 0 ; i<numNodes; i++) {
            for (int j = 0; j<numNodes; j++) {
                if(!approximatelyEqual(_adjMatrix.getValue(i, j),0.0,0.0000000001)) 
                    addEdge(i, j, _adjMatrix.getValue(i, j));
            }
        }

        // ADDING NODE NAMES AS INTEGERS CAST TO STRING?

        for(int i = 0; i < numNodes; i++){
            nodeToIndex[std::to_string(i)] = i;
            nameVector.push_back(std::to_string(i));
        }
    } else {
        Logger::getInstance().printError("WeightedEdgeGraph::WeightedEdgeGraph(constructor): invalid argument for graph constructor, the matrix is not square");
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::WeightedEdgeGraph(constructor): invalid argument for graph constructor, the matrix is not square");
    }
    
}

WeightedEdgeGraph::WeightedEdgeGraph(std::vector<std::string>& nodeNames){
    int numNodes = SizeToInt(nodeNames.size());
    this->numberOfNodes = numNodes;
    this->nodeValues = new double[numNodes];
    this->adjList = std::vector<std::unordered_set<int>>(numNodes);

    this->adjMatrix = Matrix<double>(numNodes,numNodes);

    this->nodeToIndex = std::map<std::string, int>();
    for (int i = 0; i < numNodes; i++) {
        nodeValues[i]=0;
        nodeToIndex[nodeNames[i]] = i;
        nameVector.push_back(nodeNames[i]);
    }
    
}

WeightedEdgeGraph::WeightedEdgeGraph(std::vector<std::string>& nodeNames,std::vector<double>& nodeVal){
    if(nodeNames.size()==nodeVal.size()){
        int numNodes = SizeToInt(nodeNames.size());
        this->numberOfNodes = numNodes;
        this->nodeValues = new double[numNodes];
        this->adjList = std::vector<std::unordered_set<int>>(numNodes);

        this->adjMatrix = Matrix<double>(numNodes,numNodes);

        this->nodeToIndex = std::map<std::string, int>();
        for (int i = 0; i < numNodes; i++) {
            nodeValues[i]=nodeVal[i];
            nodeToIndex[nodeNames[i]] = i;
            nameVector.push_back(nodeNames[i]);
        }
    }
    else throw std::invalid_argument("[ERROR] WeightedEdgeGraph::WeightedEdgeGraph(constructor): invalid argument for graph constructor, nodeNames and nodeValues have not the same length");
    
}

WeightedEdgeGraph::~WeightedEdgeGraph(){
    if(nodeValues) {
        delete [] this->nodeValues;
    }
    
}

int WeightedEdgeGraph::outDegreeOfNode(int node)const{
    if(node >= numberOfNodes || node < 0){
        Logger::getInstance().printError("WeightedEdgeGraph::outDegreeOfNode: node is not in the graph ");
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::outDegreeOfNode: invalid argument for outdegree of node");
    }
    return adjList[node].size();
}

//TODO use adjacency matrix to get the outdegree, since it's more efficient
int WeightedEdgeGraph::inDegreeOfNode(int node)const{
    if(node >= numberOfNodes || node < 0){
        Logger::getInstance().printError("WeightedEdgeGraph::inDegreeOfNode: node " + std::to_string(node) + " is not in the graph ");
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::inDegreeOfNode: invalid argument for indegree of node");
    }
    int inDegree = 0;
    for(int i = 0; i < numberOfNodes; i++){
        if(adjList[i].contains(node)){
            inDegree++;
        }
    }
    return inDegree;
}

int WeightedEdgeGraph::degreeOfNode(int node)const{
    if(node >= numberOfNodes || node < 0){
        Logger::getInstance().printError("WeightedEdgeGraph::degreeOfNode: node " + std::to_string(node) + " is not in the graph ");
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::degreeOfNode: invalid argument for degree of node");
    }
    return getNeighbors(node).size();
}

WeightedEdgeGraph* WeightedEdgeGraph::addEdge(int node1, int node2, double weight, bool directed){
    if(node1 >= numberOfNodes || node2 >= numberOfNodes){
        Logger::getInstance().printError("add edge failed for edges " + std::to_string(node1) + " and " + std::to_string(node2));
        if(node1 >= numberOfNodes){
            Logger::getInstance().printError("node1(number " + std::to_string(node1) + ") is not in the graph that has " + std::to_string(numberOfNodes) + " nodes");
        }
        else{
            Logger::getInstance().printError("node2(number " + std::to_string(node2) + ") is not in the graph that has " + std::to_string(numberOfNodes) + " nodes");
        }
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::addEdge: failed to add an edge, see error logs");
    } else if (connectedNodes(node1, node2)) {
        //edge already added
    } else {
        numberOfEdges++;
        edgesVector.push_back(std::tuple<int, int, double>(node1,node2, weight));
        adjList[node1].insert(node2);
        adjMatrix(node1,node2) = weight;
        if(!directed){
            adjList[node2].insert(node1);
            adjMatrix(node2,node1) = weight;
        }

    }

    return this;
}


WeightedEdgeGraph* WeightedEdgeGraph::addEdge(std::string node1name, std::string node2name, double weight, bool directed){
    if(!(nodeToIndex.contains(node1name) && nodeToIndex.contains(node2name)) ){
        Logger::getInstance().printError("add edge failed for edges " + node1name + " and " + node2name);
        if(!nodeToIndex.contains(node1name)){
            Logger::getInstance().printError("WeightedEdgeGraph::addEdge: node1 " + node1name + " is not in the graph ");
        }
        else{
            Logger::getInstance().printError("WeightedEdgeGraph::addEdge: node2 " + node2name + " is not in the graph ");
        }
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::addEdge: invalid argument when adding an edge");
    } else if (connectedNodes(node1name, node2name)) {
        adjMatrix(nodeToIndex[node1name],nodeToIndex[node2name]) = weight;
    } else {
        int node1 = nodeToIndex[node1name];
        int node2 = nodeToIndex[node2name];
        numberOfEdges++;
        edgesVector.push_back(std::tuple<int, int, double>(node1,node2, weight));
        adjList[node1].insert(node2);
        adjMatrix(node1,node2) = weight;
        if(!directed){
            adjList[node2].insert(node1);
            adjMatrix(node2,node1) = weight;
        }
    }

    return this;
}

WeightedEdgeGraph* WeightedEdgeGraph::addNode(double value){
    this->numberOfNodes++;
    adjMatrix = adjMatrix.copyAndAddRowsColsWithZeros(1, 1);
    double* tmp = nodeValues;
    nodeValues = new double[this->numberOfNodes];
    std::copy(tmp,tmp+(this->numberOfNodes-1),nodeValues);
    delete [] tmp;
    nodeValues[this->numberOfNodes-1]=value;

    adjList.push_back(std::unordered_set<int>());
    nameVector.push_back(std::to_string(this->numberOfNodes-1));
    nodeToIndex[std::to_string(this->numberOfNodes-1)] = this->numberOfNodes-1;
    return this;
}
WeightedEdgeGraph* WeightedEdgeGraph::addNode(std::string name, double value){
    if(nodeToIndex.contains(name)){
        //throw exceptions or handle it differently, like incrementing a counter or changing-adding the last characters to the string
        // for now it just throws an exception
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::addNode: node name already present");
    }else{
        this->numberOfNodes++;
        adjMatrix = adjMatrix.copyAndAddRowsColsWithZeros(1, 1);
        double* tmp = nodeValues;
        nodeValues = new double[this->numberOfNodes];
        std::copy(tmp,tmp+(this->numberOfNodes-1),nodeValues);
        delete [] tmp;
        nodeValues[this->numberOfNodes-1]=value;

        adjList.push_back(std::unordered_set<int>());
        nameVector.push_back(name);
        nodeToIndex[name] = this->numberOfNodes-1;
    }
    return this;
}


WeightedEdgeGraph* WeightedEdgeGraph::addNodes(const std::vector<double>& values){
    int oldNumberOfNodes = this->numberOfNodes; 
    this->numberOfNodes += values.size();
    adjMatrix = adjMatrix.copyAndAddRowsColsWithZeros(values.size(), values.size());
    double* tmp = nodeValues;
    nodeValues = new double[this->numberOfNodes];
    std::copy(tmp,tmp+(oldNumberOfNodes),nodeValues);
    delete [] tmp;
    
    int i = 0;
    for(auto it = values.cbegin();it != values.cend();it++,i++){
        adjList.push_back(std::unordered_set<int>());
        nameVector.push_back(std::to_string(oldNumberOfNodes+i));
        nodeToIndex[std::to_string(oldNumberOfNodes+i)] = oldNumberOfNodes + i;
        nodeValues[oldNumberOfNodes+i]=*it;
    }
    return this;
}

WeightedEdgeGraph* WeightedEdgeGraph::addNodes(const std::vector<std::string>& names, const std::vector<double>& values){
    //how to handle some names already present in the graph?
    auto controlMapContainsValue = [this](std::string name){return this->nodeToIndex.contains(name);};
    std::vector<bool> tmpVec = std::vector<bool>(names.size(),false);  // initialization is necessary when working with transformation
    std::transform(names.cbegin(),names.cend(),tmpVec.begin(),controlMapContainsValue);
    if(std::reduce(tmpVec.cbegin(),tmpVec.cend(),false,[](bool num1, bool num2){return num1 || num2;})){
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::addNodes: some names in the new nodes are already present in the graph, aborting operation of augmentation of the graph");
    }
    else if (values.size()==0) {
        //default values
        int oldNumberOfNodes = this->numberOfNodes; 
        this->numberOfNodes += names.size();
        adjMatrix = adjMatrix.copyAndAddRowsColsWithZeros(names.size(), names.size());
        double* tmp = nodeValues;
        nodeValues = new double[this->numberOfNodes];
        std::copy(tmp,tmp+(oldNumberOfNodes),nodeValues);
        delete [] tmp;
        
        int i = 0;
        for(auto it = names.cbegin();it != names.cend();it++,i++){
            adjList.push_back(std::unordered_set<int>());
            nameVector.push_back(*it);
            nodeToIndex[*it] = oldNumberOfNodes + i;
            nodeValues[oldNumberOfNodes+i]=0;
        }

    }
    else if( names.size() != values.size()){
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::addNodes: values are not the same size as names when adding nodes");
    }
    else {
        int oldNumberOfNodes = this->numberOfNodes; 
        this->numberOfNodes += names.size();
        adjMatrix = adjMatrix.copyAndAddRowsColsWithZeros(names.size(), names.size());
        double* tmp = nodeValues;
        nodeValues = new double[this->numberOfNodes];
        std::copy(tmp,tmp+(oldNumberOfNodes),nodeValues);
        delete [] tmp;
        
        //std::copy(nodeValues + oldNumberOfNodes,nodeValues+this->numberOfNodes,values.cbegin());
        int i = 0;
        for(auto it = names.cbegin();it != names.cend();it++,i++){
            adjList.push_back(std::unordered_set<int>());
            nameVector.push_back(*it);
            nodeToIndex[*it] = oldNumberOfNodes + i;
            nodeValues[oldNumberOfNodes+i]=values[i];
        }
    }
    return this;
}

WeightedEdgeGraph* WeightedEdgeGraph::addNodesAndCopyNew(const std::vector<double>& values){
    WeightedEdgeGraph* retPointer = this->copyNew();
    return retPointer->addNodes(values);
}

WeightedEdgeGraph* WeightedEdgeGraph::addNodesAndCopyNew(const std::vector<std::string>& names, const std::vector<double>& values){
    WeightedEdgeGraph* retPointer = this->copyNew();
    return retPointer->addNodes(names,values);
}

WeightedEdgeGraph* WeightedEdgeGraph::setNodeValue(int node, double value){
    if (node < numberOfNodes && node >= 0) {
        nodeValues[node] = value;
    } else{
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::setNodeValue: node index not in the graph");
    }
    return this;
}
WeightedEdgeGraph* WeightedEdgeGraph::setNodeValue(std::string node, double value){
    if ( nodeToIndex.contains(node)) {
        nodeValues[nodeToIndex[node]] = value;
    } else{
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::setNodeValue: node name not in the graph");
    }
    return this;
}

WeightedEdgeGraph* WeightedEdgeGraph::setNodeName(std::string nodenameTarget, std::string nodenameSet){
    int index = getIndex(nameVector, nodenameTarget);
    if(index > 0){
        nameVector[index] = nodenameSet;
        auto node = nodeToIndex.extract(nodenameTarget);
        node.key() = nodenameSet;
        nodeToIndex.insert(std::move(node));
    }
    else{
        Logger::getInstance().printError("WeightedEdgeGraph::setNodeName: node name not found: " + nodenameTarget);
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::setNodeName: node name not found");
    } 
    return this;
}

WeightedEdgeGraph* WeightedEdgeGraph::setNodesNames(const std::vector<std::string>& nodenameSets, const std::vector<std::string>& nodenameTargets){
    if(nodenameSets.size() == nodenameTargets.size()){
        if (nodenameTargets.size() > 0) {
            for (int i = 0; i < SizeToInt(nodenameSets.size());i++) {
                setNodeName(nodenameTargets[i], nodenameSets[i]);
            }
        }
        else {
            
        }
    } else if( (nodenameTargets.size() == 0)){
        if ((nodenameSets.size()==nodeToIndex.size() && nameVector.size() == nodenameSets.size())) {
            nodeToIndex = std::map<std::string, int>(); //getting rid of the old mapping
            nameVector = nodenameSets;
            for (int i = 0; i < SizeToInt( nodenameSets.size()); i++) {
                nodeToIndex[nodenameSets[i]] = i;
            }
        } else {
            throw std::invalid_argument("[ERROR] WeightedEdgeGraph::setNodesNames: nodes to set are not all : " + std::to_string(nodenameSets.size()) + " =/=" + std::to_string(nodeToIndex.size()));    
        }
    } else {
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::setNodesNames: nodes to set and nodes to change are not of the same size : " + std::to_string(nodenameSets.size()) + " =/=" + std::to_string(nodenameTargets.size()) );
    }
    return this;
}



double WeightedEdgeGraph::getNodeValue(int node)const{
    if(node >= 0 && node < numberOfNodes)
        return nodeValues[node];
    else throw std::invalid_argument("[ERROR] WeightedEdgeGraph::getNodeValue: node value cannot be retrieved: node not in the list (as index)"); 
}
double WeightedEdgeGraph::getNodeValue(std::string node)const{
    if(nodeToIndex.contains(node))
        return nodeValues[nodeToIndex.at(node)];
    else throw std::invalid_argument("[ERROR] WeightedEdgeGraph::getNodeValue: node value cannot be retrieved: node not in the list (as name)");
}
std::vector<double> WeightedEdgeGraph::getNodeValues(const std::vector<int>& nodes)const{
    std::vector<double> ret;
    for (auto it = nodes.cbegin(); it != nodes.cend(); it++) {
        ret.push_back(getNodeValue(*it));
    }
    return ret;
}
std::vector<double> WeightedEdgeGraph::getNodeValues(const std::vector<std::string>& nodes)const{
    
    std::vector<double> ret;
    if (nodes.size()==0) {
        //default means all node values return as vector of double
        for (int i = 0; i < numberOfNodes; i++) {
            ret.push_back(getNodeValue(i));
        }
    } else{
        for (auto it = nodes.cbegin(); it != nodes.cend(); it++) {
            ret.push_back(getNodeValue(*it));
        }
    }
    return ret;
}



//functions to remove since they can be problematic

//TODO complete
Matrix<double> WeightedEdgeGraph::makeMatrix(){
    //emptying old memory
    // new array
    return this->adjMatrix;
}




// accessory functions

int WeightedEdgeGraph::getNumNodes()const {
    return numberOfNodes;
}

int WeightedEdgeGraph::getNumEdges()const {
    return numberOfEdges;
}


std::string WeightedEdgeGraph::getnodeValuesStr()const{
    std::string stringa = "";
    for (int i = 0; i<numberOfNodes; i++) {
        stringa += std::to_string(nodeValues[i]) + std::string(" ");
    }
    return stringa;
}


std::unordered_set<int> WeightedEdgeGraph::getAdjList(int node)const{
    if(node>=numberOfNodes){
        Logger::getInstance().printError("getAdjList: trying to get an adjacent list of an unknown node: " + std::to_string(node) + ">=" + std::to_string(numberOfNodes));
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::getAdjList: adjacent list of an unknown node");
    } else if(node < 0){
        Logger::getInstance().printError("getAdjList: trying to get an adjacent list of negative index node: " + std::to_string(node));
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::getAdjList: adjacent list of an negative index node");    
    }
    return adjList[node];
}

std::unordered_set<int> WeightedEdgeGraph::getAdjList(std::string node)const{
    return getAdjList(nodeToIndex.at(node));
}

std::string WeightedEdgeGraph::getAdjListStr(int node)const{
    std::string stringa;
    std::unordered_set<int> adjListNode = getAdjList(node);
    for(auto it = adjListNode.cbegin(); it != adjListNode.cend();it++){
                stringa += std::to_string(*it) + " ";
            }
    return stringa;
}

bool WeightedEdgeGraph::containsNode(std::string node)const{
    return nodeToIndex.contains(node);
}

std::vector<int> WeightedEdgeGraph::getPredecessors(int node)const{
    if(node >= numberOfNodes || node < 0){
        Logger::getInstance().printError("WeightedEdgeGraph::getPredecessors: node " + std::to_string(node) + " is not in the graph ");
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::getPredecessors: invalid argument for predecessors of node");
    }
    std::vector<int> predecessors;
    for(int i = 0; i < numberOfNodes; i++){
        if(adjList[i].contains(node)){
            predecessors.push_back(i);
        }
    }
    return predecessors;
}

std::vector<int> WeightedEdgeGraph::getSuccessors(int node)const{
    if(node >= numberOfNodes || node < 0){
        Logger::getInstance().printError("WeightedEdgeGraph::getSuccessors: node " + std::to_string(node) + " is not in the graph ");
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::getSuccessors: invalid argument for successors of node");
    }
    std::vector<int> successors;
    for(auto it = adjList[node].cbegin(); it != adjList[node].cend(); it++){
        successors.push_back(*it);
    }
    return successors;
}

std::vector<int> WeightedEdgeGraph::getNeighbors(int node)const{
    if(node >= numberOfNodes || node < 0){
        Logger::getInstance().printError("WeightedEdgeGraph::getNeighbors: node " + std::to_string(node) + " is not in the graph ");
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::getNeighbors: invalid argument for neighbors of node");
    }
    std::vector<int> neighbors;
    std::vector<int> successors = getSuccessors(node);
    std::vector<int> predecessors = getPredecessors(node);
    neighbors = vectorsUnion(successors, predecessors);

    return neighbors;
}

std::vector<std::string> WeightedEdgeGraph::getPredecessors(std::string node)const{
    if(!nodeToIndex.contains(node)){
        Logger::getInstance().printError("WeightedEdgeGraph::getPredecessors: node " + node + " is not in the graph ");
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::getPredecessors: invalid argument for predecessors of node");
    }
    int nodeIndex = nodeToIndex.at(node);
    std::vector<int> predecessors = getPredecessors(nodeIndex);
    std::vector<std::string> ret;
    for(auto it = predecessors.cbegin(); it != predecessors.cend(); it++){
        ret.push_back(nameVector[*it]);
    }
    return ret;
}

std::vector<std::string> WeightedEdgeGraph::getSuccessors(std::string node)const{
    if(!nodeToIndex.contains(node)){
        Logger::getInstance().printError("WeightedEdgeGraph::getSuccessors: node " + node + " is not in the graph ");
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::getSuccessors: invalid argument for successors of node");
    }
    int nodeIndex = nodeToIndex.at(node);
    std::vector<int> successors = getSuccessors(nodeIndex);
    std::vector<std::string> ret;
    for(auto it = successors.cbegin(); it != successors.cend(); it++){
        ret.push_back(nameVector[*it]);
    }
    return ret;
}

std::vector<std::string> WeightedEdgeGraph::getNeighbors(std::string node)const{
    if(!nodeToIndex.contains(node)){
        Logger::getInstance().printError("WeightedEdgeGraph::getNeighbors: node " + node + " is not in the graph ");
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::getNeighbors: invalid argument for neighbors of node");
    }
    int nodeIndex = nodeToIndex.at(node);
    std::vector<int> neighbors = getNeighbors(nodeIndex);
    std::vector<std::string> ret;
    for(auto it = neighbors.cbegin(); it != neighbors.cend(); it++){
        ret.push_back(nameVector[*it]);
    }
    return ret;
}


std::string WeightedEdgeGraph::getAdjListStr(std::string node)const{
    return getAdjListStr(nodeToIndex.at(node));
}

std::vector<std::tuple<int, int,double>> WeightedEdgeGraph::getEdgesVector()const{
    return edgesVector;
}


bool WeightedEdgeGraph::adjNodes(int node1, int node2){
    std::unordered_set<int> adjListNode1 = getAdjList(node1);
    std::unordered_set<int> adjListNode2 = getAdjList(node2);
    return ( (adjListNode1.find(node2) != adjListNode1.end()) || (adjListNode2.find(node1) != adjListNode2.end())) ;
}

bool WeightedEdgeGraph::adjNodes(std::string node1, std::string node2){
    int node1Index = nodeToIndex.at(node1);
    int node2Index = nodeToIndex.at(node2);
    return adjNodes(node1Index,node2Index);
}


bool WeightedEdgeGraph::connectedNodes(int node1, int node2){
    std::unordered_set<int> adjListNode = getAdjList(node1);
    return ( adjListNode.find(node2) != adjListNode.end());
}

bool WeightedEdgeGraph::connectedNodes(std::string node1, std::string node2){
    if(!nodeToIndex.contains(node1) || !nodeToIndex.contains(node2)){
        Logger::getInstance().printError("WeightedEdgeGraph::connectedNodes: node " + node1 + " or " + node2 + " is not in the graph ");
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::connectedNodes: invalid argument for connected nodes");
    }
    int node1Index = nodeToIndex.at(node1);
    int node2Index = nodeToIndex.at(node2);
    return ( connectedNodes(node1Index,node2Index)); ;
}

int WeightedEdgeGraph::getMaxDegree()const{
    int maxDegree = 0;
    for (int i = 0; i < numberOfNodes; i++) {
        int degree = this->degreeOfNode(i);
        if(degree > maxDegree){
            maxDegree = degree;
        }
    }
    return maxDegree;
}

double WeightedEdgeGraph::getAverageDegree()const{
    double sum = 0;
    for (int i = 0; i < numberOfNodes; i++) {
        sum += this->degreeOfNode(i);
    }
    return sum/numberOfNodes;
}

//non copy and swap to not reallocate some of the resources (doesn't get called with g1 = g2 but its called when invocking *g1=*g2 on pointers)
WeightedEdgeGraph& WeightedEdgeGraph::operator=(const WeightedEdgeGraph& g2){
    if (this!=&g2) {
        this->numberOfNodes = g2.numberOfNodes;
        //deleting old data
        if(nodeValues)delete[] nodeValues;

        //creating new data
        this->nodeValues = new double[g2.numberOfNodes];
        this->adjList = std::vector<std::unordered_set<int>>(g2.numberOfNodes);
        this->edgesVector.clear();
        this->adjMatrix = Matrix<double>(g2.numberOfNodes,g2.numberOfNodes);

        for(auto it = g2.edgesVector.cbegin(); it!=g2.edgesVector.cend();it++){
            int node1 = std::get<0>(*it);
            int node2 = std::get<1>(*it);
            double weight = std::get<2>(*it);
            this->addEdge(node1,node2,weight);
        }
        this->nodeToIndex = g2.getNodeToIndexMap();   //maybe the error is here
        for (int i = 0; i < this->numberOfNodes; i++) {
            nodeValues[i]=g2.getNodeValue(i);
        }
    }
    return *this;
}


void WeightedEdgeGraph::assign(const WeightedEdgeGraph& g2){
    if (this!=&g2) {
        this->numberOfNodes = g2.numberOfNodes;
        //deleting old data
        if(nodeValues)delete[] nodeValues;

        //creating new data
        this->nodeValues = new double[g2.numberOfNodes];
        this->adjList = std::vector<std::unordered_set<int>>(g2.numberOfNodes);
        this->edgesVector.clear();
        this->adjMatrix = Matrix<double>(g2.numberOfNodes,g2.numberOfNodes);

        for(auto it = g2.edgesVector.cbegin(); it!=g2.edgesVector.cend();it++){
            int node1 = std::get<0>(*it);
            int node2 = std::get<1>(*it);
            double weight = std::get<2>(*it);
            this->addEdge(node1,node2,weight);
        }
        this->nodeToIndex = g2.getNodeToIndexMap();   //maybe the error is here
        for (int i = 0; i < this->numberOfNodes; i++) {
            nodeValues[i]=g2.getNodeValue(i);
        }
    }
    return;
}

WeightedEdgeGraph* WeightedEdgeGraph::copyNew()const{
    WeightedEdgeGraph* g2 = new WeightedEdgeGraph(adjMatrix);
    //creating new data
    g2->setNodesNames(nameVector);
    
    for (int i = 0; i < this->numberOfNodes; i++) {
        g2->setNodeValue(i,getNodeValue(i));
    }

    for(auto it = edgesVector.cbegin(); it!=edgesVector.cend();it++){
        int node1 = std::get<0>(*it);
        int node2 = std::get<1>(*it);
        double weight = std::get<2>(*it);
        g2->addEdge(node1,node2,weight);
    }
    return g2;
}

// copy and swap
// WeightedEdgeGraph& WeightedEdgeGraph::operator=(const WeightedEdgeGraph g2){
//     if (this!=&g2) {
//         this->numberOfNodes = g2.numberOfNodes;
//         //deleting old data
//         if(nodeValues)delete[] nodeValues;

//         //creating new data
//         this->nodeValues = new double[g2.numberOfNodes];
//         this->adjList = std::vector<std::unordered_set<int>>(g2.numberOfNodes);
//         this->edgesVector.clear();
//         this->adjMatrix = Matrix<double>(g2.numberOfNodes,g2.numberOfNodes);

//         for(auto it = g2.edgesVector.cbegin(); it!=g2.edgesVector.cend();it++){
//             int node1 = std::get<0>(*it);
//             int node2 = std::get<1>(*it);
//             double weight = std::get<2>(*it);
//             this->addEdge(node1,node2,weight);
//         }
//         this->nodeToIndex = g2.getNodeToIndexMap();   //maybe the error is here
//         for (int i = 0; i < this->numberOfNodes; i++) {
//             nodeValues[i]=g2.getNodeValue(i);
//         }
//     }
//     return *this;
// }

void WeightedEdgeGraph::print()const{
    std::cout << *this;
}

std::ostream& operator<< (std::ostream &out, const WeightedEdgeGraph& data) {
            out << "number of nodes: "<<data.getNumNodes() << "  and of edges:" << data.getNumEdges() <<std::endl;
            std::string nodeValues = data.getnodeValuesStr();
            out <<"node values: "<< nodeValues << std::endl;
            out << "Adj Lists" << std::endl;
            std::map<std::string,int> nodeNamesAndIndexes = data.getNodeToIndexMap();
            for(auto it=nodeNamesAndIndexes.cbegin();it!= nodeNamesAndIndexes.cend();it++){
                out << "node " << it->second << "("<< it->first <<") :" << data.getAdjListStr(it->second) << std::endl;
            }
            out << "Edges vector: {";
            std::vector<std::tuple<int,int,double>> tmpVec= data.getEdgesVector();
            for(auto it = tmpVec.cbegin();it!=tmpVec.cend();it++){
                out << "(" << get<0>(*it)<< ","<< get<1>(*it)<< ","<< get<2>(*it)<< "," << ")," ;
            }
            out << "}"<< std::endl;
            out << "Adj matrix: (";
            for(int i = 0;i<data.getNumNodes();i++){
                for(int j = 0;j<data.getNumNodes();j++){
                    out << data.adjMatrix.getValue(i,j) << ", ";
                }
                out <<std::endl ;
            }
            out << "}"<< std::endl;
            return out;
        }


// WeightedEdgeGraph& WeightedEdgeGraph::setAdjMatrix(Matrix<double>& mat){
//     if (mat.getCols()==mat.getRows()) {
//         this->numberOfNodes = mat.getCols();
//         //deleting old data
//         delete[] nodeValues;
//         delete[] adjList;

//         //creating new data
//         this->nodeValues = new double[numberOfNodes];
//         this->adjList = new std::unordered_set<int>[this->numberOfNodes];
//         this->edgesVector.clear();
//         for (int i = 0 ; i<this->numberOfNodes; i++) {
//             for (int j = 0; j<this->numberOfNodes; j++) {
//                 addEdge(i, j, mat.getValue(i, j));
//             }
//         }
//         return *this;
//     } else throw std::invalid_argument("adjMatrix is not square(does not represent a graph)");
// }


void WeightedEdgeGraph::saveEdgesToFile(std::string outputFolder, std::string filename)const{
    std::ofstream myfile;
    //create the file, if it exists it will be overwritten
    myfile.open(outputFolder + "/" + filename);
    // write the header
    myfile << "source" << "\t" << "target" << "\t" << "weight" << std::endl;
    if(myfile.is_open()){
        for(auto it = edgesVector.cbegin(); it!=edgesVector.cend();it++){
            int node1_id = std::get<0>(*it);
            int node2_id = std::get<1>(*it);
            std::string node1 = nameVector[node1_id];
            std::string node2 = nameVector[node2_id];
            double weight = std::get<2>(*it);
            myfile << node1 << "\t" << node2 << "\t" << weight << std::endl;
        }
        myfile.close();
    } else {
        Logger::getInstance().printError("WeightedEdgeGraph::saveEdgesToFile: file not opened");
        throw std::invalid_argument("[ERROR] WeightedEdgeGraph::saveEdgesToFile: file not opened");
    }
}