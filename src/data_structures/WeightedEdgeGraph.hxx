/**
 * @file WeightedEdgeGraph.hxx
 * @ingroup Core
 * @brief Class for representing a weighted edge graph.
 * @details This class provides methods for creating, modifying, and accessing properties of a weighted edge graph.
 */
#pragma once

#include "data_structures/Matrix.hxx"
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <tuple>
/*std::get<i>( tpl) to get the value or set it in a tuple*/
#include <unordered_set>
#include <iostream>
#include <vector>
#include <utility>

class WeightedEdgeGraph{
    private:
        int numberOfNodes=0; ///< number of nodes in the graph
        int numberOfEdges=0; ///< number of edges in the graph
        double* nodeValues=nullptr;  ///< arrays of nodeValues
        std::vector<std::unordered_set<int>> adjList;  ///< adjList as vector of unordered sets
        std::vector<std::string> nameVector; ///< vector of node names
        std::map<std::string, int> nodeToIndex; ///< map of node names to indexes

    public:

        Matrix<double> adjMatrix; ///< adjacency matrix, public for easy access
        std::vector<std::tuple<int, int, double> > edgesVector; ///< vector of edges as tuples (node1ID, node2ID, weight)

        // constructors and destructors
        /**
         * @brief Default constructor for creating an empty weighted edge graph.
         * @details Initializes the graph with zero nodes and edges, and allocates memory for the adjacency matrix(with 0x0 elements).
         */
        WeightedEdgeGraph();

        /**
         * @brief Constructor for creating a weighted edge graph with a specified number of nodes.
         * @param numNodes The number of nodes in the graph.
         * @details Initializes the graph with the specified number of nodes and zero edges, and allocates memory for the adjacency matrix(with nxn elements).
         */
        WeightedEdgeGraph(int numNodes);

        /**
         * @brief Constructor for creating a weighted edge graph with specified node names.
         * @param nodeNames A vector of strings representing the names of the nodes.
         * @details Initializes the graph with the specified node names and zero edges, and allocates memory for the adjacency matrix(with nxn elements).
         */
        WeightedEdgeGraph(std::vector<std::string>& nodeNames);
        /**
         * @brief Constructor for creating a weighted edge graph with specified node names and values.
         * @param nodeNames A vector of strings representing the names of the nodes.
         * @param nodeVal A vector of doubles representing the values of the nodes.
         * @details Initializes the graph with the specified node names and values, and zero edges, and allocates memory for the adjacency matrix(with nxn elements).
         */
        WeightedEdgeGraph(std::vector<std::string>& nodeNames,std::vector<double>& nodeVal);
        /**
         * @brief Constructor for creating a weighted edge graph from an adjacency matrix.
         * @param _adjMatrix The adjacency matrix representing the graph.
         * @details Initializes the graph with the specified adjacency matrix. Edges are added based on the non-zero values in the matrix.
         * @warning The adjacency matrix must be square (same number of rows and columns).
         * @throw std::invalid_argument if the adjacency matrix is not square.
         */
        WeightedEdgeGraph(const Matrix<double>& _adjMatrix);

        /**
         * @brief Destructor for cleaning up the allocated memory.
         * @details Deallocates the memory used for the node values. Memory handling for adjMatrix is done in the Matrix class.
         */
        ~WeightedEdgeGraph();

        /**
         * @brief Function to add an edge to the graph.
         * @param node1 The index of the first node.
         * @param node2 The index of the second node.
         * @param weight The weight of the edge.
         * @param directed Whether the edge is directed (default is true).
         * @details Adds an edge between the specified nodes with the given weight. If the edge already exists, it updates the weight.
         * @return A pointer to the updated graph.
         * @warning This function doesn't throw exceptions for invalid node indexes. It just prints an error message in cerror and continues 
         */
        WeightedEdgeGraph* addEdge(int node1, int node2, double weight, bool directed=true);
        /**
         * @brief Function to add an edge to the graph using node names.
         * @param node1name The name of the first node.
         * @param node2name The name of the second node.
         * @param weight The weight of the edge.
         * @param directed Whether the edge is directed (default is true).
         * @details Adds an edge between the specified nodes with the given weight. If the edge already exists, it updates the weight.
         * @return A pointer to the updated graph.
         * @warning This function doesn't throw exceptions for invalid node names. It just prints an error message in cerror and continues 
         */
        WeightedEdgeGraph* addEdge(std::string node1name, std::string node2name, double weight, bool directed=true);

        /**
         * @brief Function to get the weight of an edge between two nodes.
         * @param node1 The index of the first node.
         * @param node2 The index of the second node.
         * @return The weight of the edge between the specified nodes.
         * @throw std::out_of_range if one of the nodes is out of range (index). 
         */
        double getEdgeWeight(int node1, int node2)const{
            if(node1 >= 0 && node2 >= 0)
                return adjMatrix.getValue(node1,node2);
            else throw std::out_of_range("WeightedEdgeGraph::getEdgeWeight: one of the nodes is out of range(index)");
        }
        /**
         * @brief Function to get the weight of an edge between two nodes using their names.
         * @param node1 The name of the first node.
         * @param node2 The name of the second node.
         * @return The weight of the edge between the specified nodes.
         * @throw std::out_of_range if one of the nodes is out of range (meaning that no node has that name). 
         */
        double getEdgeWeight(std::string node1, std::string node2)const{
            if(getIndexFromName(node1) >= 0 && getIndexFromName(node2) >= 0)
                return adjMatrix.getValue(nodeToIndex.at(node1),nodeToIndex.at(node2));
            else throw std::out_of_range("WeightedEdgeGraph::getEdgeWeight: one of the nodes is out of range(string)");
        }

        
        /**
         * @brief Function to get the index of a node by its name.
         * @param name The name of the node.
         * @return The index of the node. Or -1 if the node is not found.
         * @warning This function doesn't throw exceptions for invalid node names. It just returns -1 if the node is not found.
         * @details This function is used to get the index of a node by its name. It uses the nodeToIndex map to find the index.
         */
        int getIndexFromName(std::string name)const {
            try {
                return nodeToIndex.at(name);
            }
            catch (const std::out_of_range&) {
                return -1;
                // TODO: Deal with the missing element.
            }
        }

        /**
         * @brief Function to add a node to the graph.
         * @param value The value of the node (default is 0).
         * @return A pointer to the updated graph.
         * @details Adds a node with the specified value to the graph. The node is added at the end of the adjacency matrix and the adjacency list. The name of the node will be set to the index (so same name and index)
         */
        WeightedEdgeGraph* addNode(double value=0);
        /**
         * @brief Function to add a node to the graph with a specified name.
         * @param name The name of the node.
         * @param value The value of the node (default is 0).
         * @return A pointer to the updated graph.
         * @details Adds a node with the specified name and value to the graph. The node is added at the end of the adjacency matrix and the adjacency list.
         * @throw std::invalid_argument if the node name already exists in the graph.
         */
        WeightedEdgeGraph* addNode(std::string name, double value=0);

        /**
         * @brief Function to add multiple nodes to the graph.
         * @param values A vector of doubles representing the values of the nodes.
         * @return A pointer to the updated graph.
         * @details Adds multiple nodes with the specified values to the graph. The nodes are added at the end of the adjacency matrix and the adjacency list.
         * @details The names of the nodes will be set to the index (so same name and index)
         */
        WeightedEdgeGraph* addNodes(const std::vector<double>& values);
        /**
         * @brief Function to add multiple nodes to the graph with specified names and values.
         * @param names A vector of strings representing the names of the nodes.
         * @param values A vector of doubles representing the values of the nodes (default is an empty vector).
         * @return A pointer to the updated graph.
         * @details Adds multiple nodes with the specified names and values to the graph. The nodes are added at the end of the adjacency matrix and the adjacency list.
         * @throw std::invalid_argument if the some of the node names already exist in the graph.
        */
        WeightedEdgeGraph* addNodes(const std::vector<std::string>& names, const std::vector<double>& values=std::vector<double>());

        /**
         * @brief Function to add multiple nodes to the graph and copy the graph into a new graph in dynamic memory.
         * @param values A vector of doubles representing the values of the nodes.
         * @return A pointer to the new graph.
         * @details add n nodes, where n is the size of the vector passed as input, and assign to each node its corrispondent value
         * @details Copy the graph into a new graph in dynamic memory
         * @details Internally uses the addNodes function to add the nodes to the new graph.
        */
        WeightedEdgeGraph* addNodesAndCopyNew(const std::vector<double>& values);
        
        /**        
         * @brief Function to add multiple nodes to the graph and copy the graph into a new graph in dynamic memory.
         * @param names A vector of strings representing the names of the nodes.
         * @param values A vector of doubles representing the values of the nodes (default is an empty vector).
         * @return A pointer to the new graph.
         * @details add n nodes(with n names), where n is the size of the vector passed as input, and assign to each node its corrispondent value
         * @details Copy the graph into a new graph in dynamic memory.
         * @details Internally uses the addNodes function to add the nodes to the new graph
         * @throw std::invalid_argument if the some of the node names already exist in the graph.
        */
        WeightedEdgeGraph* addNodesAndCopyNew(const std::vector<std::string>& names, const std::vector<double>& values=std::vector<double>());


        // setters
        /**
         * @brief Function to set the value of a node by its index.
         * @param node The index of the node.
         * @param value The value to set.
         * @return A pointer to the updated graph.
         * @details Sets the value of the specified node to the given value.
         * @throw std::invalid_argument if the node index is out of range.
        */
        WeightedEdgeGraph* setNodeValue(int node, double value);
        
        /**
         * @brief Function to set the value of a node by its name.
         * @param node The name of the node.
         * @param value The value to set.
         * @return A pointer to the updated graph.
         * @details Sets the value of the specified node to the given value.
         * @throw std::invalid_argument if the node name is not found in the graph.
        */
        WeightedEdgeGraph* setNodeValue(std::string node, double value);

        /**
         * @brief Function to set the names of the nodes in the graph.
         * @param nodenameTarget The name of the node to change.
         * @param nodenameSet The new name for the node.
         * @return A pointer to the updated graph.
         * @details Sets the name of the specified node to the given new name.
         * @throw std::invalid_argument if the node name is not found in the graph.
        */
        WeightedEdgeGraph* setNodeName(std::string nodenameTarget, std::string nodenameSet);
        /**
         * @brief Function to set the names of multiple nodes in the graph.
         * @param nodenameSets A vector of strings representing the new names for the nodes.
         * @param nodenameTargets A vector of strings representing the names of the nodes to change (default is an empty vector).
         * @return A pointer to the updated graph.
         * @details Sets the names of the specified nodes to the given new names.
         * @details If provided with one parameter, controls the vector size and sets the node names if they are of the same size.
         * @details If provided with two parameters, changes the nodes in nodenameTargets with the values in nodenameSets.
         * @throw std::invalid_argument if the node names are not found in the graph or if the sizes of the vectors are not equal.
        */
        WeightedEdgeGraph* setNodesNames(const std::vector<std::string>& nodenameSets, const std::vector<std::string>& nodenameTargets = std::vector<std::string>());

        // getters
        /**
         * @brief Function to get the value of a node by its index.
         * @param node The index of the node.
         * @return The value of the specified node.
         * @throw std::invalid_argument if the node index is out of range.
        */
        double getNodeValue(int node)const;
        /**
         * @brief Function to get the value of a node by its name.
         * @param node The name of the node.
         * @return The value of the specified node.
         * @throw std::invalid_argument if the node name is not found in the graph.
        */
        double getNodeValue(std::string node)const;
        /**
         * @brief Function to get the values of multiple nodes by their indexes
         * @param node A vector of integers representing the indexes of the nodes
         * @return A vector of doubles representing the values of the specified nodes.
         * @details If the vector is empty, returns all node values.
         * @throw std::invalid_argument if the node indexes are out of range
         */
        std::vector<double> getNodeValues(const std::vector<int>& node)const;
        /**
         * @brief Function to get the values of multiple nodes by their names
         * @param node A vector of strings representing the names of the nodes
         * @return A vector of doubles representing the values of the specified nodes.
         * @details If the vector is empty, returns all node values.
         * @throw std::invalid_argument if the node names are not found in the graph
         */
        std::vector<double> getNodeValues(const std::vector<std::string>& node=std::vector<std::string>())const;

        //optimization functions to make new Matrix, SUGGESTED not using these functions
        /**
         * @brief Function to create a new adjacency matrix from the current graph.
         * @return A new adjacency matrix representing the graph.
         * @details This function creates a new adjacency matrix from the current graph. It is suggested not to use this function as it may cause memory leaks.
         * @warning This function is not implemented completely and will probably be removed in the future.
         */
        Matrix<double> makeMatrix();

        // accessory functions

        /**
         * @brief Function to get the number of nodes in the graph(immutable).
         * @return The number of nodes in the graph.
         */
        int getNumNodes()const ;
        /**
         * @brief Function to get the number of edges in the graph(immutable).
         * @return The number of edges in the graph.
         */
        int getNumEdges()const ;
        /**
         * @brief Function to get the names of the nodes in the graph(immutable).
         * @return A vector of strings representing the names of the nodes.
         */
        std::vector<std::string> getNodeNames()const{ return nameVector;}
        /**
         * @brief Function to get the out degree of a certain node in the graph(immutable).
         * @param node The index of the node.
         * @return an integer representing the out degree of the node.
         * @details The out degree of a node is the number of edges that are directed away from the node.
         * @throw std::invalid_argument if the node index is out of range.(-1 or greater than the number of nodes)
         */
        int outDegreeOfNode(int node)const;
        /**
         * @brief Function to get the in degree of a certain node in the graph(immutable).
         * @param node The index of the node.
         * @return an integer representing the in degree of the node.
         * @details The in degree of a node is the number of edges that are directed towards the node.
         * @throw std::invalid_argument if the node index is out of range.(-1 or greater than the number of nodes)
         */
        int inDegreeOfNode(int node)const;
        /**
         * @brief Function to get the degree of a certain node in the graph(immutable).
         * @param node The index of the node.
         * @return an integer representing the degree of the node.
         * @details The degree of a node is the number of edges that are connected to the node.
         * @details The degree of a node is the sum of its in degree and out degree.
         * @throw std::invalid_argument if the node index is out of range.(-1 or greater than the number of nodes)
         */
        int degreeOfNode(int node)const;
        /**
         * @brief Function to know if a node is in the graph(immutable)
         * @param node The name of the node.
         * @return true if the node is in the graph, false otherwise.
         */
        bool containsNode(std::string node)const;


        /**
         * @brief Function to get the string representation of the node values(immutable)
         * @return A string representing the node values.
         * @details This function returns a string representation of the node values in the graph.
         */
        std::string getnodeValuesStr()const;

        /**
         * @brief Function to get the adjacency list of a node by its index(immutable).
         * @param node The index of the node.
         * @return A vector of integers representing the adjacent nodes.
         * @throw std::invalid_argument if the node index is out of range.(-1 or greater than the number of nodes)
         */
        std::unordered_set<int> getAdjList(int node)const;
        /**
         * @brief Function to get the adjacency list of a node by its name(immutable).
         * @param node The name of the node.
         * @return A vector of integers representing the adjacent nodes.
         * @throw std::invalid_argument if the node name is not found in the graph.
         * @details Internally calls the getAdjList(int node) function.
         * @see getAdjList(int node)
         * @see getIndexFromName(std::string name)
         * @see getNodeToIndexMap()
         */
        std::unordered_set<int> getAdjList(std::string node)const;

        /**
         * @brief Function to get the string representation of the adjacency list of a node by its index(immutable).
         * @param node The index of the node.
         * @return A string representing the adjacency list of the specified node.
         * @throw std::invalid_argument if the node index is out of range.(-1 or greater than the number of nodes)
         * @details This function uses the getAdjList(int node) function to get the adjacency list of the specified node and then converts it to a string.
         * @see getAdjList(int node)
         */
        std::string getAdjListStr(int node)const;
        /**
         * @brief Function to get the string representation of the adjacency list of a node by its name(immutable).
         * @param node The name of the node.
         * @return A string representing the adjacency list of the specified node.
         * @throw std::invalid_argument if the node name is not found in the graph.
         * @details This function uses the getAdjList(std::string node) function to get the adjacency list of the specified node and then converts it to a string.
         * @see getAdjList(std::string node)
         */
        std::string getAdjListStr(std::string node)const;


        //boolean functions to control adjacency or connectection(using adjacency list)
        /**
         * @brief Function to check if two nodes are adjacent by their indexes.
         * @param node1 The index of the first node.
         * @param node2 The index of the second node.
         * @return true if the nodes are adjacent, false otherwise.
         */
        bool adjNodes(int node1, int node2);
        /**
         * @brief Function to check if two nodes are adjacent by their names.
         * @param node1 The name of the first node.
         * @param node2 The name of the second node.
         * @return true if the nodes are adjacent, false otherwise.
         */
        bool adjNodes(std::string node1, std::string node2);
        /**
         * @brief Function to check if two nodes are connected by their indexes.
         * @param node1 The index of the first node.
         * @param node2 The index of the second node.
         * @return true if the nodes are connected, false otherwise.
         * @details Two nodes are connected if there is a path between them.
         * @details This function uses the adjacency list to check if the nodes are connected.
         * @warning This function is not implemented yet.
         * @warning The order of the nodes is important. The function will return true only if there exists a path from node1 to node2.
         */
        bool connectedNodes(int node1, int node2);
        /**
         * @brief Function to check if two nodes are connected by their names.
         * @param node1 The name of the first node. It is used as the starting point of the search.
         * @param node2 The name of the second node. It is used as the target of the search.
         * @return true if the nodes are connected, false otherwise.
         * @details Two nodes are connected if there is a path between them.
         * @details This function uses the adjacency list to check if the nodes are connected.
         * @warning This function is not completely implemented yet. Only returns true if the nodes are also adjacent.
         * @warning The order of the nodes is important. The function will return true only if there exists a path from node1 to node2.
         */
        bool connectedNodes(std::string node1, std::string node2);
        // TODO implement above functions using adjacency matrix

        /**
         * @brief Function to get the edges of the graph as a vector of tuples.
         * @return A vector of tuples representing the edges of the graph.
         * @details Each tuple contains the index of the first node, the index of the second node, and the weight of the edge.
         */
        std::vector<std::tuple<int, int, double>> getEdgesVector()const;


        /**
         * @brief Operator to assign a new graph to the current graph.
         * @param g2 The graph to assign.
         * @return A reference to the current graph.
         * @details This operator assigns a new graph to the current graph. It copies the adjacency matrix, node values, and adjacency list from the new graph.
         */
        WeightedEdgeGraph& operator=(const WeightedEdgeGraph& g2);
        /**
         * @brief Function to assign a new graph to the current graph.
         * @param g2 The graph to assign.
         * @details This function assigns a new graph to the current graph. It copies the adjacency matrix, node values, and adjacency list from the new graph.
         * @warning This function is not implemented yet.
         */
        void assign(const WeightedEdgeGraph& g2);
        /**
         * @brief Function to copy the current graph into a new graph in dynamic memory.
         * @return A pointer to the new graph.
         * @details This function copies the current graph into a new graph in dynamic memory. It uses the copy constructor to create the new graph.
         * @warning This function is not implemented yet.
         */
        WeightedEdgeGraph* copyNew()const;

        // optimization methods

        /**
         * @brief Function to get the maximum degree of the graph(immutable).
         * @return The maximum degree of the graph.
         * @details The maximum degree of the graph is the maximum degree of all nodes in the graph.
         */
        int getMaxDegree()const;
        /**
         * @brief Function to get the average degree of the graph(immutable).
         * @return The average degree of the graph.
         * @details The average degree of the graph is the sum of the degrees of all nodes divided by the number of nodes.
         */
        double getAverageDegree()const;

        /**
         * @brief Function to get the node to index map(immutable).
         * @return A map of node names to indexes.
         * @details This function just returns the private member nodeToIndex.
         */
        std::map<std::string, int> getNodeToIndexMap()const {return nodeToIndex;}
        /**
         * @brief Function to print the graph
         * @details This function prints the graph to the standard output. It uses the operator<< to print the graph.
         */
        void print()const;
        /**
         * @brief Function to save the graph edges to a file.
         * @param outputFolder The folder where the file will be saved.
         * @param filename The name of the file.
         * @details This function saves the graph edges to a file. The file will be saved in the specified folder with the specified name.
         * @details The file will be saved in the following format:
         * ```
         * node1 node2 weight
         * ```
         * @details The function will overwrite the file if it already exists.
         * @throw std::invalid_argument if the file is not opened.
         */
        void saveEdgesToFile(std::string outputFolder, std::string filename)const;

};

/**
 * @brief Operator to print the graph.
 * @param out The output stream.
 * @param data The graph to print.
 * @return A reference to the output stream.
 * @details This operator prints the graph to the specified output stream. It uses the print function of the graph class.
 */
std::ostream& operator<< (std::ostream &out, const WeightedEdgeGraph& data); 