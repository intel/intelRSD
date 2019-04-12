# <license_header>
#
# Copyright (c) 2017-2019 Intel Corporation
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

cmake_minimum_required(VERSION 3.4)
project(TELEMETRY CXX)

# TODO: remove old-style flags when all usages are removed
set(TELEMETRY_LIBRARIES
    telemetry
    agent-framework
    CACHE PATH "Telemetry libraries"
)

set(TELEMETRY_INCLUDE_DIRS
    ${AGENT_FRAMEWORK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "Telemetry include directory"
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
    src/dcpmem_telemetry_reader.cpp

    src/metric_definitions.cpp
    src/metric_processor.cpp
    src/telemetry_service.cpp
)

target_include_directories(telemetry
    SYSTEM PRIVATE
    ${SAFESTRING_INCLUDE_DIRS}

    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${AGENT_FRAMEWORK_INCLUDE_DIRS}
    ${JSON_INCLUDE_DIRS}
    ${JSON_RPC_INCLUDE_DIRS}
    ${IPMI_INCLUDE_DIRS}
    ${IPMITOOL_INCLUDE_DIRS}
    ${CONFIGURATION_INCLUDE_DIRS}
    ${PMEM_INCLUDE_DIRS}

)

target_link_libraries(telemetry
    PUBLIC
    agent-framework
    logger
    common-include
    uuid
    ${IPMI_LIBRARIES}
    ${IPMITOOL_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
    configuration
    pmem
)

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_source_files_properties(src/telemetry_service.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-unsafe-loop-optimizations"
    )
endif ()


add_subdirectory(tests)
