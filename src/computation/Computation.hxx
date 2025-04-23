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
        Computation();
        ~Computation();
        Computation(std::string _thisCellType, const std::vector<double>& _input);   // useless???
        /**
         * \brief constructor function without knowledge of the other cell types, this part can be seen as the classical algorithm without additional computation for message passing between cells, only intra-cell propagation
         * @param std::string _thisCellType: the type of this computation, this information will be used as the unique name for the Agent
         * @param const std::vector<double>& _input: input vector of the nodes values, initially the one passed in the input
         * @param const Matrix<double>& _W: the adjacency matrix along the values of every edge in the graph that it represents
         * @param const std::vector<std::string>& graphNames: the graph nodes names, in order defined by the adjacency matrix
         * \return nothing
        */
        Computation(std::string _thisCellType,const std::vector<double>& _input, const Matrix<double>& _W, const std::vector<std::string>& graphNames);


        /**
         * \brief constructor function Computation without knowledge of the other cell types, this part can be seen as the classical algorithm without additional computation for message passing between cells, only intra-cell propagation
         * @param std::string _thisCellType: the type of this computation, this information will be used as the unique name for the Agent
         * @param const std::vector<double>& _input: input vector of the nodes values, initially the one passed in the input
         * @param const Matrix<double>& _W: the adjacency matrix along the values of every edge in the graph that it represents
         * @param const std::vector<std::string>& graphNames: the graph nodes names, in order defined by the adjacency matrix
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
        
        std::vector<double> getInput()const{return input;}
        std::vector<double> getOutput()const{return output;}
        std::vector<double> getInputAugmented()const{return inputAugmented;}
        std::vector<double> getOutputAugmented()const{return outputAugmented;}
        WeightedEdgeGraph* getGraph()const{return graph;}
        WeightedEdgeGraph* getAugmentedGraph()const{return augmentedGraph;}
        std::vector<std::string> getCellTypes()const{return types;}
        std::string getLocalCellType()const{return localType;}
        bool isInitializedArmaNotAugmented()const{return armaInitializedNotAugmented;}
        bool isInitializedArmaAugmented()const{return armaInitializedAugmented;}
        arma::Col<double> getInputArma()const{return InputArma;}
        arma::Mat<double> getPseudoInverseArma()const{return pseudoInverseArma;}
        arma::Col<double> getInputAugmentedArma()const{return InputAugmentedArma;}
        arma::Mat<double> getPseudoInverseAugmentedArma()const{return pseudoInverseAugmentedArma;}
        /**
         * \brief get the value of a node in the graph
         * @param std::string nodeName: the name of the node in the graph
        */
        double getOutputNodeValue(std::string nodeName)const{
            if(nodeToIndex.find(nodeName) == nodeToIndex.end()){
                // TESTING
                // printing the node names
                // std::cout << "Node names: ";
                // for(auto it = nodeToIndex.begin(); it != nodeToIndex.end(); it++){
                //     std::cout << it->first << " ";
                // }
                // std::cout << std::endl;
                // TESTING
                throw std::out_of_range("Computation::getOutputNodeValue: the node name is not in the graph");
            }
            int index = nodeToIndex.at(nodeName);
            return outputAugmented[index];
            };
        /**
         * \brief get the value of a node in the graph
         * @param std::string nodeName: the name of the node in the graph
         */
        double getInputNodeValue(std::string nodeName)const{
            if(nodeToIndex.find(nodeName) == nodeToIndex.end())
                throw std::out_of_range("Computation::getInputNodeValue: the node name is not in the graph");
            int index = nodeToIndex.at(nodeName);
            return inputAugmented[index];
            };
        /**
         * \brief get the value of a node in the graph
         * @param std::string nodeName: the name of the node in the graph
         */
        double getInputNodeValueArma(std::string nodeName)const{
            if(nodeToIndex.find(nodeName) == nodeToIndex.end())
                throw std::out_of_range("Computation::getInputNodeValueArma: the node name is not in the graph");
            int index = nodeToIndex.at(nodeName);
            return InputAugmentedArma[index];
            };
        /**
         * \brief set the value of a node in the graph
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

        double getVirtualInputForType(std::string type, std::string sourceNode="")const;
        double getVirtualOutputForType(std::string type, std::string targetNode="")const;
        void setInputVinForType(std::string type, double value, std::string sourceNode="");
        void setInputVoutForType(std::string type, double value, std::string targetNode="");
        void setDissipationModel(DissipationModel* dissipationModel);
        void setConservationModel(ConservationModel* conservationModel);
        void setPropagationModel(PropagationModel* propagationModel);
        void setInputAugmented(const std::vector<double>& inputAugmented);
        void setGraph(WeightedEdgeGraph* _graph){this->graph = _graph;} // only used for testing and pointer management


        // get-set for saturation function
        std::function<double(double,double)> getSaturationFunction()const{return saturationFunction;}
        void setSaturationFunction(std::function<double(double,double)> saturationFunction){this->saturationFunction = saturationFunction;}

        void resetVirtualOutputs();

        //optimization
        void freeAugmentedGraphs();
        void freeFunctions();

        // operators
        Computation& operator=( const Computation& );
        Computation copy()const;
        void assign(const Computation&);
        
};