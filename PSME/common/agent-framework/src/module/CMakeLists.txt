# <license_header>
#
# Copyright (c) 2015-2017 Intel Corporation
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
    constants/pnc.cpp
    constants/rmm.cpp
    constants/psme.cpp
    constants/command.cpp
    constants/regular_expressions.cpp

    model/attributes/array.cpp
    model/attributes/attributes.cpp
    model/attributes/result_status.cpp
    model/attributes/collection.cpp
    model/attributes/cpu_id.cpp
    model/attributes/extended_cpu_id.cpp
    model/attributes/fru_info.cpp
    model/attributes/identifier.cpp
    model/attributes/ipv4_address.cpp
    model/attributes/ipv6_address.cpp
    model/attributes/memory_location.cpp
    model/attributes/location.cpp
    model/attributes/manager_entry.cpp
    model/attributes/message.cpp
    model/attributes/neighbor_info.cpp
    model/attributes/network_service.cpp
    model/attributes/next_hop.cpp
    model/attributes/oem.cpp
    model/attributes/pci_device.cpp
    model/attributes/graphical_console.cpp
    model/attributes/serial_console.cpp
    model/attributes/command_shell.cpp
    model/attributes/status.cpp
    model/attributes/subcomponent_entry.cpp
    model/attributes/target_lun.cpp
    model/attributes/usb_device.cpp
    model/attributes/connection_data.cpp
    model/attributes/iscsi_boot.cpp
    model/attributes/iscsi_data.cpp
    model/attributes/memory_location.cpp
    model/attributes/region.cpp
    model/attributes/acl_vlan_id.cpp
    model/attributes/acl_address.cpp
    model/attributes/acl_port.cpp
    model/attributes/task_entry.cpp
    model/attributes/connected_entity.cpp
    model/attributes/wildcard.cpp
    model/attributes/calculation_parameters.cpp
    model/attributes/metric_definition_entry.cpp
    model/attributes/on_package_memory.cpp
    model/attributes/fpga.cpp

    model/acl.cpp
    model/acl_rule.cpp
    model/authorization_certificate.cpp
    model/block_device.cpp
    model/chassis.cpp
    model/chassis_sensor.cpp
    model/drive.cpp
    model/endpoint.cpp
    model/ethernet_switch_port.cpp
    model/ethernet_switch_port_vlan.cpp
    model/ethernet_switch.cpp
    model/fabric.cpp
    model/fan.cpp
    model/physical_drive.cpp
    model/iscsi_target.cpp
    model/logical_drive.cpp
    model/manager.cpp
    model/memory.cpp
    model/metric.cpp
    model/metric_definition.cpp
    model/network_interface.cpp
    model/network_device_function.cpp
    model/network_device.cpp
    model/pcie_device.cpp
    model/pcie_function.cpp
    model/port.cpp
    model/power_zone.cpp
    model/processor.cpp
    model/psu.cpp
    model/remote_ethernet_switch.cpp
    model/resource.cpp
    model/static_mac.cpp
    model/storage_controller.cpp
    model/storage_subsystem.cpp
    model/storage_service.cpp
    model/switch.cpp
    model/system.cpp
    model/task.cpp
    model/thermal_zone.cpp
    model/trusted_module.cpp
    model/vlan.cpp
    model/zone.cpp


    loader/loader.cpp

    enum/processor_instruction_set.cpp
    enum/sensor_type.cpp
    enum/hssi_configuration.cpp
    enum/hssi_sideband.cpp

    managers/iscsi_target_manager.cpp

    managers/utils/manager_utils.cpp
    managers/many_to_many_manager.cpp
    managers/generic_manager_registry.cpp
    managers/table_interface.cpp

    compute_components.cpp
    network_components.cpp
    storage_components.cpp
    chassis_components.cpp
    pnc_components.cpp
    common_components.cpp

    utils/compute_hash.cpp
    utils/json_converter.cpp
    utils/json_transformations.cpp
    utils/optional/extensions.cpp
    utils/optional/optional-lib/optional.cpp
    utils/time.cpp
    utils/is_requested_metric.cpp
    utils/is_ascii_string.cpp

    requests/validation/json_check_type.cpp
    requests/validation/network.cpp
    requests/validation/compute.cpp
    requests/validation/storage.cpp
    requests/validation/common.cpp
    requests/validation/pnc.cpp
    requests/validation/rmm.cpp

    requests/common/get_managers_collection.cpp
    requests/common/get_manager_info.cpp
    requests/common/get_collection.cpp
    requests/common/set_component_attributes.cpp
    requests/common/get_chassis_info.cpp
    requests/common/get_drive_info.cpp
    requests/common/get_tasks_collection.cpp
    requests/common/get_task_info.cpp
    requests/common/delete_task.cpp
    requests/common/get_task_result_info.cpp
    requests/common/get_system_info.cpp
    requests/common/get_storage_subsystem_info.cpp
    requests/common/get_metrics.cpp
    requests/common/get_metric_definitions_collection.cpp
    requests/common/get_metric_definition_info.cpp
    requests/common/get_processor_info.cpp

    requests/compute/get_memory_info.cpp
    requests/compute/get_storage_controller_info.cpp
    requests/compute/get_network_interface_info.cpp
    requests/compute/get_network_device_info.cpp
    requests/compute/get_network_device_function_info.cpp
    requests/compute/get_trusted_module_info.cpp

    requests/chassis/get_authorization_certificate.cpp

    requests/rmm/get_power_zone_info.cpp
    requests/rmm/get_psu_info.cpp
    requests/rmm/get_thermal_zone_info.cpp
    requests/rmm/get_fan_info.cpp

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
    requests/storage/get_storage_service_info.cpp
    requests/storage/get_iscsi_target_info.cpp

    requests/pnc/get_switch_info.cpp
    requests/pnc/get_fabric_info.cpp
    requests/pnc/get_zone_info.cpp
    requests/pnc/get_pcie_device_info.cpp
    requests/pnc/get_pcie_function_info.cpp
    requests/pnc/get_endpoint_info.cpp
    requests/pnc/add_zone.cpp
    requests/pnc/get_port_info.cpp
    requests/pnc/delete_zone.cpp
    requests/pnc/add_zone_endpoint.cpp
    requests/pnc/delete_zone_endpoint.cpp

    requests/psme/attach.cpp
    requests/psme/component_notification.cpp
    requests/psme/heart_beat.cpp

    responses/common/set_component_attributes.cpp
    responses/common/delete_task.cpp
    responses/common/get_task_result_info.cpp

    responses/network/add_port_vlan.cpp
    responses/network/add_ethernet_switch_port.cpp
    responses/network/delete_port_vlan.cpp
    responses/network/delete_ethernet_switch_port.cpp
    responses/network/add_ethernet_switch_port_members.cpp
    responses/network/delete_ethernet_switch_port_members.cpp
    responses/network/add_acl.cpp
    responses/network/add_acl_rule.cpp
    responses/network/add_vlan.cpp
    responses/network/delete_acl.cpp
    responses/network/add_acl_port.cpp
    responses/network/delete_acl_rule.cpp
    responses/network/delete_acl_port.cpp
    responses/network/add_port_static_mac.cpp
    responses/network/delete_port_static_mac.cpp
    responses/network/delete_vlan.cpp

    responses/storage/add_iscsi_target.cpp
    responses/storage/add_logical_drive.cpp
    responses/storage/delete_iscsi_target.cpp
    responses/storage/delete_logical_drive.cpp

    responses/pnc/add_zone.cpp
    responses/pnc/delete_zone.cpp
    responses/pnc/add_zone_endpoint.cpp
    responses/pnc/delete_zone_endpoint.cpp

    responses/psme/attach.cpp
    responses/psme/heart_beat.cpp
)

set_source_files_properties(
    managers/utils/manager_utils.cpp
    COMPILE_FLAGS "-Wno-exit-time-destructors -Wno-global-constructors"
)

set_source_files_properties(
    model/data_types/data_field.cpp
    model/system.cpp
)

add_library(module OBJECT ${SOURCES})
