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
 * @file compute_manager.hpp
 * @brief Compute Components Interface
 * */

#ifndef AGENT_FRAMEWORK_MODULE_COMPUTE_MANAGER_HPP
#define AGENT_FRAMEWORK_MODULE_COMPUTE_MANAGER_HPP

#include "agent-framework/generic/singleton.hpp"
#include "agent-framework/module-ref/managers/generic_manager.hpp"
#include "agent-framework/module-ref/managers/utils/manager_utils.hpp"
#include "agent-framework/module-ref/model/chassis.hpp"
#include "agent-framework/module-ref/model/manager.hpp"
#include "agent-framework/module-ref/model/system.hpp"
#include "agent-framework/module-ref/model/processor.hpp"
#include "agent-framework/module-ref/model/dimm.hpp"
#include "agent-framework/module-ref/model/memory_chunk.hpp"
#include "agent-framework/module-ref/model/network_interface.hpp"
#include "agent-framework/module-ref/model/vlan.hpp"
#include "agent-framework/module-ref/model/storage_controller.hpp"
#include "agent-framework/module-ref/model/drive.hpp"

/*! Psme namespace */
namespace agent_framework {
namespace module {

using namespace agent_framework::model;

/*!
 * @brief Class for managing compute components and subcomponents
 * */
class ComputeManager : public generic::Singleton<ComputeManager> {
public:
    using ModuleManager = GenericManager<model::Manager>;
    using SystemManager = GenericManager<model::System>;
    using ProcessorManager = GenericManager<model::Processor>;
    using DimmManager = GenericManager<model::Dimm>;
    using MemoryChunkManager = GenericManager<model::MemoryChunk>;
    using NetworkInterfaceManager = GenericManager<model::NetworkInterface>;
    using PortVlanManager = GenericManager<model::Vlan>;
    using StorageControllerManager = GenericManager<model::StorageController>;
    using DriveManager = GenericManager<model::Drive>;
    using ChassisManager = GenericManager<model::Chassis>;

    virtual ~ComputeManager();

    /*!
     * @brief Get system manager
     * @return Instance of module manager
     * */
    ModuleManager& get_module_manager() {
        return m_module_manager;
    }

    /*!
     * @brief Get system manager
     * @return Instance of system manager
     * */
    SystemManager& get_system_manager() {
        return m_system_manager;
    }

    /*!
     * @brief Get processor manager
     * @return Instance of processor manager
     * */
    ProcessorManager& get_processor_manager() {
        return m_processor_manager;
    }

    /*!
     * @brief Get dimm manager
     * @return Instance of dimm manager
     * */
    DimmManager& get_dimm_manager() {
        return m_dimm_manager;
    }

    /*!
     * @brief Get memory chunk manager
     * @return Instance of memory chunk manager
     * */
    MemoryChunkManager& get_memory_chunk_manager() {
        return m_memory_chunk_manager;
    }

    /*!
     * @brief Get network interface manager
     * @return Instance of network interface manager
     * */
    NetworkInterfaceManager& get_network_interface_manager() {
        return m_network_interface_manager;
    }

    /*!
     * @brief Get port VLAN manager
     * @return Instance of port VLAN manager
     * */
    PortVlanManager& get_port_vlan_manager() {
        return m_port_vlan_manager;
    }

    /*!
     * @brief Get storage controller manager
     * @return Instance of storage controller manager
     * */
    StorageControllerManager& get_storage_controller_manager() {
        return m_storage_controller_manager;
    }

    /*!
     * @brief Get drive manager
     * @return Instance of drive manager
     * */
    DriveManager& get_drive_manager() {
        return m_drive_manager;
    }

    /*!
     * @brief Get chassis manager
     * @return Reference to a chassis manager instance
     * */
    ChassisManager& get_chassis_manager() {
        return m_chassis_manager;
    }


private:
    ModuleManager m_module_manager{};
    SystemManager m_system_manager{};
    ProcessorManager m_processor_manager{};
    DimmManager m_dimm_manager{};
    MemoryChunkManager m_memory_chunk_manager{};
    NetworkInterfaceManager m_network_interface_manager{};
    PortVlanManager m_port_vlan_manager{};
    StorageControllerManager m_storage_controller_manager{};
    DriveManager m_drive_manager{};
    ChassisManager m_chassis_manager{};
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_COMPUTE_Manager_HPP */
