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
)


add_library(ipmi-command-sdv OBJECT ${SOURCES})

