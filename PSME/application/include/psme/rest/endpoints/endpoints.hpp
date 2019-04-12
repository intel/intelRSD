/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * */

#pragma once



#include "redfish.hpp"
#include "root.hpp"
#include "metadata_root.hpp"
#include "metadata.hpp"
#include "update_service.hpp"
#include "simple_update_action_info.hpp"
#include "simple_update.hpp"
#include "event_service.hpp"
#include "session_service.hpp"
#include "psme/rest/endpoints/task_service/task_service.hpp"
#include "psme/rest/endpoints/task_service/task_collection.hpp"
#include "psme/rest/endpoints/task_service/task.hpp"
#include "message_registry_file_collection.hpp"
#include "message_registry_file.hpp"
#include "message_registry.hpp"
#include "event_subscriptions_collection.hpp"
#include "event_subscription.hpp"
#include "session_collection.hpp"
#include "session.hpp"
#include "psme/rest/endpoints/manager/manager.hpp"
#include "psme/rest/endpoints/manager/manager_collection.hpp"
#include "psme/rest/endpoints/manager/manager_reset.hpp"
#include "psme/rest/endpoints/manager/manager_load_factory_defaults.hpp"
#include "psme/rest/endpoints/manager/manager_network_interface.hpp"
#include "psme/rest/endpoints/manager/manager_network_interface_collection.hpp"
#include "psme/rest/endpoints/manager/manager_vlan_network_interface_collection.hpp"
#include "psme/rest/endpoints/manager/manager_vlan_network_interface.hpp"
#include "psme/rest/endpoints/task_service/monitor.hpp"
#include "psme/rest/endpoints/manager/network_protocol.hpp"
#include "odata_service_document.hpp"
#include "intel_registry.hpp"

#include "system/system.hpp"
#include "system/systems_collection.hpp"
#include "system/system_reset.hpp"
#include "system/system_change_tpm_state.hpp"
#include "system/memory.hpp"
#include "system/memory_collection.hpp"
#include "system/memory_metrics.hpp"
#include "system/system_dcpmem_secure_erase.hpp"
#include "system/system_network_interface.hpp"
#include "system/system_network_interface_collection.hpp"
#include "system/processor.hpp"
#include "system/processors_collection.hpp"
#include "system/processor_metrics.hpp"
#include "system/processor_secure_erase.hpp"
#include "system/storage_subsystem.hpp"
#include "system/storage_subsystems_collection.hpp"
#include "system/volumes_collection.hpp"
#include "system/system_network_interface.hpp"
#include "system/system_network_interface_collection.hpp"
#include "system/system_metrics.hpp"
#include "system/hosted_storage_services.hpp"
#include "system/network_interface.hpp"
#include "system/network_interfaces_collection.hpp"
#include "system/network_device_function.hpp"
#include "system/network_device_functions_collection.hpp"

#include "storage/allocated_pools_collection.hpp"
#include "storage/drive_collection.hpp"
#include "storage/storage_service.hpp"
#include "storage/storage_services_collection.hpp"
#include "storage/storage_pool.hpp"
#include "storage/storage_pool_collection.hpp"
#include "storage/volume.hpp"
#include "storage/volume_metrics.hpp"
#include "storage/volume_initialize.hpp"
#include "storage/volume_collection.hpp"
#include "storage/allocated_volumes_collection.hpp"
#include "storage/capacity_source.hpp"
#include "storage/providing_drives_collection.hpp"
#include "storage/providing_pools_collection.hpp"
#include "storage/providing_volumes_collection.hpp"

#include "ethernet/ethernet_switch.hpp"
#include "ethernet/ethernet_switch_collection.hpp"
#include "ethernet/ethernet_switch_metrics.hpp"
#include "ethernet/ethernet_switch_port.hpp"
#include "ethernet/ethernet_switch_port_collection.hpp"
#include "ethernet/ethernet_switch_port_metrics.hpp"
#include "ethernet/vlan_network_interface.hpp"
#include "ethernet/vlan_network_interface_collection.hpp"
#include "ethernet/acl_collection.hpp"
#include "ethernet/acl.hpp"
#include "ethernet/acl_bind.hpp"
#include "ethernet/rule_collection.hpp"
#include "ethernet/rule.hpp"
#include "ethernet/static_mac_collection.hpp"
#include "ethernet/static_mac.hpp"

#include "chassis/chassis.hpp"
#include "chassis/chassis_collection.hpp"
#include "chassis/chassis_reset.hpp"
#include "chassis/thermal.hpp"
#include "chassis/power.hpp"
#include "chassis/power_supply_request_state_change.hpp"
#include "chassis/drive.hpp"
#include "chassis/drive_metrics.hpp"
#include "chassis/drives_collection.hpp"
#include "chassis/drive_secure_erase.hpp"
#include "chassis/pcie_device.hpp"
#include "chassis/pcie_devices_collection.hpp"
#include "chassis/pcie_function.hpp"
#include "chassis/pcie_functions_collection.hpp"

#include "fabric/fabric.hpp"
#include "fabric/fabrics_collection.hpp"
#include "fabric/endpoint.hpp"
#include "fabric/endpoints_collection.hpp"
#include "fabric/switch.hpp"
#include "fabric/switches_collection.hpp"
#include "fabric/switch_reset.hpp"
#include "fabric/port.hpp"
#include "fabric/port_metrics.hpp"
#include "fabric/port_reset.hpp"
#include "fabric/ports_collection.hpp"
#include "fabric/zone.hpp"
#include "fabric/zones_collection.hpp"

#include "telemetry/metric_definition.hpp"
#include "telemetry/metric_definitions_collection.hpp"
#include "telemetry/metric_report_definition.hpp"
#include "telemetry/metric_report_definitions_collection.hpp"
#include "telemetry/telemetry_service.hpp"
#include "telemetry/trigger.hpp"
#include "telemetry/triggers_collection.hpp"

#include "account_service/account_service.hpp"
#include "account_service/account_collection.hpp"
#include "account_service/account.hpp"
#include "account_service/role.hpp"
#include "account_service/role_collection.hpp"