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

if (NOT GTEST_FOUND)
    return()
endif ()

add_gtest(loader_test psme-gpt-nvme
    loader_validation_test.cpp
    loader_config_test.cpp
    test_runner.cpp
)

generate_config_file(loader_validation_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/configuration_full.hpp
    "NVME_FULL_CONFIGURATION"
    "Declaration of full configuration"
)

target_sources(${test_target} PRIVATE
    ../../src/loader/loader.cpp
    ../../src/loader/config.cpp
)

target_link_libraries(${test_target}
    nvme partition
    nvme-target
    agent-framework
    uuid
    logger
    configuration
    ${SAFESTRING_LIBRARIES}
)

target_include_directories(${test_target}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${AGENT_FRAMEWORK_INCLUDE_DIRS}
)

