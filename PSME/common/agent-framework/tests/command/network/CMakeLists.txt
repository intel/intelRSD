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

add_gtest(network_command agent-framework
    test_runner.cpp
    get_switch_info_test.cpp
    get_switch_port_info_test.cpp
    get_port_vlan_info_test.cpp
    add_port_vlan_test.cpp
    delete_port_vlan_test.cpp
    get_remote_switch_info_test.cpp
    get_manager_info_test.cpp
)

target_link_libraries(${test_target}
    ${UUID_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
    configuration
    ${PCA95XX_LIBRARIES}
)
