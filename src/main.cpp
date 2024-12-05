#include "command_line_args_parser.h"
#include "common_functions.h"
#include "video_streamer.h"

int main(int argc, char* argv[]) {
    CommandLineArgsParser parser;
    if (!parser.parse(argc, argv)) {
        return -1;
    }
    const auto& configFileName = parser.getConfigFileName();

    CommonFunctions::printLibrariesVersions();

    VideoStreamer streamer;
    if (!streamer.setup(configFileName)) {
        return -1;
    }
    if (!streamer.process()) {
        return -1;
    }
    return 0;
}
