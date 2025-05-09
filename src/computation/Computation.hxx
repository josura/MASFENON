/**
 * @file Computation.hxx
 * @ingroup Core
 * @brief Defines the Computation class used for managing the logic of intra- and inter-network dynamics in MASFENON.
 * @details This class handles core data structures, including graphs and matrices, and executes propagation,
 * dissipation, and conservation steps using models. It supports augmented graphs and integrates with Armadillo for efficient computations.
 */
#pragma once

#include "computation/DissipationModel.hxx"
#include "computation/ConservationModel.hxx"
#include "computation/PropagationModel.hxx"
#include "data_structures/Matrix.hxx"
#include "data_structures/WeightedEdgeGraph.hxx"
#include <map>
#include <string>
#include <tuple>
#include <vector>
#include <functional>

/**
 * @class Computation
 * @brief Core class for executing propagation, dissipation, and conservation over a network in MASFENON.
 * @details Handles data transformations, internal and augmented graph representations, and provides matrix operations.
 */
class Computation{
    private:
        std::vector<double> input;                    /**< Input vector for the normal graph. */
        std::vector<double> output;                   /**< Output vector after computation on the normal graph. */
        std::vector<double> inputAugmented;           /**< Input vector for the augmented graph. */
        std::vector<double> outputAugmented;          /**< Output vector after computation on the augmented graph. */

        WeightedEdgeGraph* graph;                     /**< Pointer to the core graph. */
        WeightedEdgeGraph* augmentedGraph;            /**< Pointer to the augmented graph. */

        std::vector<std::string> types;           /**< List of all known cell types. */
        std::string localType;                    /**< The cell type of the current agent. */

        bool armaInitializedNotAugmented = false;     /**< Indicates whether the Armadillo structure is initialized for the core graph. */
        bool armaInitializedAugmented = false;        /**< Indicates whether the Armadillo structure is initialized for the augmented graph. */

        arma::Col<double> InputArma;                  /**< Armadillo vector of inputs for core graph. */
        arma::Mat<double> pseudoInverseArma;          /**< Armadillo pseudo-inverse matrix for core graph. */
        arma::Col<double> InputAugmentedArma;         /**< Armadillo vector of inputs for augmented graph. */
        arma::Mat<double> pseudoInverseAugmentedArma; /**< Armadillo pseudo-inverse matrix for augmented graph. */

        std::map<std::string, int> nodeToIndex;       /**< Maps node names to their indices. */

        DissipationModel* dissipationModel = nullptr;     /**< Pointer to dissipation model. */
        ConservationModel* conservationModel = nullptr;   /**< Pointer to conservation model. */
        PropagationModel* propagationModel = nullptr;     /**< Pointer to propagation model. */

        std::function<double(double,double)> saturationFunction; /**< Function to apply saturation logic to computed values. */

    public:
        /**
         * @brief Default constructor for Computation class.
         * @details Initializes the computation object with default values. Not typically used in practice.
         */
        Computation();
        /**
         * @brief Destructor for Computation class.
         * @details Cleans up resources and memory used by the computation object.
         */
        ~Computation();
        /**
         * @brief Construct a Computation object from cell type and input.
         * @param _thisCellType The cell type of the current agent.
         * @param _input Initial values for the computation.
         * @details technically, this constructor is not used in the code, but it is useful for testing purposes.
         */
        Computation(std::string _thisCellType, const std::vector<double>& _input); 
        /**
         * @brief constructor function without knowledge of the other cell types, this part can be seen as the classical algorithm without additional computation for message passing between cells, only intra-cell propagation
         * @param _thisCellType: the type of this computation, this information will be used as the unique name for the Agent
         * @param _input: input vector of the nodes values, initially the one passed in the input
         * @param _W: the adjacency matrix along the values of every edge in the graph that it represents
         * @param graphNames: the graph nodes names, in order defined by the adjacency matrix
        */
        Computation(std::string _thisCellType,const std::vector<double>& _input, const Matrix<double>& _W, const std::vector<std::string>& graphNames);


        /**
         * @brief constructor function Computation without knowledge of the other cell types, this part can be seen as the classical algorithm without additional computation for message passing between cells, only intra-cell propagation
         * @param _thisCellType: the type of this computation, this information will be used as the unique name for the Agent
         * @param _input: input vector of the nodes values, initially the one passed in the input
         * @param _W: the adjacency matrix along the values of every edge in the graph that it represents
         * @param graphNames: the graph nodes names, in order defined by the adjacency matrix
        */
        Computation(std::string _thisCellType,const std::vector<double>& _input, WeightedEdgeGraph* _graph, const std::vector<std::string>& graphNames);
        
        /*
        Augment the graph with types and a new set of edges from virtual nodes in the augmented graph to the graph(virtual inputs and virtual outputs) 
        @param const std::vector<std::string>& _types: the types other than this type, the other agents in the network
        @param 
        */
        void augmentGraph(const std::vector<std::string>&,const std::vector<std::pair<std::string,std::string>>& newEdgesList =std::vector<std::pair<std::string,std::string>>(), const std::vector<double>& newEdgesValue = std::vector<double>(), bool includeSelfVirtual=false);
        void augmentGraphNoComputeInverse(const std::vector<std::string>&,const std::vector<std::pair<std::string,std::string>>& newEdgesList =std::vector<std::pair<std::string,std::string>>(), const std::vector<double>& newEdgesValue = std::vector<double>(), bool includeSelfVirtual=false);
        void addEdges(const std::vector<std::pair<std::string,std::string>>& , const std::vector<double>& , bool bothDirections = false, bool inverseComputation = true);
        void addEdges(const std::vector<std::tuple<std::string,std::string,double>>&  , bool bothDirections = false, bool inverseComputation = true);
        void addEdges(const std::vector<std::pair<int,int>>& , const std::vector<double>& , bool bothDirections = false, bool inverseComputation = true);
        void addEdges(const std::vector<std::tuple<int,int,double>>&  , bool bothDirections = false, bool inverseComputation = true);
        void addEdgesAndNodes(const std::vector<std::tuple<std::string,std::string,double>>&  , bool bothDirections = false, bool inverseComputation = true);
        std::vector<double> computePerturbation();
        std::vector<double> computeAugmentedPerturbation(); //taking into account virtual nodes in the augmented graph
        std::vector<double> computeAugmentedPerturbationDissipatedAfterCompute(double timeStep); //taking into account dissipation after every iteration(Dissipation model), dissipation after the computation of the perturbated value
        std::vector<double> computeAugmentedPerturbationDissipatedBeforeCompute(double timeStep); //taking into account dissipation after every iteration (Dissipation model), dissipation before the computation of the perturbated value
        std::vector<double> computeAugmentedPerturbationSaturatedAndDissipatedBeforeCompute(double timeStep,const std::vector<double>& saturationsVector = std::vector<double>()); //taking into account saturation(hyperbolic tangent and scaling) and dissipation after every iteration
        std::vector<double> computeAugmentedPerturbationEnhanced2(double timeStep, bool saturation = true, const std::vector<double>& saturationsVector = std::vector<double>(),const std::vector<double>& qVector = std::vector<double>()); //taking into account saturation(hyperbolic tangent and scaling), dissipation and conservation after every iteration
        std::vector<double> computeAugmentedPerturbationEnhanced3(double timeStep, bool saturation = true, const std::vector<double>& saturationsVector = std::vector<double>(),const std::vector<double>& qVector = std::vector<double>(), std::function<double(double)> propagationScaleFunction = [](double time)-> double{return 1.0;}); //taking into account scaling
        std::vector<double> computeAugmentedPerturbationEnhanced4(double timeStep, bool saturation = true, const std::vector<double>& saturationsVector = std::vector<double>(),const std::vector<double>& qVector = std::vector<double>()); //all the models
        std::pair<std::string,double> getMapVirtualOutputsToCellInputs(); //TODO
        void updateInput(const std::vector<double>& newInp = std::vector<double>(), bool augmented = false);

        // get sets
        /**
         * @brief Getting input of the Computation object
         * @details These functions provide access to the private members of the class, allowing read-only access to the data.
         * @return The corresponding private member.
         */
        std::vector<double> getInput()const{return input;}
        /**
         * @brief Getting output of the Computation object
         * @details These functions provide access to the private members of the class, allowing read-only access to the data.
         * @return The corresponding private member.
         */
        std::vector<double> getOutput()const{return output;}
        /**
         * @brief Getting input augmented of the Computation object
         * @details These functions provide access to the private members of the class, allowing read-only access to the data.
         * @return The corresponding private member.
         */
        std::vector<double> getInputAugmented()const{return inputAugmented;}
        /**
         * @brief Getting output augmented of the Computation object
         * @details These functions provide access to the private members of the class, allowing read-only access to the data.
         * @return The corresponding private member.
         */
        std::vector<double> getOutputAugmented()const{return outputAugmented;}
        /**
         * @brief Getting the graph pointer of the Computation object
         * @details These functions provide access to the private members of the class, allowing read-only access to the data.
         * @return The corresponding private member.
         */
        WeightedEdgeGraph* getGraph()const{return graph;}
        /**
         * @brief Getting the augmented graph pointer of the Computation object
         * @details These functions provide access to the private members of the class, allowing read-only access to the data.
         * @return The corresponding private member.
         */
        WeightedEdgeGraph* getAugmentedGraph()const{return augmentedGraph;}
        /**
         * @brief Getting the types of the Computation object
         * @details These functions provide access to the private members of the class, allowing read-only access to the data.
         * @return The corresponding private member.
         */
        std::vector<std::string> getTypes()const{return types;}
        /**
         * @brief Getting the local type of the Computation object
         * @details These functions provide access to the private members of the class, allowing read-only access to the data.
         * @return The corresponding private member.
         */
        std::string getLocalType()const{return localType;}
        /**
         * @brief Know if the Armadillo structure is initialized for the core graph
         * @details These functions provide access to the private members of the class, allowing read-only access to the data.
         * @return True if the Armadillo structure is initialized for the core graph, false otherwise.
         */
        bool isInitializedArmaNotAugmented()const{return armaInitializedNotAugmented;}
        /**
         * @brief Know if the Armadillo structure is initialized for the augmented graph
         * @details These functions provide access to the private members of the class, allowing read-only access to the data.
         * @return True if the Armadillo structure is initialized for the augmented graph, false otherwise.
         */
        bool isInitializedArmaAugmented()const{return armaInitializedAugmented;}
        /**
         * @brief Getting the Armadillo input vector of the Computation object
         * @details These functions provide access to the private members of the class, allowing read-only access to the data.
         * @return The corresponding private member.
         */
        arma::Col<double> getInputArma()const{return InputArma;}
        /**
         * @brief Getting the Armadillo pseudo-inverse matrix of the Computation object
         * @details These functions provide access to the private members of the class, allowing read-only access to the data.
         * @return The corresponding private member.
         */
        arma::Mat<double> getPseudoInverseArma()const{return pseudoInverseArma;}
        /**
         * @brief Getting the Armadillo input augmented vector of the Computation object
         * @details These functions provide access to the private members of the class, allowing read-only access to the data.
         * @return The corresponding private member.
         */
        arma::Col<double> getInputAugmentedArma()const{return InputAugmentedArma;}
        /**
         * @brief Getting the Armadillo pseudo-inverse matrix of the augmented graph
         * @details These functions provide access to the private members of the class, allowing read-only access to the data.
         * @return The corresponding private member.
         */
        arma::Mat<double> getPseudoInverseAugmentedArma()const{return pseudoInverseAugmentedArma;}
        /**
         * @brief get the output value of a node in the graph
         * @param std::string nodeName: the name of the node in the graph
         * @return double: the value of the node in the graph
        */
        double getOutputNodeValue(std::string nodeName)const{
            if(nodeToIndex.find(nodeName) == nodeToIndex.end()){
                throw std::out_of_range("Computation::getOutputNodeValue: the node name is not in the graph");
            }
            int index = nodeToIndex.at(nodeName);
            return outputAugmented[index];
            };
        /**
         * @brief get the input value of a node in the graph
         * @param std::string nodeName: the name of the node in the graph
         * @return double: the value of the node in the graph
         */
        double getInputNodeValue(std::string nodeName)const{
            if(nodeToIndex.find(nodeName) == nodeToIndex.end())
                throw std::out_of_range("Computation::getInputNodeValue: the node name is not in the graph");
            int index = nodeToIndex.at(nodeName);
            return inputAugmented[index];
            };
        /**
         * @brief get the value of a node in the graph in the Armadillo structure
         * @param std::string nodeName: the name of the node in the graph
         * @return double: the value of the node in the graph
         */
        double getInputNodeValueArma(std::string nodeName)const{
            if(nodeToIndex.find(nodeName) == nodeToIndex.end())
                throw std::out_of_range("Computation::getInputNodeValueArma: the node name is not in the graph");
            int index = nodeToIndex.at(nodeName);
            return InputAugmentedArma[index];
            };
        /**
         * @brief set the input value of a node in the graph
         * @param std::string nodeName: the name of the node in the graph
         * @param double value: the value to set
         */
        void setInputNodeValue(std::string nodeName, double value){
            if(nodeToIndex.find(nodeName) == nodeToIndex.end())
                throw std::out_of_range("Computation::setInputNodeValue: the node name is not in the graph");
            int index = nodeToIndex.at(nodeName);
            inputAugmented[index] = value;
            InputAugmentedArma[index] = value;
        };
        /**
         * @brief get the value of a virtual input node in the graph
         * @param std::string type: the type of the node in the source graph
         * @param std::string sourceNode: the name of the source node in the source graph
         * @return double: the value of the node in the graph
         */
        double getVirtualInputForType(std::string type, std::string sourceNode="")const;
        /**
         * @brief get the value of a virtual output node in the graph
         * @param std::string type: the type of the node in the target graph
         * @param std::string targetNode: the name of the target node in the target graph
         * @return double: the value of the node in the graph
         */
        double getVirtualOutputForType(std::string type, std::string targetNode="")const;
        /**
         * @brief set the value of a virtual input node in the graph
         * @param std::string type: the type of the node in the source graph
         * @param double value: the value to set
         * @param std::string sourceNode: the name of the source node in the source graph
         */
        void setInputVinForType(std::string type, double value, std::string sourceNode="");
        /**
         * @brief set the value of a virtual output node in the graph
         * @param std::string type: the type of the node in the target graph
         * @param double value: the value to set
         * @param std::string targetNode: the name of the target node in the target graph
         */
        void setInputVoutForType(std::string type, double value, std::string targetNode="");
        /**
         * @brief set the Dissipation model of the graph (passing the pointer to the instance of the model)
         * @param DissipationModel* dissipationModel: the pointer to the instance of the model
         */
        void setDissipationModel(DissipationModel* dissipationModel);
        /**
         * @brief set the Conservation model of the graph (passing the pointer to the instance of the model)
         * @param ConservationModel* conservationModel: the pointer to the instance of the model
         */
        void setConservationModel(ConservationModel* conservationModel);
        /**
         * @brief set the Propagation model of the graph (passing the pointer to the instance of the model)
         * @param PropagationModel* propagationModel: the pointer to the instance of the model
         */
        void setPropagationModel(PropagationModel* propagationModel);
        /**
         * @brief set the value of all the input nodes in the graph
         * @param std::vector<double> input: the vector of values to set
         * @details This function sets the value of all the input nodes in the graph to the values in the vector.
         * @details The size of the vector must be equal to the number of nodes in the graph.
         * @details The function will throw an exception if the size of the vector is not equal to the number of nodes in the graph.
         */
        void setInputAugmented(const std::vector<double>& inputAugmented);
        /**
         * @brief set the graph of the computation object
         * @param WeightedEdgeGraph* _graph: the pointer to the graph
         * @details This function sets the graph of the computation object to the graph passed as a parameter.
         * @details This function is only used for testing and pointer management.
         * @details This function is not really used in the code in key phases, but it is useful for testing purposes and pointer management.
         */
        void setGraph(WeightedEdgeGraph* _graph){this->graph = _graph;}


        /**
         * @brief get the saturation function
         * @details This function returns the saturation function used in the computation.
         * @details The saturation function is a function that takes two double values as input and returns a double value.
         * @return std::function<double(double,double)>: the saturation function        
         */
        std::function<double(double,double)> getSaturationFunction()const{return saturationFunction;}
        /**
         * @brief set the saturation function
         * @param std::function<double(double;double)> saturationFunction: the saturation function to set
         * @details This function sets the saturation function used in the computation.
         * @details The saturation function is a function that takes two double values as input and returns a double value.
         */
        void setSaturationFunction(std::function<double(double,double)> saturationFunction){this->saturationFunction = saturationFunction;}

        /**
         * @brief reset the virtual outputs for the computation, setting them to 0
         * @details This function resets the virtual output nodes values for the computation, setting them to 0.
         */
        void resetVirtualOutputs();

        //optimization
        /**
         * @brief free the Augmented graph
         * @details This function frees the memory used by the augmented graph.
         */
        void freeAugmentedGraphs();
        /**
         * @brief free the models used in the computation, that is the dissipation, conservation and propagation models
         * @details This function frees the memory used by the functions used in the computation.
         * @details At the moment only the dissipation model is freed, the conservation and propagation models are not freed since they are the same for each computation (for now)
         * @warning This function is unstable for now, it's only used for having a clean memory
         */
        void freeFunctions();

        // operators
        /**
         * @brief Assignment operator for Computation class.
         * @param rhs The right-hand side Computation object to assign from.
         * @details This operator allows for the assignment of one Computation object to another.
         * @return A reference to the current object.
         * @warning This operator does not perform a deep copy of the graph and augmented graph, only the pointers are copied.
         * @warning This operator does not perform a deep copy of the dissipation, conservation and propagation models, only the pointers are copied.
         */
        Computation& operator=( const Computation& );
        /**
         * @brief Copy constructor for Computation class.
         * @param rhs The right-hand side Computation object to copy from.
         * @details This constructor allows for the creation of a new Computation object as a copy of an existing one.
         * @return A reference to the new object.
         * @warning This constructor does not perform a deep copy of the graph and augmented graph, only the pointers are copied.
         * @warning This constructor does not perform a deep copy of the dissipation, conservation and propagation models, only the pointers are copied.
         */
        Computation copy()const;
        /**
         * @brief Assignment operator for Computation class.
         * @param rhs The right-hand side Computation object to assign from.
         * @details This operator allows for the assignment of one Computation object to another.
         * @return A reference to the current object.
         */
        void assign(const Computation&);
        
};