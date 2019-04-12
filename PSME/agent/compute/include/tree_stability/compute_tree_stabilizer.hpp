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
 * @file compute_tree_stabilizer.hpp
 * */



#pragma once



#include "agent-framework/tree_stability/tree_stabilizer.hpp"

#include <string>



namespace agent {
namespace compute {

/*!
 * @brief Tree stability managing class for compute agent.
 * */
class ComputeTreeStabilizer : public agent_framework::TreeStabilizer {
public:

    /*! Destructor */
    virtual ~ComputeTreeStabilizer();


    /*!
     * Stabilizes compute agent resource tree starting from the module.
     *
     * @param[in] module_uuid Temporary UUID of module to stabilize
     *
     * @return Persistent module UUID
     * */
    virtual const std::string stabilize(const std::string& module_uuid) override;


    /*!
     * Stabilize system and all resources below.
     *
     * @param[in] system_uuid UUID of system to stabilize
     *
     * @return System persistent UUID
     * */
    const std::string stabilize_system(const std::string& system_uuid);


    /*!
     * Stabilize chassis and all resources below.
     *
     * @param[in] chassis_uuid UUID of chassis to stabilize
     *
     * @return Chassis persistent UUID
     * */
    const std::string stabilize_chassis(const std::string& chassis_uuid);


    /*!
     * Stabilize processor and all resources below.
     *
     * @param[in] processor_uuid UUID of processor to stabilize
     *
     * @return Processor persistent UUID
     * */
    const std::string stabilize_processor(const std::string& processor_uuid);


    /*!
     * Stabilize memory and all resources below.
     *
     * @param[in] memory_uuid UUID of memory to stabilize
     *
     * @return Memory persistent UUID
     * */
    const std::string stabilize_memory(const std::string& memory_uuid);


    /*!
     * Stabilize network interface and all resources below.
     *
     * @param[in] nic_uuid UUID of network interface to stabilize
     *
     * @return Network interface persistent UUID
     * */
    const std::string stabilize_nic(const std::string& nic_uuid);


    /*!
     * Stabilize PCIe port and all resources below.
     *
     * @param[in] pcie_port_uuid UUID of PCIe port to stabilize
     *
     * @return PCIe port persistent UUID
     * */
    const std::string stabilize_pcie_port(const std::string& pcie_port_uuid);


    /*!
     * Stabilize storage_controller and all resources below.
     *
     * @param[in] storage_controller_uuid UUID of storage controller to stabilize
     *
     * @return Storage controller persistent UUID
     * */
    const std::string stabilize_storage_controller(const std::string& storage_controller_uuid);


    /*!
     * Stabilize storage_subsystem and all resources below.
     *
     * @param[in] storage_subsystem_uuid UUID of storage subsystem to stabilize
     *
     * @return Storage subsystem persistent UUID
     * */
    const std::string stabilize_storage_subsystem(const std::string& storage_subsystem_uuid);


    /*!
     * Stabilize Network Device and all resources below.
     *
     * @param[in] network_device_uuid UUID of Network Device to stabilize
     *
     * @return Network Device's persistent UUID
     * */
    const std::string stabilize_network_device(const std::string& network_device_uuid);


    /*!
     * Stabilize network_device_function and all resources below.
     *
     * @param[in] network_device_function_uuid UUID of Network Device Function to stabilize
     *
     * @return Network Device Function's persistent UUID
     * */
    const std::string stabilize_network_device_function(const std::string& network_device_function_uuid);

    /*!
     * Stabilize drive and all resources below.
     *
     * @param[in] drive_uuid UUID of the drive to stabilize
     *
     * @return Drive's persistent UUID
     * */
    const std::string stabilize_drive(const std::string& drive_uuid);

    /*!
     * Stabilize trusted module and all resources below.
     *
     * @param[in] uuid UUID of the module to stabilize
     *
     * @return Module's persistent UUID
     * */
    const std::string stabilize_trusted_module(const std::string& uuid);
};

}
}
