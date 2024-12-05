#ifndef COMMAND_LINE_ARGS_PARSER_H
#define COMMAND_LINE_ARGS_PARSER_H

#include <string>

class CommandLineArgsParser {
public:
    CommandLineArgsParser() = default;
    CommandLineArgsParser(const CommandLineArgsParser& other) = delete;
    CommandLineArgsParser& operator=(const CommandLineArgsParser& other) = delete;
    ~CommandLineArgsParser() = default;
    CommandLineArgsParser(CommandLineArgsParser&& other) = delete;
    CommandLineArgsParser& operator=(CommandLineArgsParser&& other) = delete;

    bool parse(int argc, char* argv[]);
    const std::string& getConfigFileName() const { return m_configFileName; }

private:
    std::string m_configFileName;
};

#endif // COMMAND_LINE_ARGS_PARSER_H
