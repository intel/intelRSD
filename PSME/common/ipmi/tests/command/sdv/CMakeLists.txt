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

add_gtest(sdv_commands ipmi
    get_mdr_data_region_status_test.cpp
    get_pwm_test.cpp
    get_fan_tachs_test.cpp
    set_pwm_test.cpp
    get_psshelf_critical_test.cpp
    get_aggregated_thermal_sensor.cpp
    get_aggregated_health_sensor.cpp
    get_drawer_power.cpp
    send_psu_command_test.cpp
    get_presense_test.cpp
    get_authorized_certificate_hash_test.cpp
    set_authorized_certificate_test.cpp
    get_ps_input_power.cpp
    reset_test.cpp
    get_telemetry_readings_test.cpp
    get_cpu_and_memory_temperature_test.cpp
    rsd/mdr_region_read_test.cpp
    rsd/mdr_region_write_test.cpp
    rsd/mdr_region_update_complete_test.cpp
    rsd/get_mdr_data_region_status_test.cpp
    rsd/mdr_region_lock_test.cpp
    rsd/get_acpi_table_names_test.cpp
    rsd/set_acpi_table_names_test.cpp
    test_runner.cpp
)

add_library(ipmi_sdv_commands
    $<TARGET_OBJECTS:ipmi-base>
    $<TARGET_OBJECTS:ipmi-command-sdv>
    $<TARGET_OBJECTS:ipmi-manager-ipmitool>
)

target_link_libraries(${test_target}
    ipmi_sdv_commands
    ${LOGGER_LIBRARIES}
    ${IPMITOOL_LIBRARIES}
    ${GTEST_LIBRARIES}
)
