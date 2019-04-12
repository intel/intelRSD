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

if (NOT GTEST_FOUND)
    return()
endif()

add_gtest(loader_load_test psme-compute
    loader_config_validation_test.cpp
    test_runner.cpp
)

generate_config_file(loader_config_validation_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/configuration_full.hpp
    "COMPUTE_FULL_CONFIGURATION"
    "Declaration of full configuration"
)

add_library(compute-loader-test-libs
    $<TARGET_OBJECTS:compute-loader>
)

target_link_libraries(${test_target}
    ${AGENT_COMPUTE_LIBRARIES}
    compute-utils
    ${UUID_LIBRARIES}
    ${LOGGER_LIBRARIES}
    ${PCA95XX_LIBRARIES}
    configuration
    ${ISCSI_MDR_LIBRARIES}
    ipmi
    ${IPMITOOL_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
    pthread
    ${MICROHTTPD_LIBRARIES}
    ${CURL_LIBRARIES}
    md5
    netlink
    ${NL3_LIBRARIES}
)
