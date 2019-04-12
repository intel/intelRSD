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
 *
 * @file manager_utils.cpp
 * @brief Utilities used by managers.
 * */

#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/network_components.hpp"
#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/module/chassis_components.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/module/common_components.hpp"



namespace agent_framework {
namespace module {

// template specializations for common components

template<>
GenericManager<model::Manager>&
get_manager<model::Manager>() { return CommonComponents::get_instance()->get_module_manager(); }


template<>
GenericManager<model::Task>& get_manager<model::Task>() { return CommonComponents::get_instance()->get_task_manager(); }


template<>
GenericManager<model::Metric>&
get_manager<model::Metric>() { return CommonComponents::get_instance()->get_metric_manager(); }


template<>
GenericManager<model::MetricDefinition>&
get_manager<model::MetricDefinition>() { return CommonComponents::get_instance()->get_metric_definition_manager(); }


template<>
GenericManager<model::Chassis>&
get_manager<model::Chassis>() { return CommonComponents::get_instance()->get_chassis_manager(); }


template<>
GenericManager<model::Drive>&
get_manager<model::Drive>() { return CommonComponents::get_instance()->get_drive_manager(); }


template<>
GenericManager<model::System>&
get_manager<model::System>() { return CommonComponents::get_instance()->get_system_manager(); }


template<>
GenericManager<model::StorageSubsystem>&
get_manager<model::StorageSubsystem>() { return CommonComponents::get_instance()->get_storage_subsystem_manager(); }


template<>
GenericManager<model::NetworkInterface>&
get_manager<model::NetworkInterface>() { return CommonComponents::get_instance()->get_network_interface_manager(); }


template<>
GenericManager<model::Fabric>&
get_manager<model::Fabric>() { return CommonComponents::get_instance()->get_fabric_manager(); }


template<>
GenericManager<model::Zone>& get_manager<model::Zone>() { return CommonComponents::get_instance()->get_zone_manager(); }


template<>
GenericManager<model::Endpoint>&
get_manager<model::Endpoint>() { return CommonComponents::get_instance()->get_endpoint_manager(); }


template<>
GenericManager<model::PcieDevice>& get_manager<model::PcieDevice>() {
    return CommonComponents::get_instance()->get_pcie_device_manager();
}


template<>
GenericManager<model::PcieFunction>& get_manager<model::PcieFunction>() {
    return CommonComponents::get_instance()->get_pcie_function_manager();
}

// template specializations for compute components

template<>
GenericManager<model::Processor>&
get_manager<model::Processor>() { return CommonComponents::get_instance()->get_processor_manager(); }


template<>
GenericManager<model::Memory>&
get_manager<model::Memory>() { return ComputeComponents::get_instance()->get_memory_manager(); }


template<>
GenericManager<model::MemoryDomain>&
get_manager<model::MemoryDomain>() { return ComputeComponents::get_instance()->get_memory_domain_manager(); }

template<>
GenericManager<model::MemoryChunks>&
get_manager<model::MemoryChunks>() { return ComputeComponents::get_instance()->get_memory_chunks_manager(); }

template<>
GenericManager<model::StorageController>&
get_manager<model::StorageController>() { return ComputeComponents::get_instance()->get_storage_controller_manager(); }


template<>
GenericManager<model::NetworkDevice>&
get_manager<model::NetworkDevice>() { return ComputeComponents::get_instance()->get_network_device_manager(); }


template<>
GenericManager<model::NetworkDeviceFunction>&
get_manager<model::NetworkDeviceFunction>() { return ComputeComponents::get_instance()->get_network_device_function_manager(); }


template<>
GenericManager<model::TrustedModule>&
get_manager<model::TrustedModule>() { return ComputeComponents::get_instance()->get_trusted_module_manager(); }

// template specializations for network components

template<>
GenericManager<model::EthernetSwitch>&
get_manager<model::EthernetSwitch>() { return NetworkComponents::get_instance()->get_switch_manager(); }


template<>
GenericManager<model::RemoteEthernetSwitch>&
get_manager<model::RemoteEthernetSwitch>() { return NetworkComponents::get_instance()->get_remote_switch_manager(); }


template<>
GenericManager<model::EthernetSwitchPort>&
get_manager<model::EthernetSwitchPort>() { return NetworkComponents::get_instance()->get_port_manager(); }


template<>
GenericManager<model::Vlan>&
get_manager<model::Vlan>() { return NetworkComponents::get_instance()->get_vlan_manager(); }


template<>
GenericManager<model::EthernetSwitchPortVlan>&
get_manager<model::EthernetSwitchPortVlan>() { return NetworkComponents::get_instance()->get_port_vlan_manager(); }


template<>
GenericManager<model::Acl>& get_manager<model::Acl>() { return NetworkComponents::get_instance()->get_acl_manager(); }


template<>
GenericManager<model::AclRule>&
get_manager<model::AclRule>() { return NetworkComponents::get_instance()->get_acl_rule_manager(); }


template<>
GenericManager<model::StaticMac>&
get_manager<model::StaticMac>() { return NetworkComponents::get_instance()->get_static_mac_manager(); }

// template specializations for storage components

template<>
GenericManager<model::StorageService>&
get_manager<model::StorageService>() { return StorageComponents::get_instance()->get_storage_service_manager(); }


template<>
GenericManager<model::Volume>&
get_manager<model::Volume>() { return StorageComponents::get_instance()->get_volume_manager(); }


template<>
GenericManager<model::StoragePool>&
get_manager<model::StoragePool>() { return StorageComponents::get_instance()->get_storage_pool_manager(); }


// template specializations for PNC components

template<>
GenericManager<model::Switch>&
get_manager<model::Switch>() { return PncComponents::get_instance()->get_switch_manager(); }


template<>
GenericManager<model::Port>& get_manager<model::Port>() { return PncComponents::get_instance()->get_port_manager(); }


// template specializations for chassis/rmm components

template<>
GenericManager<model::PowerZone>&
get_manager<model::PowerZone>() { return ChassisComponents::get_instance()->get_power_zone_manager(); }


template<>
GenericManager<model::ThermalZone>&
get_manager<model::ThermalZone>() { return ChassisComponents::get_instance()->get_thermal_zone_manager(); }


template<>
GenericManager<model::Fan>& get_manager<model::Fan>() { return ChassisComponents::get_instance()->get_fan_manager(); }


template<>
GenericManager<model::Psu>& get_manager<model::Psu>() { return ChassisComponents::get_instance()->get_psu_manager(); }


template<>
GenericManager<model::ChassisSensor>&
get_manager<model::ChassisSensor>() { return ChassisComponents::get_instance()->get_chassis_sensor_manager(); }

// template specializations for ManyToMany managers

template<>
managers::ManyToManyManager&
get_m2m_manager<model::Zone, model::Endpoint>() { return CommonComponents::get_instance()->get_zone_endpoint_manager(); }


template<>
managers::ManyToManyManager&
get_m2m_manager<model::Endpoint, model::Port>() { return PncComponents::get_instance()->get_endpoint_port_manager(); }


template<>
managers::ManyToManyManager&
get_m2m_manager<model::Drive, model::PcieFunction>() { return PncComponents::get_instance()->get_drive_function_manager(); }

template<>
managers::ManyToManyManager&
get_m2m_manager<model::Processor, model::PcieFunction>() { return PncComponents::get_instance()->get_processor_function_manager(); }


template<>
managers::ManyToManyManager&
get_m2m_manager<model::StorageSubsystem, model::Drive>() { return CommonComponents::get_instance()->get_storage_subsystem_drives_manager(); }


template<>
managers::ManyToManyManager&
get_m2m_manager<model::StoragePool, model::Volume>() { return CommonComponents::get_instance()->get_storage_pool_volumes_manager(); }


template<>
managers::ManyToManyManager&
get_m2m_manager<model::StorageService, model::Drive>() { return CommonComponents::get_instance()->get_storage_service_drives_manager(); }


template<>
managers::ManyToManyManager&
get_m2m_manager<model::System, model::StorageService>() { return StorageComponents::get_instance()->get_system_storage_services_manager(); }

}
}
