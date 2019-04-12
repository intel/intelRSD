# <license_header>
#
# Copyright (c) 2018-2019 Intel Corporation
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
project("PSME SPDK" C CXX)

if (CMAKE_CROSSCOMPILING)
    message(WARNING "Not supported for crosscompiling.")
    return()
endif ()

add_executable(psme-spdk-nvme
    main.cpp
   )

generate_default_config_file(
    main.cpp
    agent
    spdk
    "AGENT_SPDK_CONFIGURATION_FILE"
    ./default_configuration.hpp
    .
)

target_link_libraries(psme-spdk-nvme
    -Wl,--whole-archive spdk-commands -Wl,--no-whole-archive
    spdk-discovery
    configuration
    common-include
)

add_subdirectory(common)
add_subdirectory(discovery)
add_subdirectory(tree_stability)
add_subdirectory(tests)
add_subdirectory(command)
add_subdirectory(loader)



