#pragma once
#include <vector>
#include <string>
#include <boost/token_functions.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

/**
 * \brief  Split a string into a vector of strings using the specified delimiter
 * \return  the vector of strings
*/
std::vector<std::string> splitStringIntoVector(std::string toSplit , std::string delimiter);

/**
 * \brief  Split a string into a vector of strings using the specified delimiter into two part, the first part is the string before the delimiter and the second part is the string after the delimiter
 * the first delimiter found is used
 * \return  the vector of strings
*/
std::vector<std::string> splitStringIntoVectorTwoParts(std::string toSplit , std::string delimiter);

/**
 * \brief  Split a string representing a virtual node (v-in or v-out) into a vector of strings, where the first element is the type and the second element is the name of the node if present
 * \return  the vector of strings
*/
std::vector<std::string> splitVirtualNodeStringIntoVector(std::string toSplit);
