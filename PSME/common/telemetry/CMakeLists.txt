# <license_header>
#
# Copyright (c) 2017-2018 Intel Corporation
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
project(TELEMETRY CXX)

set(TELEMETRY_LIBRARIES
    telemetry
    ${AGENT_FRAMEWORK_LIBRARIES}
    CACHE PATH "Telemetry libraries"
)

set(TELEMETRY_INCLUDE_DIRS
    ${AGENT_FRAMEWORK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "Telemetry include directory"
)

include_directories(
    include
    ${AGENT_FRAMEWORK_INCLUDE_DIRS}
    ${JSON_INCLUDE_DIRS}
    ${JSON_RPC_INCLUDE_DIRS}
    ${JSONCXX_INCLUDE_DIRS}
    ${COMMON_INCLUDE_DIRS}
    ${LOGGER_INCLUDE_DIRS}
    ${IPMI_INCLUDE_DIRS}
    ${CONFIGURATION_INCLUDE_DIRS}
)

include_directories(SYSTEM
    ${SAFESTRING_INCLUDE_DIRS}
)

add_library(telemetry STATIC
    src/resource_key.cpp
    src/metric_definition_builder.cpp
    src/telemetry_reader.cpp

    src/cpu_dimm_temperature_telemetry_reader.cpp
    src/grantley_cpu_dimm_temperature_telemetry_reader.cpp
    src/purley_cpu_dimm_temperature_telemetry_reader.cpp
    src/bandwidth_telemetry_reader.cpp
    src/sensor_telemetry_reader.cpp
    src/health_telemetry_reader.cpp
    src/telemetry_hub_telemetry_reader.cpp
    src/memory_throttling_telemetry_reader.cpp

    src/metric_definitions.cpp
    src/metric_processor.cpp
    src/telemetry_service.cpp
)

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_source_files_properties(src/telemetry_service.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-unsafe-loop-optimizations"
    )
endif()


install(TARGETS telemetry
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

add_subdirectory(tests)
