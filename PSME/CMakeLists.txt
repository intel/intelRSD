# <license_header>
#
# Copyright (c) 2015-2019 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# </license_header>

cmake_minimum_required(VERSION 2.8)
message(STATUS "Current cmake is ${CMAKE_VERSION}")

project("PSME FW Build" C CXX)

###############################################################################

string(TIMESTAMP BUILD_TIME "%H:%M:%S, %d-%m-%Y")

# Target folder defaults
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


set(CMAKE_INCLUDE_PATH "${CMAKE_BINARY_DIR}/include;${CMAKE_SYSTEM_INCLUDE_PATH}")
set(CMAKE_LIBRARY_PATH "${CMAKE_BINARY_DIR}/lib;${CMAKE_SYSTEM_LIBRARY_PATH}")
set(CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR};${CMAKE_SYSTEM_PREFIX_PATH}")

if ("${CMAKE_TARGET_ARCH}" STREQUAL "32")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        set(CMAKE_SHARED_LINKER_FLAGS "-L${CMAKE_BINARY_DIR}/lib")
        set(BUILD_GNUTLS YES)
        set(BUILD_MICROHTTPD YES)
        set(BUILD_CURL YES)
        set(BUILD_EOS_SDK YES)
        set(BUILD_EOS_EAPI YES)
endif()

if (NOT (CMAKE_CROSSCOMPILING OR "${CMAKE_TARGET_ARCH}" STREQUAL "32"))
    set(BUILD_FABRIC YES)
    set(BUILD_OPAE YES)
    set(BUILD_AARDVARK YES)
endif()

# Path to folder with all cmake scripts
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# add pkg-config directory to search path
set(CMAKE_PKG_CONFIG_DIR ${CMAKE_BINARY_DIR}/pkg-config)
file(MAKE_DIRECTORY "${CMAKE_PKG_CONFIG_DIR}")
set(ENV{PKG_CONFIG_PATH} "${CMAKE_PKG_CONFIG_DIR}" "${CMAKE_MODULE_PATH}")

# Setup compilers
include(AddGnuCompiler)
include(AddClangCompiler)

# Add code coverage
include(AddCodeCoverage)

# Add documentation
include(AddDocumentation)

# Add function which skips global constructor warnings
include(PsmeAgentFramework)
include(InstallScriptsCommon)

# Add function to generate header files with default or test configuration
include(GenerateConfigHeader)

option(ENABLE_HTTPS "Enable HTTPS support" ON)

find_package(PkgConfig REQUIRED)
find_package(PackageHandleStandardArgs REQUIRED)


# TODO Issues to be done, order not significant..
#
# TODO parsing args in find_or_install/install_third_party (URL/REQUIRED)
# TODO cleanup with "local" pkgconfig -> store all files in lib/pkgconfig
# TODO GoogleTest/Mock with silent handling
# TODO Check what about google_tests (last "version" on github..)
# TODO ac_cv_va_copy env should not be used (is there any option to replace it?)
# TODO add ABI tags checker for all find scripts (link small file with the lib.. if fails library is not found)
# TODO add warning to all "non-999" .so generating packages created by Install scripts

###############################################################################
# Find packages provided by us or by system

# will not link if not ENABLE_HTTPS: some symbols are referenced directly
if (ENABLE_HTTPS)
    if (BUILD_GNUTLS)
        install_third_party(Gmp)
        install_third_party(Nettle)
        install_third_party(GnuTLS)
    else()
        find_package(GnuTLS REQUIRED)
    endif()
endif()

if (BUILD_CURL)
    install_third_party(Curl)
else()
    find_package(Curl REQUIRED)
endif()

if (BUILD_MICROHTTPD)
    install_third_party(GPG-error)
    install_third_party(GCrypt)
    install_third_party(Microhttpd)
else()
    find_package(Microhttpd REQUIRED)
endif()

install_third_party(NlohmannJson)

find_package(UDEV REQUIRED)
install_third_party(Ipmitool)

find_package(Lvm2App REQUIRED)
find_package(NL3 REQUIRED)
find_package(DevMapper REQUIRED)

install_third_party(SafeString)

if (BUILD_OPAE)
    install_third_party(OPAE11)
    install_third_party(OPAE)
endif()

if (BUILD_FABRIC)
    install_third_party(Fabric)
endif()

if (BUILD_AARDVARK)
    install_third_party(Aardvark)
endif()

if (NOT CMAKE_CROSSCOMPILING)
    # Enable google testing
    install_third_party(GoogleTest)
    install_third_party(GoogleMock)
    enable_testing()
endif()

if (BUILD_EOS_SDK)
    install_third_party(EosSDK)
endif()

# Enable sanitizers
if (CMAKE_BUILD_TYPE STREQUAL "asanitize")
    find_package(Asan)
    if (NOT ASAN_FOUND)
        message(FATAL_ERROR "ASAN library not found, Address/Memory SANITIZE version will not be compiled.")
        return()
    endif()
endif()

if (CMAKE_BUILD_TYPE STREQUAL "tsanitize")
    find_package(Tsan)
    if (NOT TSAN_FOUND)
        message(FATAL_ERROR "TSAN library not found, Thread SANITIZE version will not be compiled.")
        return()
    endif()
endif()

find_package(Zlib)


###############################################################################
message(STATUS "Building sources...")
add_definitions($ENV{OPTIONS})

add_subdirectory(common)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/agent)
	add_subdirectory(agent)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/agent-stubs)
	add_subdirectory(agent-stubs)
endif()

add_subdirectory(application)
add_subdirectory(encrypter)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/devtools)
    add_subdirectory(devtools)
endif()

if (GTEST_FOUND)
    get_property(ut_targets GLOBAL PROPERTY unittest_all_targets)
    list(REMOVE_DUPLICATES ut_targets)
    file(WRITE ${CMAKE_BINARY_DIR}/unittest_targets.txt " ")
    foreach(ut_target ${ut_targets})
        file(APPEND ${CMAKE_BINARY_DIR}/unittest_targets.txt " ${ut_target}")
    endforeach()
endif()
