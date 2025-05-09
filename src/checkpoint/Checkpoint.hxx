/**
 * @file Checkpoint.hxx
 * @ingroup Core
 * @brief Defines the Checkpoint class used for saving and restoring computation states in the MASFENON framework.
 * @details The Checkpoint class allows saving intermediate states of simulations to disk, cleaning existing checkpoints, 
 * and reloading saved states for specific agent types and iteration steps.
 */
#pragma once
#include <string>
#include <vector>
#include "computation/Computation.hxx"

/**
 * @class Checkpoint
 * @brief Handles checkpointing functionality for the MASFENON simulation framework.
 * @details This class allows saving and restoring the computation state for each agent type at specific intra- and inter-iteration steps.
 */
class Checkpoint {
public:
    /**
     * @brief Constructor for the Checkpoint class.
     */
    Checkpoint();
    /**
     * @brief Destructor for the Checkpoint class.
     */
    ~Checkpoint();

    /**
     * @brief Save the state of the computation of a type in a file.
     * @param type The identifier of the agent type.
     * @param interIteration The current inter-iteration index.
     * @param intraIteration The current intra-iteration index.
     * @param currentComputation A pointer to the Computation object to save.
     */
    void saveState(const std::string type, const int interIteration, const int intraIteration, const Computation* currentComputation);
    /**
     * @brief Remove all the checkpoints of a given type.
     * @param type The identifier of the agent type whose checkpoints are to be deleted.
     */
    void cleanCheckpoints(const std::string type);
    /**
     * @brief Load the state of the computation of a type from a file.
     * @param type The identifier of the agent type.
     * @param interIteration The inter-iteration index from which to load.
     * @param intraIteration The intra-iteration index from which to load.
     * @param computation A pointer to the Computation object to load the state into.
     * @return A pointer to the loaded Computation object.
     */
    void loadState(const std::string type, int& interIteration, int& intraIteration, Computation* computation);

    /**
     * @brief Set the checkpoint folder.
     * @param folder The path to the folder where checkpoints will be saved.
     */
    void setCheckPointFolder(const std::string& folder);

private:
    std::string checkPointFolder; // The folder where checkpoints are saved.
};
