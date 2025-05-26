/**
 * @file Checkpoint.cxx
 * @ingroup Core
 * @brief Implements the Checkpoint class for managing simulation states.
 * @details Includes logic for saving, loading, and cleaning checkpoint data during MASFENON simulations.
 */
#include "checkpoint/Checkpoint.hxx"
#include "computation/Computation.hxx"
#include "logging/Logger.hxx"
#include <string>
#include "utils/utilities.hxx"

Checkpoint::Checkpoint() {
    this->checkPointFolder = "checkpoints/";
    // control if the folder exists
    // if not, create it
    if (folderExists(this->checkPointFolder) == false)
    {
        bool success = createFolder(this->checkPointFolder);
        if (!success)
        {
            Logger::getInstance().printError("Checkpoint::Checkpoint: Unable to create folder " + std::string(this->checkPointFolder));
            throw std::runtime_error("[ERROR] Checkpoint::Checkpoint: Unable to create folder " + this->checkPointFolder);
        }
    }
    
}

Checkpoint::~Checkpoint() {
    // Add your code here
}

void Checkpoint::saveState(const std::string type, const int interIteration, const int intraIteration, const Computation* currentComputation) {
    std::string fileName = this->checkPointFolder + "checkpoint_" + type + "_" + std::to_string(interIteration) + "_" + std::to_string(intraIteration) + ".tsv";
    std::ofstream file(fileName);
    std::vector<std::string> nodeNames = currentComputation->getAugmentedGraph()->getNodeNames();
    std::vector<double> nodeValues = currentComputation->getInputAugmented();
    if (file.is_open())
    {
        //header
        // file << "interIteration\tintraIteration\tnodeName\tnodeValue\n";
        file << "nodeName\tnodeValue\n";
        //body
        for(uint i = 0; i < nodeValues.size(); i++){
            // file<<interIteration<< "\t"<< intraIteration<<"\t"<< nodeNames[i]<<"\t"<<std::to_string(nodeValues[i]);
            file << nodeNames[i]<<"\t"<<std::to_string(nodeValues[i]);
            file << std::endl;
        }
        file.close();
    }
    else
    {
        Logger::getInstance().printError("Checkpoint::saveState: Unable to open file " + fileName);
    }
}

void Checkpoint::cleanCheckpoints(std::string type) {
    std::string folder = this->checkPointFolder;
    std::vector<std::string> files = listFiles(folder);
    for (std::string file : files)
    {
        if (file.find("checkpoint_" + type + "_") != std::string::npos)
        {
            // std::string fileName = folder + file;
            if (remove(file.c_str()))
            {
                Logger::getInstance().printError("Checkpoint::cleanCheckpoints: Unable to delete file " + file);
            }
        }
    }
}

void Checkpoint::loadState(const std::string type, int& interIteration, int& intraIteration, Computation* computation) {
    std::string fileName = this->checkPointFolder + "checkpoint_" + type + "_";
    std::vector<std::string> files = listFiles(this->checkPointFolder);
    bool checkPointExists = false;
    for (std::string file : files)
    {
        if (file.find(fileName) != std::string::npos)
        {
            interIteration = stoi(splitStringIntoVector(file, "_")[2]);
            intraIteration = stoi(splitStringIntoVector(file, "_")[3]);
            checkPointExists = true;
            fileName = file;
            break;
        }
    }

    if (!checkPointExists)
    {
        Logger::getInstance().printError("Checkpoint::loadState: Checkpoint file not found");
        throw std::runtime_error("[ERROR] Checkpoint::loadState: Checkpoint file not found");
    }
    

    std::ifstream file(fileName);
    if (file.is_open())
    {
        std::string line;
        std::getline(file, line); // skip header
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            // std::string interIterationStr;
            // std::string intraIterationStr;
            std::string nodeName;
            std::string nodeValueStr;
            // iss >> interIterationStr >> intraIterationStr >> nodeName >> nodeValueStr;
            iss >> nodeName >> nodeValueStr;
            double nodeValue = std::stod(nodeValueStr);
            // interIteration = std::stoi(interIterationStr);
            // intraIteration = std::stoi(intraIterationStr);
            computation->setInputNodeValue(nodeName, nodeValue);
        }
        file.close();
    }
    else
    {
        Logger::getInstance().printError("Checkpoint::loadState: Unable to open file " + fileName);
        throw std::runtime_error("[ERROR] Checkpoint::loadState: Unable to open file " + fileName);
    }
}