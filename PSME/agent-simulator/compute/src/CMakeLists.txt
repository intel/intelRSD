# <license_header>
#
# Copyright (c) 2015-2017 Intel Corporation
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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SOURCES
    loader/compute_loader.cpp
    main.cpp
)

include_directories(
    ${PSME_COMPUTE_SIMULATOR_DIR}/include
    ${LIBXMLPP_INCLUDE_DIRS}
)

add_subdirectory(discovery)
add_subdirectory(asset_configuration)
add_subdirectory(command)

add_library(compute-commands-simulator STATIC
    $<TARGET_OBJECTS:compute-command-simulator>
)

add_library(asset_configuration
    $<TARGET_OBJECTS:asset_configuration_impl>
)

add_library(common-discovery
    $<TARGET_OBJECTS:discovery-impl>
)

add_executable(psme-compute-simulator
    loader/compute_loader.cpp
    main.cpp
)

target_link_libraries(psme-compute-simulator
    ${PSME_COMPUTE_SIMULATOR_LIBRARIES}
    common-discovery
    asset_configuration
    ${AGENT_FRAMEWORK_LIBRARIES}
    ${UUID_LIBRARIES}
    ${LOGGER_LIBRARIES}
    ${PCA95XX_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
    ${CONFIGURATION_LIBRARIES}
    ${JSONCXX_LIBRARIES}
    ${LIBXMLPP_LIBRARIES}
    ${GLIBMM_LIBRARY}
    ${GLIB_LIBRARY}
    pthread
    ${MICROHTTPD_LIBRARIES}
    ${CURL_LIBRARIES}
    md5
)
