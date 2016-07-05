# <license_header>
#
# Copyright (c) 2015-2016 Intel Corporation
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
project("PSME FW Build" C CXX)

###############################################################################

string(TIMESTAMP BUILD_TIME "%H:%M:%S, %d-%m-%Y")

# Target folder defaults
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Path to folder with all cmake scripts
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Setup compilers
include(AddGnuCompiler)
include(AddClangCompiler)

# Add code coverage
include(AddCodeCoverage)

# Add documentation
include(AddDocumentation)

# Add cpack to setup output product
include(AddCPack)

# Add valgrind diagnostic options
include(AddDiagnostics)

# Add function which skips global constructor warnings
include(PsmeAgentFramework)

# Add common headers
include(CopyCommonInclude)

option(ENABLE_HTTPS "Enable HTTPS support" ON)

if (ENABLE_HTTPS)
    find_package(Libgcrypt REQUIRED)
    find_package(GnuTLS REQUIRED)
endif()

# Add external packages which are stored in third_party or so
# TODO - get rid of as many of them as possible
include(InstallGoogleTest)
include(InstallGoogleMock)
include(InstallMd5)
include(InstallSafeString)
include(InstallCurl)
include(InstallIES)
include(InstallJsonCpp)
include(InstallMicrohttpd)
include(InstallJsonRpcCpp)
include(InstallUUID)
include(InstallNcurses)
include(InstallIpmitool)
include(InstallSysfs)
include(InstallLvm2App)
include(InstallNL3)

# Find packages provided by us or by system
# TODO - REQUIRED / OPTIONAL
find_package(PkgConfig)
find_package(GoogleTest)
find_package(Md5)
find_package(SafeString)
find_package(Ipmitool)
find_package(Lvm2App)
find_package(Curl)
find_package(Glib)
find_package(Glibmm)
find_package(IES)
find_package(Ncurses)
find_package(JsonCpp)
find_package(JsonRpcCpp)
find_package(Libxmlpp)
find_package(Microhttpd)
find_package(NL3)
find_package(Sysfs)
find_package(UDEV)
find_package(UUID)
find_package(LLDP)
find_package(Fm10kd)

# Enable google testing
enable_testing()

###############################################################################

add_subdirectory(common)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/agent)
	add_subdirectory(agent)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/agent-intel)
	add_subdirectory(agent-intel)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/agent-simulator)
	add_subdirectory(agent-simulator)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/agent-stubs)
	add_subdirectory(agent-stubs)
endif()

add_subdirectory(application-ref)
add_subdirectory(encrypter)
