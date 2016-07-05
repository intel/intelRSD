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

if (NOT GTEST_FOUND)
    return()
endif()

add_gtest(module agent-framework
    test_runner.cpp
    module_test.cpp
    module_manager_test.cpp
    collection_test.cpp
    port_test.cpp
    chassis_test.cpp
    hard_drive_partition_test.cpp
    hard_drive_test.cpp
    logical_drive_test.cpp
    vlan_test.cpp
)

target_link_libraries(${test_target}
    ${LOGGER_LIBRARIES}
    ${UUID_LIBRARIES}
    ${AGENT_FRAMEWORK_LIB}
    ${SAFESTRING_LIBRARIES}
    agent-mocks
    ${CONFIGURATION_LIBRARIES}
    ${JSONCXX_LIBRARIES}
    ${JSONCPP_LIBRARIES}
    ${PCA95XX_LIBRARIES}
)
