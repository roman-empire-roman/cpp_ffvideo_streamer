#include "common_functions.h"

extern "C" {
    #include <libavutil/avutil.h>
}

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/format.hpp>
#include <boost/version.hpp>
#include <chrono>
#include <filesystem>
#include <frozen/bits/version.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <rapidjson/rapidjson.h>
#include <sstream>
#include <string_view>
#include <unordered_set>
#include <vector>

#include <Poco/Exception.h>
#include <Poco/Net/DNS.h>
#include <Poco/Net/HostEntry.h>
#include <Poco/Net/IPAddress.h>
#include <Poco/Net/NetException.h>
#include <Poco/URI.h>
#include <Poco/Version.h>

#include "lodepng.h"

bool CommonFunctions::fileExists(const std::string& fileName) {
    if (fileName.empty()) {
        std::cerr << "{CommonFunctions::fileExists}; file name is empty" << std::endl;
        return false;
    }
    if (!std::filesystem::exists(fileName)) {
        std::cerr << "{CommonFunctions::fileExists}; file '" << fileName << "' does NOT exist" << std::endl;
        return false;
    }
    return true;
}

bool CommonFunctions::isRegularFile(const std::string& fileName) {
    if (fileName.empty()) {
        std::cerr << "{CommonFunctions::isRegularFile}; file name is empty" << std::endl;
        return false;
    }
    if (!std::filesystem::is_regular_file(fileName)) {
        std::cerr << "{CommonFunctions::isRegularFile}; file '" << fileName << "' is NOT a regular file" << std::endl;
        return false;
    }
    return true;
}

bool CommonFunctions::isCharacterFile(const std::string& fileName) {
    if (fileName.empty()) {
        std::cerr << "{CommonFunctions::isCharacterFile}; file name is empty" << std::endl;
        return false;
    }
    if (!std::filesystem::is_character_file(fileName)) {
        std::cerr << "{CommonFunctions::isCharacterFile}; file '" << fileName << "' is NOT a character file" << std::endl;
        return false;
    }
    return true;
}

bool CommonFunctions::getFileContents(const std::string& fileName, std::string& fileContents) {
    fileContents.clear();
    if (fileName.empty()) {
        std::cerr << "{CommonFunctions::getFileContents}; file name is empty" << std::endl;
        return false;
    }

    std::ifstream fileStream(fileName);
    if (!fileStream.is_open()) {
        std::cerr << "{CommonFunctions::getFileContents}; unable to open file '" << fileName << "'" << std::endl;
        return false;
    }
    std::stringstream stringBuffer;
    stringBuffer << fileStream.rdbuf();
    fileContents = stringBuffer.str();
    return true;
}

std::int64_t CommonFunctions::getCurTimeSinceEpoch() {
    return std::chrono::duration_cast< std::chrono::microseconds >(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

std::optional<std::int64_t> CommonFunctions::getDiffTime(std::int64_t beginTime, std::int64_t endTime) {
    if (beginTime < 0) {
        std::cerr << "{CommonFunctions::getDiffTime}; begin time is less than zero" << std::endl;
        return std::nullopt;
    }
    if (endTime < 0) {
        std::cerr << "{CommonFunctions::getDiffTime}; end time is less than zero" << std::endl;
        return std::nullopt;
    }
    if (endTime < beginTime) {
        std::cerr << "{CommonFunctions::getDiffTime}; end time is less than begin time" << std::endl;
        return std::nullopt;
    }
    return std::make_optional<std::int64_t>(
        std::chrono::duration<std::int64_t, std::micro>(
            endTime - beginTime
        ).count()
    );
}

std::optional<std::string> CommonFunctions::extractHostNameFromRtmpUrl(const std::string& rtmpUrl) {
    if (rtmpUrl.empty()) {
        std::cerr << "{CommonFunctions::extractHostNameFromRtmpUrl}; rtmp url is empty" << std::endl;
        return std::nullopt;
    }

    std::string hostName;
    try {
        Poco::URI url(rtmpUrl);
        hostName = url.getHost();
    } catch (const Poco::SyntaxException& exception) {
        std::cerr << "{CommonFunctions::extractHostNameFromRtmpUrl}; "
            "exception 'Poco::SyntaxException' was successfully caught; "
            "exception code: '" << exception.code() << "'; "
            "exception description: '" << exception.displayText() << "'; "
            "rtmp url: '" << rtmpUrl << "'" << std::endl;
        return std::nullopt;
    } catch (...) {
        std::cerr << "{CommonFunctions::extractHostNameFromRtmpUrl}; "
            "unknown exception was caught; "
            "rtmp url: '" << rtmpUrl << "'" << std::endl;
        return std::nullopt;
    }
    return std::make_optional<std::string>(hostName);
}

bool CommonFunctions::isHostNameValid(const std::string& hostName) {
    if (hostName.empty()) {
        std::cerr << "{CommonFunctions::isHostNameValid}; host name is empty" << std::endl;
        return false;
    }

    try {
        Poco::Net::IPAddress ipAddressWrapper(hostName);
        if (ipAddressWrapper.isWildcard()) {
            std::cerr << "{CommonFunctions::isHostNameValid}; IP address is NULL" << std::endl;
            return false;
        }
        auto ipAddress = ipAddressWrapper.toString();
        if (ipAddress.empty()) {
            std::cerr << "{CommonFunctions::isHostNameValid}; IP address is empty" << std::endl;
            return false;
        }
        std::cout << "{CommonFunctions::isHostNameValid}; IP address '" << ipAddress << "' is valid" << std::endl;
        return true;
    } catch ([[maybe_unused]] const Poco::Net::InvalidAddressException& exception) {
    } catch (...) {
    }

    try {
        Poco::Net::HostEntry hostEntry = Poco::Net::DNS::resolve(hostName);

        std::unordered_set<std::string> ipAddresses;
        for (const auto& ipAddressWrapper : hostEntry.addresses()) {
            if (ipAddressWrapper.isWildcard()) {
                continue;
            }
            auto ipAddress = ipAddressWrapper.toString();
            if (ipAddress.empty()) {
                continue;
            }
            ipAddresses.insert(ipAddress);
        }
        if (ipAddresses.empty()) {
            std::cerr << "{CommonFunctions::isHostNameValid}; host name '" << hostName << "' was NOT resolved to "
                "one or more IP addresses" << std::endl;
            return false;
        }
        if (1 == ipAddresses.size()) {
            std::cout << "{CommonFunctions::isHostNameValid}; host name '" << hostName << "' was successfully resolved to "
                "IP address '" << *ipAddresses.begin() << "'" << std::endl;
        } else {
            std::cout << "{CommonFunctions::isHostNameValid}; host name '" << hostName << "' was successfully resolved to "
                "'" << ipAddresses.size() << "' IP addresses" << std::endl;
        }
    } catch (const Poco::Net::HostNotFoundException& exception) {
        std::cerr << "{CommonFunctions::isHostNameValid}; "
            "host name '" << hostName << "' was NOT found; "
            "exception code: '" << exception.code() << "'; "
            "exception description: '" << exception.displayText() << "'" << std::endl;
        return false;
    } catch (const Poco::Net::NoAddressFoundException& exception) {
        std::cerr << "{CommonFunctions::isHostNameValid}; "
            "no IP addresses found for host name '" << hostName << "'; "
            "exception code: '" << exception.code() << "'; "
            "exception description: '" << exception.displayText() << "'" << std::endl;
        return false;
    } catch (const Poco::Net::DNSException& exception) {
        std::cerr << "{CommonFunctions::isHostNameValid}; "
            "exception 'Poco::Net::DNSException' was successfully caught; "
            "host name: '" << hostName << "'; "
            "exception code: '" << exception.code() << "'; "
            "exception description: '" << exception.displayText() << "'" << std::endl;
        return false;
    } catch (const Poco::IOException& exception) {
        std::cerr << "{CommonFunctions::isHostNameValid}; "
            "exception 'Poco::IOException' was successfully caught; "
            "host name: '" << hostName << "'; "
            "exception code: '" << exception.code() << "'; "
            "exception description: '" << exception.displayText() << "'" << std::endl;
        return false;
    } catch (...) {
        std::cerr << "{CommonFunctions::isHostNameValid}; "
            "unknown exception was caught; "
            "host name: '" << hostName << "'" << std::endl;
        return false;
    }
    return true;
}

bool CommonFunctions::getPngSize(const std::string& fileName, unsigned int& width, unsigned int& height) {
    width = 0;
    height = 0;
    if (fileName.empty()) {
        std::cerr << "{CommonFunctions::getPngSize}; file name is empty" << std::endl;
        return false;
    }

    try {
        std::vector<unsigned char> imageBuffer;
        unsigned int errorCode = lodepng::decode(imageBuffer, width, height, fileName);
        if (0 != errorCode) {
            std::cerr << "{CommonFunctions::getPngSize}; unable to get PNG image width and/or height; "
                "error code: '" << errorCode << " (" << lodepng_error_text(errorCode) << ")'; "
                "file name: '" << fileName << "'" << std::endl;
            return false;
        }
    } catch (const std::length_error& exception) {
        std::cerr << "{CommonFunctions::getPngSize}; "
            "exception 'std::length_error' was successfully caught; "
            "exception description: '" << exception.what() << "'; "
            "file name: '" << fileName << "'" << std::endl;
        return false;
    } catch (const std::bad_alloc& exception) {
        std::cerr << "{CommonFunctions::getPngSize}; "
            "exception 'std::bad_alloc' was successfully caught; "
            "exception description: '" << exception.what() << "'; "
            "file name: '" << fileName << "'" << std::endl;
        return false;
    } catch (...) {
        std::cerr << "{CommonFunctions::getPngSize}; "
            "unknown exception was caught while "
            "decoding PNG image; "
            "file name: '" << fileName << "'" << std::endl;
        return false;
    }
    return true;
}

void CommonFunctions::printDiagnosticInfo() {
    auto diagnosticInfo = boost::current_exception_diagnostic_information();
    boost::algorithm::replace_all(diagnosticInfo, "\n", " ");
    boost::algorithm::trim(diagnosticInfo);
    if (!diagnosticInfo.empty()) {
        std::cerr << "{CommonFunctions::printDiagnosticInfo}; "
            "'" << diagnosticInfo << "'" << std::endl;
    }
}

void CommonFunctions::printLibrariesVersions() {
    {
        using namespace std::literals;

        const char* rawFFmpegVersion = av_version_info();
        if (rawFFmpegVersion) {
            try {
                std::string_view ffmpegVersion(
                    rawFFmpegVersion, std::strlen(rawFFmpegVersion)
                );
                auto beginPos = ffmpegVersion.find_first_of("0123456789."sv);
                auto endPos = ffmpegVersion.find_last_of("0123456789."sv);
                if (
                    (std::string_view::npos != beginPos) &&
                    (std::string_view::npos != endPos) &&
                    (endPos >= beginPos)
                ) {
                    auto version = ffmpegVersion.substr(beginPos, (endPos - beginPos + 1));
                    std::cout << "{CommonFunctions::printLibrariesVersions}; "
                        "FFmpeg version: " << std::quoted(version, '\'') << std::endl;
                }
            } catch (const std::length_error& exception) {
                std::cerr << "{CommonFunctions::printLibrariesVersions}; "
                    "exception 'std::length_error' was successfully caught; "
                    "exception description: " << std::quoted(exception.what(), '\'') << std::endl;
            } catch (const std::out_of_range& exception) {
                std::cerr << "{CommonFunctions::printLibrariesVersions}; "
                    "exception 'std::out_of_range' was successfully caught; "
                    "exception description: " << std::quoted(exception.what(), '\'') << std::endl;
            } catch (...) {
                std::cerr << "{CommonFunctions::printLibrariesVersions}; "
                    "unknown exception was caught while "
                    "processing FFmpeg version" << std::endl;
            }
        }
    }

    std::cout << "{CommonFunctions::printLibrariesVersions}; Boost version: '"  <<
        boost::format("%1%.%2%.%3%") %
        static_cast<int>(BOOST_VERSION / 100000) %
        static_cast<int>((BOOST_VERSION / 100) % 1000) %
        static_cast<int>(BOOST_VERSION % 100) << "'" << std::endl;

    std::cout << "{CommonFunctions::printLibrariesVersions}; Frozen version: '"  <<
        boost::format("%1%.%2%.%3%") %
        static_cast<int>(FROZEN_MAJOR_VERSION) %
        static_cast<int>(FROZEN_MINOR_VERSION) %
        static_cast<int>(FROZEN_PATCH_VERSION) << "'" << std::endl;

    std::cout << "{CommonFunctions::printLibrariesVersions}; Poco version: '" <<
        boost::format("%1%.%2%.%3%") %
        static_cast<int>((POCO_VERSION >> 24) & 0xFF) %
        static_cast<int>((POCO_VERSION >> 16) & 0xFF) %
        static_cast<int>((POCO_VERSION >> 8) & 0xFF) << "'" << std::endl;

    std::cout << "{CommonFunctions::printLibrariesVersions}; RapidJSON version: '"  <<
        boost::format("%1%.%2%.%3%") %
        static_cast<int>(RAPIDJSON_MAJOR_VERSION) %
        static_cast<int>(RAPIDJSON_MINOR_VERSION) %
        static_cast<int>(RAPIDJSON_PATCH_VERSION) << "'" << std::endl;
}
