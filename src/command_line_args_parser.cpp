#include "command_line_args_parser.h"

#include <boost/program_options.hpp>
#include <iostream>
#include <utility>

#include "common_functions.h"

bool CommandLineArgsParser::parse(int argc, char* argv[]) {
    if (!m_configFileName.empty()) {
        std::cerr << "{CommandLineArgsParser::parse}; configuration file name is already set" << std::endl;
        return false;
    }

    try {
        std::string configFileName;
        boost::program_options::options_description description;
        description.add_options()
            (
                "help,h", "Display help message"
            )
            (
                "config,c",
                boost::program_options::value<std::string>(
                    &configFileName
                )->required(), "Path to configuration file"
            );

        boost::program_options::variables_map variables;
        boost::program_options::store(
            boost::program_options::parse_command_line(
                argc, argv, description
            ), variables
        );

        if ((variables.count("help") > 0) && (variables.count("config") > 0)) {
            std::cerr << "{CommandLineArgsParser::parse}; select only one option: '--help' or '--config'" << std::endl;
            return false;
        }
        if (variables.count("help") > 0) {
            std::cout << description;
            return false;
        }
        boost::program_options::notify(variables);

        if (!CommonFunctions::fileExists(configFileName)) {
            return false;
        }
        if (!CommonFunctions::isRegularFile(configFileName)) {
            return false;
        }
        m_configFileName = std::move(configFileName);
    } catch (const boost::program_options::error& exception) {
        std::cerr << "{CommandLineArgsParser::parse}; "
            "exception 'boost::program_options::error' was successfully caught while "
            "parsing command line arguments; "
            "exception description: '" << exception.what() << "'" << std::endl;
        return false;
    } catch (...) {
        std::cerr << "{CommandLineArgsParser::parse}; "
            "unknown exception was caught while "
            "parsing command line arguments" << std::endl;
        CommonFunctions::printDiagnosticInfo();
        return false;
    }
    return true;
}
