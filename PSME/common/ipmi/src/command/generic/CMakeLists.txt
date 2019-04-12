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
    get_device_id.cpp
    get_chassis_status.cpp
    set_system_boot_options.cpp
    get_system_boot_options.cpp
    chassis_control_command.cpp
    get_sensor_reading.cpp
    get_sensor_reading_factors.cpp
    get_system_guid.cpp
    get_device_guid.cpp
    master_write_read.cpp
    get_lan_config_params.cpp
    get_fru_inventory_area_info.cpp
    read_fru_data.cpp
    get_sdr_repository_info.cpp
    reserve_sdr_repository.cpp
    get_sdr.cpp
    warm_reset.cpp
    raw_request.cpp
    sel/get_sel_info.cpp
    sel/get_sel_entry.cpp
    sel/reserve_sel.cpp
    sel/delete_sel_entry.cpp
    sel/get_sel_time.cpp
)

set_source_files_properties(
    chassis_control_command.cpp
    COMPILE_FLAGS "-Wno-exit-time-destructors -Wno-global-constructors"
)

add_library(ipmi-command-generic OBJECT ${SOURCES})
