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

add_gtest(generic_commands ipmi
    get_chassis_status.cpp
    get_lan_config_params_test.cpp
    get_sdr_repository_info_test.cpp
    reserve_sdr_repository_test.cpp
    get_sdr_test.cpp
    get_fru_inventory_area_info.cpp
    read_fru_data.cpp
    test_runner.cpp
)

add_library(ipmi_generic_commands_test
    $<TARGET_OBJECTS:ipmi-base>
    $<TARGET_OBJECTS:ipmi-command-generic>
    $<TARGET_OBJECTS:ipmi-manager-ipmitool>
)

target_link_libraries(${test_target}
    ipmi_generic_commands_test
    ${LOGGER_LIBRARIES}
    ${IPMITOOL_LIBRARIES}
    ${GTEST_LIBRARIES}
)
