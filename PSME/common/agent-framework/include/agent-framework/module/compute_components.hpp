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
 *
 * @file compute_components.hpp
 * @brief Compute Components Interface
 * */

#pragma once



#include "agent-framework/generic/singleton.hpp"
#include "agent-framework/module/managers/generic_manager.hpp"
#include "agent-framework/module/managers/many_to_many_manager.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "agent-framework/module/model/model_compute.hpp"

/*! Psme namespace */
namespace agent_framework {
namespace module {

/*!
 * @brief Class for managing compute components and subcomponents
 * */
class ComputeComponents : public generic::Singleton<ComputeComponents> {
public:

    using MemoryManager = GenericManager<model::Memory>;
    using MemoryDomainManager = GenericManager<model::MemoryDomain>;
    using MemoryChunksManager = GenericManager<model::MemoryChunks>;
    using NetworkDeviceManager = GenericManager<model::NetworkDevice>;
    using NetworkDeviceFunctionsManager = GenericManager<model::NetworkDeviceFunction>;
    using StorageControllerManager = GenericManager<model::StorageController>;
    using TrustedModuleManager = GenericManager<model::TrustedModule>;


    virtual ~ComputeComponents();


    /*!
     * @brief Get memory manager
     * @return Instance of memory manager
     * */
    MemoryManager& get_memory_manager() {
        return m_memory_manager;
    }


    /*!
     * @brief Get memory domain manager
     * @return Instance of memory domain manager
     * */
    MemoryDomainManager& get_memory_domain_manager() {
        return m_memory_domain_manager;
    }


    /*!
     * @brief Get memory chunks manager
     * @return Instance of memory chunks manager
     * */
    MemoryChunksManager& get_memory_chunks_manager() {
        return m_memory_chunks_manager;
    }


    /*!
     * @brief Get network device manager
     * @return Instance of network device manager
     * */
    NetworkDeviceManager& get_network_device_manager() {
        return m_network_device_manager;
    }


    /*!
     * @brief Get network device function manager
     * @return Reference to a network device function manager instance
     * */
    NetworkDeviceFunctionsManager& get_network_device_function_manager() {
        return m_network_device_functions_manager;
    }


    /*!
     * @brief Get storage controller manager
     * @return Instance of storage controller manager
     * */
    StorageControllerManager& get_storage_controller_manager() {
        return m_storage_controller_manager;
    }


    /*!
     * @brief Get trusted module manager
     * @return Instance of trusted module manager
     * */
    TrustedModuleManager& get_trusted_module_manager() {
        return m_trusted_module_manager;
    }


private:
    MemoryManager m_memory_manager{};
    MemoryDomainManager m_memory_domain_manager{};
    MemoryChunksManager m_memory_chunks_manager{};
    NetworkDeviceManager m_network_device_manager{};
    StorageControllerManager m_storage_controller_manager{};
    NetworkDeviceFunctionsManager m_network_device_functions_manager{};
    TrustedModuleManager m_trusted_module_manager{};
};

}
}
