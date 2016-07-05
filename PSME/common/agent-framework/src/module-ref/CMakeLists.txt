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
    constants/common.cpp
    constants/compute.cpp
    constants/network.cpp
    constants/storage.cpp
    constants/chassis.cpp
    constants/command.cpp

    model/attributes/array.cpp
    model/attributes/attributes.cpp
    model/attributes/collection.cpp
    model/attributes/cpu_id.cpp
    model/attributes/fru_info.cpp
    model/attributes/ipv4_address.cpp
    model/attributes/ipv6_address.cpp
    model/attributes/manager_entry.cpp
    model/attributes/neighbor_info.cpp
    model/attributes/network_service.cpp
    model/attributes/next_hop.cpp
    model/attributes/oem.cpp
    model/attributes/graphical_console.cpp
    model/attributes/serial_console.cpp
    model/attributes/command_shell.cpp
    model/attributes/status.cpp
    model/attributes/subcomponent_entry.cpp
    model/attributes/target_lun.cpp
    model/attributes/pci_device.cpp
    model/attributes/usb_device.cpp
    model/attributes/connection_data.cpp
    model/attributes/iscsi_data.cpp
    model/attributes/dimm_location.cpp
    model/attributes/interleave_set.cpp
    model/attributes/region.cpp
    model/attributes/acl_vlan_id.cpp
    model/attributes/acl_address.cpp
    model/attributes/acl_port.cpp

    model/authorization_certificate.cpp
    model/block_device.cpp
    model/chassis.cpp
    model/drive.cpp
    model/fan.cpp
    model/physical_drive.cpp
    model/iscsi_target.cpp
    model/logical_drive.cpp
    model/manager.cpp
    model/dimm.cpp
    model/memory_chunk.cpp
    model/network_interface.cpp
    model/power_zone.cpp
    model/processor.cpp
    model/psu.cpp
    model/switch.cpp
    model/remote_switch.cpp
    model/resource.cpp
    model/storage_controller.cpp
    model/storage_services.cpp
    model/switch_port.cpp
    model/system.cpp
    model/thermal_zone.cpp
    model/vlan.cpp
    model/port_vlan.cpp
    model/acl.cpp
    model/acl_rule.cpp
    model/static_mac.cpp

    loader/loader.cpp

    enum/processor_instruction_set.cpp
    enum/dimm_module_type.cpp

    managers/iscsi_target_manager.cpp

    managers/utils/manager_utils.cpp
    managers/many_to_many_manager.cpp

    compute_manager.cpp
    network_manager.cpp
    storage_manager.cpp
    chassis_manager.cpp

    utils/compute_hash.cpp
    utils/string_vector_to_json.cpp
    utils/json_to_string_vector.cpp
    utils/json_converter.cpp
    utils/optional/extensions.cpp
    utils/optional/optional-lib/optional.cpp

    requests/common/get_manager_collection.cpp
    requests/common/get_manager_info.cpp
    requests/common/get_collection.cpp
    requests/common/set_component_attributes.cpp
    requests/common/get_chassis_info.cpp

    requests/compute/get_system_info.cpp
    requests/compute/get_processor_info.cpp
    requests/compute/get_dimm_info.cpp
    requests/compute/get_memory_chunk_info.cpp
    requests/compute/get_storage_controller_info.cpp
    requests/compute/get_drive_info.cpp
    requests/compute/get_network_interface_info.cpp
    requests/compute/get_port_vlan_info.cpp
    requests/compute/delete_port_vlan.cpp

    requests/chassis/get_power_zone_info.cpp
    requests/chassis/get_psu_info.cpp
    requests/chassis/get_thermal_zone_info.cpp
    requests/chassis/get_fan_info.cpp
    requests/chassis/get_authorization_certificate.cpp

    requests/network/add_port_vlan.cpp
    requests/network/get_port_vlan_info.cpp
    requests/network/delete_port_vlan.cpp
    requests/network/get_vlan_info.cpp
    requests/network/delete_vlan.cpp
    requests/network/add_vlan.cpp
    requests/network/get_ethernet_switch_info.cpp
    requests/network/get_remote_ethernet_switch_info.cpp
    requests/network/get_ethernet_switch_port_info.cpp
    requests/network/add_ethernet_switch_port.cpp
    requests/network/delete_ethernet_switch_port.cpp
    requests/network/add_ethernet_switch_port_members.cpp
    requests/network/delete_ethernet_switch_port_members.cpp
    requests/network/add_acl.cpp
    requests/network/add_acl_rule.cpp
    requests/network/delete_acl.cpp
    requests/network/get_acl_info.cpp
    requests/network/add_acl_port.cpp
    requests/network/get_acl_rule_info.cpp
    requests/network/delete_acl_rule.cpp
    requests/network/delete_acl_port.cpp
    requests/network/add_port_static_mac.cpp
    requests/network/get_port_static_mac_info.cpp
    requests/network/delete_port_static_mac.cpp

    requests/storage/add_iscsi_target.cpp
    requests/storage/add_logical_drive.cpp
    requests/storage/delete_iscsi_target.cpp
    requests/storage/delete_logical_drive.cpp
    requests/storage/get_logical_drive_info.cpp
    requests/storage/get_physical_drive_info.cpp
    requests/storage/get_storage_services_info.cpp
    requests/storage/get_iscsi_target_info.cpp

    responses/common/set_component_attributes.cpp

    responses/network/add_port_vlan.cpp
    responses/network/add_ethernet_switch_port.cpp
    responses/network/delete_port_vlan.cpp
    responses/network/delete_ethernet_switch_port.cpp
    responses/network/add_ethernet_switch_port_members.cpp
    responses/network/delete_ethernet_switch_port_members.cpp
    responses/network/add_acl.cpp
    responses/network/add_acl_rule.cpp
    responses/network/delete_acl.cpp
    responses/network/add_acl_port.cpp
    responses/network/delete_acl_rule.cpp
    responses/network/delete_acl_port.cpp
    responses/network/add_port_static_mac.cpp
    responses/network/delete_port_static_mac.cpp

    responses/storage/add_iscsi_target.cpp
    responses/storage/add_logical_drive.cpp
    responses/storage/delete_iscsi_target.cpp
    responses/storage/delete_logical_drive.cpp
)

set_source_files_properties(
    managers/utils/manager_utils.cpp
    COMPILE_FLAGS "-Wno-exit-time-destructors -Wno-global-constructors"
)

set_source_files_properties(
    model/data_types/data_field.cpp
    model/system.cpp
)

add_library(module-ref OBJECT ${SOURCES})
