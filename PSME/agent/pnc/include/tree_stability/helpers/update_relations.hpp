/*!
 * @brief Provides class for stabilizing compute agent resource tree
 *
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
 * @file update_relations.hpp
 * */

#pragma once



#include<string>
#include "agent-framework/module/common_components.hpp"



namespace agent {
namespace pnc {
namespace helpers {

/*!
 * Update all relations in the model involving PCIe switch
 *
 * @param pcie_switch_temporary_uuid PCIe switch UUID before tree stabilization
 * @param pcie_switch_persistent_uuid PCIe switch UUID after tree stabilization
 * */
void update_pcie_switch_in_relations(const std::string& pcie_switch_temporary_uuid,
                                     const std::string& pcie_switch_persistent_uuid);


/*!
 * Update all relations in the model involving PCIe endpoint
 *
 * @param pcie_endpoint_temporary_uuid PCIe endpoint UUID before tree stabilization
 * @param pcie_endpoint_persistent_uuid PCIe endpoint UUID after tree stabilization
 * */
void update_pcie_endpoint_in_relations(const std::string& pcie_endpoint_temporary_uuid,
                                       const std::string& pcie_endpoint_persistent_uuid);


/*!
 * Update all relations in the model involving PCIe zone
 *
 * @param pcie_zone_temporary_uuid PCIe zone UUID before tree stabilization
 * @param pcie_zone_persistent_uuid PCIe zone after tree stabilization
 * */
void update_pcie_zone_in_relations(const std::string& pcie_zone_temporary_uuid,
                                   const std::string& pcie_zone_persistent_uuid);


/*!
 * Update all relations in the model involving PCIe port
 *
 * @param pcie_port_temporary_uuid PCIe port UUID before tree stabilization
 * @param pcie_port_persistent_uuid PCIe port UUID after tree stabilization
 * */
void update_pcie_port_in_relations(const std::string& pcie_port_temporary_uuid,
                                   const std::string& pcie_port_persistent_uuid);


/*!
 * Update all relations in the model involving chassis
 *
 * @param chassis_temporary_uuid Chassis UUID before tree stabilization
 * @param chassis_persistent_uuid Chassis UUID after tree stabilization
 * */
void update_chassis_in_relations(const std::string& chassis_temporary_uuid, const std::string& chassis_persistent_uuid);


/*!
 * Update all relations in the model involving PCIe drive
 *
 * @param pcie_drive_temporary_uuid PCIe drive UUID before tree stabilization
 * @param pcie_drive_persistent_uuid PCIe drive after tree stabilization
 * */
void update_pcie_drive_in_relations(const std::string& pcie_drive_temporary_uuid,
                                    const std::string& pcie_drive_persistent_uuid);


/*!
 * Update all relations in the model involving storage subsystem
 *
 * @param storage_subsystem_temporary_uuid Storage subsystem UUID before tree stabilization
 * @param storage_subsystem_persistent_uuid Storage subsystem UUID after tree stabilization
 * */
void update_storage_subsystem_in_relations(const std::string& storage_subsystem_temporary_uuid,
                                           const std::string& storage_subsystem_persistent_uuid);


/*!
 * Update all relations in the model involving processor
 *
 * @param processor_persistent_uuid Processor UUID before tree stabilization
 * @param processor_persistent_uuid Processor UUID after tree stabilization
 * */
void update_processor_in_relations(const std::string& processor_temporary_uuid,
                                   const std::string& processor_persistent_uuid);


/*!
 * Update all relations in the model involving PCIe device
 *
 * @param pcie_device_temporary_uuid PCIe device UUID before tree stabilization
 * @param pcie_device_persistent_uuid PCIe device UUID after tree stabilization
 * */
void update_pcie_device_in_relations(const std::string& pcie_device_temporary_uuid,
                                     const std::string& pcie_device_persistent_uuid);


/*!
 * Update all relations in the model involving PCIe function
 *
 * @tparam Device type of the device supported by PNC agent (Drive, Processor)
 * @param pcie_function_temporary_uuid PCIe function UUID before tree stabilization
 * @param pcie_function_persistent_uuid PCIe function UUID after tree stabilization
 * */
template<typename Device>
void update_pcie_function_in_relations(const std::string& pcie_function_temporary_uuid,
                                       const std::string& pcie_function_persistent_uuid) {
    agent_framework::module::get_m2m_manager<Device, agent_framework::model::PcieFunction>().update_child(
        pcie_function_temporary_uuid,
        pcie_function_persistent_uuid);
}

}
}
}
