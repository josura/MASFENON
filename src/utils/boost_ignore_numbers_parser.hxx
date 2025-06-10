#include <vector>
#include <string>
#include <boost/program_options.hpp>
#include <boost/program_options/option.hpp>
#include <boost/lexical_cast/try_lexical_convert.hpp>
#include <boost/program_options/value_semantic.hpp>



namespace po = boost::program_options;

std::vector<po::option> ignore_numbers(std::vector<std::string>& args);
