#include "utils/boost_ignore_numbers_parser.hxx"

std::vector<po::option> ignore_numbers(std::vector<std::string>& args)
{
    std::vector<po::option> result;
    int position = 0;
    while(!args.empty()) {
        const auto& arg = args[0];
        double num;
        if(boost::conversion::try_lexical_convert(arg, num)) {
            result.push_back(po::option());
            po::option& option = result.back();

            option.position_key = position++;
            option.value.push_back(arg);
            option.original_tokens.push_back(arg);

            args.erase(args.begin());
        } else {
            break;
        }
    }

    return result;
}