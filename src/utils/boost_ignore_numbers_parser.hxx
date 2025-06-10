/**
 * @file boost_ignore_numbers_parser.hxx
 * @ingroup Core
 * @brief  This file contains the declaration of the function used to ignore numbers (negative numbers) when parsing command line options 
 * @details  The function used to ignore numbers works with the arguments and returns a vector of program options
 */
#include <vector>
#include <string>
#include <boost/program_options.hpp>
#include <boost/program_options/option.hpp>
#include <boost/lexical_cast/try_lexical_convert.hpp>
#include <boost/program_options/value_semantic.hpp>



namespace po = boost::program_options;

/**
 * @brief Function used to ignore numbers whern parsing
 * @param args the command line parameters
 * @return The vector of program options
 * @details This checks if the token is a number (including negatives) and treats it as a positional option rather than a flag.
 */
std::vector<po::option> ignore_numbers(std::vector<std::string>& args);
