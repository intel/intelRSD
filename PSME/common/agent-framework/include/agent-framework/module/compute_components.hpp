/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
    using ProcessorManager = GenericManager<model::Processor>;
    using MemoryManager = GenericManager<model::Memory>;
    using NetworkInterfaceManager = GenericManager<model::NetworkInterface>;
    using NetworkDeviceManager = GenericManager<model::NetworkDevice>;
    using NetworkDeviceFunctionsManager = GenericManager<model::NetworkDeviceFunction>;
    using StorageControllerManager = GenericManager<model::StorageController>;

    virtual ~ComputeComponents();


    /*!
     * @brief Get processor manager
     * @return Instance of processor manager
     * */
    ProcessorManager& get_processor_manager() {
        return m_processor_manager;
    }


    /*!
     * @brief Get memory manager
     * @return Instance of memory manager
     * */
    MemoryManager& get_memory_manager() {
        return m_memory_manager;
    }


    /*!
     * @brief Get network interface manager
     * @return Instance of network interface manager
     * */
    NetworkInterfaceManager& get_network_interface_manager() {
        return m_network_interface_manager;
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


private:
    ProcessorManager m_processor_manager{};
    MemoryManager m_memory_manager{};
    NetworkInterfaceManager m_network_interface_manager{};
    NetworkDeviceManager m_network_device_manager{};
    StorageControllerManager m_storage_controller_manager{};
    NetworkDeviceFunctionsManager m_network_device_functions_manager{};
};

}
}
