/**
 * @brief This file contains the implementation of the string utilities functions
 * @details The functions are used to work on strings
 * @ingroup Core
 * @file stringUtilities.cxx
 */
#include "utils/stringUtilities.hxx"


std::vector<std::string> splitStringIntoVector(std::string toSplit , std::string delimiter){

    std::vector<std::string> tokens;
    boost::split(tokens, toSplit, boost::is_any_of(delimiter));
    return tokens;
}


std::vector<std::string> splitStringIntoVectorTwoParts(std::string toSplit , std::string delimiter){
    std::vector<std::string> ret;
    // split the string into two parts, the first delimiter found is the split point, the delimiter is not included in the result
    int index = toSplit.find(delimiter);
    std::string firstPart = toSplit.substr(0, index);
    std::string secondPart = "";
    if(index >= 0){
        secondPart = toSplit.substr(index + delimiter.length());
    }
    ret.push_back(firstPart);
    if(secondPart.length()>0)
        ret.push_back(secondPart);
    return ret;
}


std::vector<std::string> splitVirtualNodeStringIntoVector(std::string toSplit){
    std::vector<std::string> ret;
    std::vector<std::string> splitted = splitStringIntoVectorTwoParts(toSplit, ":");
    if(splitted.size()==2){
        // first element is the virtual node type (v-in or v-out)
        // second element is the virtual node name (the type of the node, and additionally the name of the node)
        // TODO check if it is a virtual input or a virtual output
        ret.push_back(splitted[0]);
        std::vector<std::string> splittedSecond = splitStringIntoVectorTwoParts(splitted[1], "_");
        if(splittedSecond.size()==2){
            ret.push_back(splittedSecond[0]);
            ret.push_back(splittedSecond[1]);
        } else if(splittedSecond.size()==1){
            ret.push_back(splittedSecond[0]);
        } else {
            throw std::invalid_argument("utilities::splitVirtualNodeStringIntoVector: invalid virtual node string " + toSplit);
        }
    } else {
        throw std::invalid_argument("utilities::splitVirtualNodeStringIntoVector: invalid virtual node string " + toSplit);
    }
    return ret;

}

boost::tokenizer<boost::char_delimiters_separator<char>> splitStringIntoVectorTokenizer(std::string toSplit , char delimiter){
    boost::char_delimiters_separator<char> sep(delimiter);
    boost::tokenizer<boost::char_delimiters_separator<char> > tokens(toSplit, sep);
    return tokens;
}

