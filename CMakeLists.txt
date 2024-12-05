cmake_minimum_required(VERSION 3.30.2)
project(cpp_ffvideo_streamer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -v")

set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

file(GLOB SRC_FILES "src/*.cpp")

find_package(Boost REQUIRED COMPONENTS program_options)
find_package(Poco REQUIRED COMPONENTS Net Foundation)

if (NOT Boost_FOUND)
    message(FATAL_ERROR "Boost was NOT found")
endif()

if (NOT Poco_FOUND)
    message(FATAL_ERROR "Poco was NOT found")
endif()

set(Boost_USE_MULTITHREADED ON)

add_executable(cpp_ffvideo_streamer ${SRC_FILES})
target_compile_features(cpp_ffvideo_streamer PRIVATE cxx_std_20)
target_compile_options(cpp_ffvideo_streamer PRIVATE -Wall -Wextra -v)
set_target_properties(cpp_ffvideo_streamer PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

if (NOT DEFINED Boost_INCLUDE_DIRS OR "${Boost_INCLUDE_DIRS}" STREQUAL "")
    message(FATAL_ERROR "Boost_INCLUDE_DIRS was NOT set")
endif()

if (NOT DEFINED Poco_INCLUDE_DIRS OR "${Poco_INCLUDE_DIRS}" STREQUAL "")
    set(Poco_INCLUDE_DIRS "/usr/local/include")
    if (NOT EXISTS "${Poco_INCLUDE_DIRS}")
        message(FATAL_ERROR "Poco_INCLUDE_DIRS (${Poco_INCLUDE_DIRS}) does NOT exist")
    endif()
endif()

target_include_directories(
    cpp_ffvideo_streamer PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${Poco_INCLUDE_DIRS}
)

# libavcodec
set(avcodec "${CMAKE_CURRENT_SOURCE_DIR}/libraries/libavcodec/libavcodec.so")
if (NOT EXISTS "${avcodec}")
    message(FATAL_ERROR "Library '${avcodec}' was NOT found")
endif()

add_library(lib_av_codec SHARED IMPORTED)
set_property(
    TARGET lib_av_codec PROPERTY IMPORTED_LOCATION "${avcodec}"
)

# libavdevice
set(avdevice "${CMAKE_CURRENT_SOURCE_DIR}/libraries/libavdevice/libavdevice.so")
if (NOT EXISTS "${avdevice}")
    message(FATAL_ERROR "Library '${avdevice}' was NOT found")
endif()

add_library(lib_av_device SHARED IMPORTED)
set_property(
    TARGET lib_av_device PROPERTY IMPORTED_LOCATION "${avdevice}"
)

# libavfilter
set(avfilter "${CMAKE_CURRENT_SOURCE_DIR}/libraries/libavfilter/libavfilter.so")
if (NOT EXISTS "${avfilter}")
    message(FATAL_ERROR "Library '${avfilter}' was NOT found")
endif()

add_library(lib_av_filter SHARED IMPORTED)
set_property(
    TARGET lib_av_filter PROPERTY IMPORTED_LOCATION "${avfilter}"
)

# libavformat
set(avformat "${CMAKE_CURRENT_SOURCE_DIR}/libraries/libavformat/libavformat.so")
if (NOT EXISTS "${avformat}")
    message(FATAL_ERROR "Library '${avformat}' was NOT found")
endif()

add_library(lib_av_format SHARED IMPORTED)
set_property(
    TARGET lib_av_format PROPERTY IMPORTED_LOCATION "${avformat}"
)

# libavutil
set(avutil "${CMAKE_CURRENT_SOURCE_DIR}/libraries/libavutil/libavutil.so")
if (NOT EXISTS "${avutil}")
    message(FATAL_ERROR "Library '${avutil}' was NOT found")
endif()

add_library(lib_av_util SHARED IMPORTED)
set_property(
    TARGET lib_av_util PROPERTY IMPORTED_LOCATION "${avutil}"
)

# libpostproc
set(postproc "${CMAKE_CURRENT_SOURCE_DIR}/libraries/libpostproc/libpostproc.so")
if (NOT EXISTS "${postproc}")
    message(FATAL_ERROR "Library '${postproc}' was NOT found")
endif()

add_library(lib_post_proc SHARED IMPORTED)
set_property(
    TARGET lib_post_proc PROPERTY IMPORTED_LOCATION "${postproc}"
)

# libswresample
set(swresample "${CMAKE_CURRENT_SOURCE_DIR}/libraries/libswresample/libswresample.so")
if (NOT EXISTS "${swresample}")
    message(FATAL_ERROR "Library '${swresample}' was NOT found")
endif()

add_library(lib_sw_resample SHARED IMPORTED)
set_property(
    TARGET lib_sw_resample PROPERTY IMPORTED_LOCATION "${swresample}"
)

# libswscale
set(swscale "${CMAKE_CURRENT_SOURCE_DIR}/libraries/libswscale/libswscale.so")
if (NOT EXISTS "${swscale}")
    message(FATAL_ERROR "Library '${swscale}' was NOT found")
endif()

add_library(lib_sw_scale SHARED IMPORTED)
set_property(
    TARGET lib_sw_scale PROPERTY IMPORTED_LOCATION "${swscale}"
)

target_link_libraries(
    cpp_ffvideo_streamer PRIVATE
    lib_av_util lib_av_codec lib_av_format lib_av_filter lib_av_device
    lib_sw_scale lib_sw_resample lib_post_proc
    Boost::program_options
    Poco::Net
    Poco::Foundation
)

unset(avcodec)
unset(avdevice)
unset(avfilter)
unset(avformat)
unset(avutil)
unset(postproc)
unset(swresample)
unset(swscale)
