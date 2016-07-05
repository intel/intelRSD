/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 *
 * @file manager_utils.hpp
 * @brief Utilities used by managers.
 * */

#include "agent-framework/module-ref/managers/utils/manager_utils.hpp"
#include "agent-framework/module-ref/managers/iscsi_target_manager.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"
#include "agent-framework/module-ref/network_manager.hpp"
#include "agent-framework/module-ref/storage_manager.hpp"

namespace agent_framework {
namespace module {

// template specializations for common components

template<>
GenericManager<model::Manager>& get_manager<model::Manager>() { return ComputeManager::get_instance()->get_module_manager(); }

template<>
GenericManager<model::Chassis>& get_manager<model::Chassis>() { return ComputeManager::get_instance()->get_chassis_manager(); }

// template specializations for compute components

template<>
GenericManager<model::Processor>& get_manager<model::Processor>() { return ComputeManager::get_instance()->get_processor_manager(); }

template<>
GenericManager<model::Dimm>& get_manager<model::Dimm>() { return ComputeManager::get_instance()->get_dimm_manager(); }

template<>
GenericManager<model::MemoryChunk>& get_manager<model::MemoryChunk>() { return ComputeManager::get_instance()->get_memory_chunk_manager(); }

template<>
GenericManager<model::Drive>& get_manager<model::Drive>() { return ComputeManager::get_instance()->get_drive_manager(); }

template<>
GenericManager<model::NetworkInterface>& get_manager<model::NetworkInterface>() { return ComputeManager::get_instance()->get_network_interface_manager(); }

template<>
GenericManager<model::StorageController>& get_manager<model::StorageController>() { return ComputeManager::get_instance()->get_storage_controller_manager(); }

template<>
GenericManager<model::System>& get_manager<model::System>() { return ComputeManager::get_instance()->get_system_manager(); }


// template specializations for network components

template<>
GenericManager<model::Switch>& get_manager<model::Switch>() { return NetworkManager::get_instance()->get_switch_manager(); }

template<>
GenericManager<model::RemoteSwitch>& get_manager<model::RemoteSwitch>() { return NetworkManager::get_instance()->get_remote_switch_manager(); }

template<>
GenericManager<model::SwitchPort>& get_manager<model::SwitchPort>() { return NetworkManager::get_instance()->get_port_manager(); }

template<>
GenericManager<model::Vlan>& get_manager<model::Vlan>() { return NetworkManager::get_instance()->get_vlan_manager(); }

template<>
GenericManager<model::PortVlan>& get_manager<model::PortVlan>() { return NetworkManager::get_instance()->get_port_vlan_manager(); }

template<>
GenericManager<model::Acl>& get_manager<model::Acl>() { return NetworkManager::get_instance()->get_acl_manager(); }

template<>
GenericManager<model::AclRule>& get_manager<model::AclRule>() { return NetworkManager::get_instance()->get_acl_rule_manager(); }

template<>
GenericManager<model::StaticMac>& get_manager<model::StaticMac>() {
    return NetworkManager::get_instance()->get_static_mac_manager(); }

// template specializations for storage components

template<>
GenericManager<model::StorageServices>& get_manager<model::StorageServices>() { return StorageManager::get_instance()->get_storage_services_manager(); }

template<>
GenericManager<model::LogicalDrive>& get_manager<model::LogicalDrive>() { return StorageManager::get_instance()->get_logical_drive_manager(); }

template<>
GenericManager<model::PhysicalDrive>& get_manager<model::PhysicalDrive>() { return StorageManager::get_instance()->get_physical_drive_manager(); }

template<>
managers::IscsiTargetManager& get_manager<model::IscsiTarget, managers::IscsiTargetManager>() { return StorageManager::get_instance()->get_iscsi_target_manager(); }

template<>
GenericManager<model::IscsiTarget>& get_manager<model::IscsiTarget>() { return StorageManager::get_instance()->get_iscsi_target_manager(); }

}
}
