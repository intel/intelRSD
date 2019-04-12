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

set(SOURCES
    get_node_info.cpp
    get_bios_version.cpp
    get_dimm_info.cpp
    get_processor_info.cpp
    get_system_mac_address_haswell.cpp
    get_thermal_state.cpp
    set_iscsi_field.cpp
    get_slot_id.cpp
    get_fan_pwm.cpp
    get_cable_id.cpp
    get_mdr_data_region_status.cpp
    mdr_region_read.cpp

    iscsi_oob_boot/set_oob_boot_options.cpp
    iscsi_oob_boot/set_oob_reset_boot_options.cpp
    iscsi_oob_boot/set_oob_control_boot_options.cpp
    iscsi_oob_boot/set_oob_heap_boot_options.cpp
    iscsi_oob_boot/set_oob_initiator_boot_options.cpp
    iscsi_oob_boot/set_oob_nic_boot_options.cpp
    iscsi_oob_boot/set_oob_target_boot_options.cpp
    iscsi_oob_boot/get_oob_boot_options.cpp
    iscsi_oob_boot/get_oob_control_boot_options.cpp
    iscsi_oob_boot/get_oob_initiator_boot_options.cpp
    iscsi_oob_boot/get_oob_nic_boot_options.cpp
    iscsi_oob_boot/get_oob_target_boot_options.cpp
    iscsi_oob_boot/get_oob_heap_boot_options.cpp
    iscsi_oob_boot/errors.cpp

    rmm/get_presence.cpp
    rmm/get_authorized_cert_hash.cpp
    rmm/get_chassis_presence.cpp
    rmm/get_fan_presence.cpp
    rmm/get_psu_presence.cpp
    rmm/get_pzone_presence.cpp
    rmm/get_chassis_alert.cpp
    rmm/get_pwm.cpp
    rmm/get_fan_tachs.cpp
    rmm/get_id_field.cpp
    rmm/set_pwm.cpp
    rmm/set_authorized_cert.cpp
    rmm/get_aggregated_thermal_sensor.cpp
    rmm/get_aggregated_health_sensor.cpp
    rmm/get_drawer_power.cpp
    rmm/get_psshelf_critical.cpp
    rmm/send_psu_command.cpp
    rmm/reset.cpp
    rmm/set_id_field.cpp
    rmm/get_ps_input_power.cpp
    rmm/get_ps_dc_current.cpp

    nm/errors.cpp
    nm/update_reading_package.cpp
    nm/query_reading_package.cpp
    nm/get_telemetry_readings.cpp
    nm/get_cpu_and_memory_temperature.cpp
    nm/get_dcpmem_readings.cpp
    nm/trigger_dcpmem_proxy_access.cpp

    rsd/get_cpu_peci_package_config_data.cpp
    rsd/get_system_mode.cpp
    rsd/get_tpm_configuration.cpp
    rsd/read_pcie_cable_eeprom_data.cpp
    rsd/set_system_mode.cpp
    rsd/set_tpm_configuration.cpp
    rsd/mdr_region_read.cpp
    rsd/get_mdr_data_region_status.cpp
    rsd/mdr_region_update_complete.cpp
    rsd/mdr_region_lock.cpp
    rsd/mdr_region_write.cpp
    rsd/get_acpi_table_names.cpp
    rsd/set_acpi_table_names.cpp
    rsd/get_dcpmem_configuration.cpp
    rsd/set_dcpmem_configuration.cpp
    rsd/get_speed_select_configuration.cpp
    rsd/set_speed_select_configuration.cpp
    )

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_source_files_properties(rmm/get_aggregated_thermal_sensor.cpp
            PROPERTIES COMPILE_FLAGS "-Wno-unsafe-loop-optimizations"
            )
endif()

add_library(ipmi-command-sdv OBJECT ${SOURCES})
