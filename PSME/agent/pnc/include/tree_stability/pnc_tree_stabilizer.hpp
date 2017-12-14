/*!
 * @brief Provides class for stabilizing compute agent resource tree
 *
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
 * @file pnc_tree_stabilizer.hpp
 * */



#pragma once



#include "agent-framework/tree_stability/tree_stabilizer.hpp"

#include <string>



namespace agent {
namespace pnc {

/*!
 * @brief Tree stability managing class for pnc agent.
 * */
class PncTreeStabilizer : public agent_framework::TreeStabilizer {
public:
    /*! Destructor */
    virtual ~PncTreeStabilizer();


    /*!
     * Stabilizes pnc agent resource tree starting from the module.
     *
     * @param[in] module_uuid Temporary UUID of module to stabilize.
     *
     * @return Persistent module UUID.
     * */
    virtual const std::string stabilize(const std::string& module_uuid) override;


    /*!
     * Stabilizes PCIe device and all resources below.
     *
     * @param device_uuid PCIe device temporary UUID.
     *
     * @return PCIe device persistent UUID.
     * */
    const std::string stabilize_pcie_device(const std::string& device_uuid) const;


    /*!
     * Stabilize PCIe function and all resource below.
     *
     * @param function_uuid UUID of PCIe function to stabilize.
     *
     * @return PCIe function persistent     UUID.
     * */
    const std::string stabilize_pcie_function(const std::string& function_uuid) const;


    /*!
     * Stabilize chassis and all resources below.
     *
     * @param chassis_uuid UUID of chassis to stabilize.
     *
     * @return Chassis persistent UUID.
     * */
    const std::string stabilize_chassis(const std::string& chassis_uuid) const;


    /*!
     * Stabilize PCIe drive and all resources below.
     *
     * @param drive_uuid UUID of drive to stabilize.
     *
     * @return PCIe drive persistent UUID.
     * */
    const std::string stabilize_drive(const std::string& drive_uuid) const;


    /*!
     * Stabilize fabric and all resources below.
     *
     * @param fabric_uuid UUID of fabric to stabilize.
     *
     * @return Fabric persistent UUID.
     */
    const std::string stabilize_fabric(const std::string& fabric_uuid) const;


    /*!
     * Stabilize PCIe switch and all resource below.
     *
     * @param switch_uuid UUID of PCIe switch to stabilize.
     *
     * @return PCIe switch persistent UUID.
     * */
    const std::string stabilize_pcie_switch(const std::string& switch_uuid) const;


    /*!
     * Stabilize port and all resources below.
     *
     * @param port_uuid UUID of port to stabilize.
     *
     * @return Port persistent UUID.
     */
    const std::string stabilize_port(const std::string& port_uuid) const;


    /*!
     * Stabilize port metric
     *
     * @param in_port_uuid temporary UUID of the port
     * @param port_uuid already stabilized UUID of the port whose metric is to be stabilized
     */
    void stabilize_port_metric(const std::string& in_port_uuid, const std::string& port_uuid) const;


    /*!
     * Stabilize PCIe zone and all resource below.
     *
     * @param zone_uuid UUID of PCIe zone to stabilize.
     *
     * @return PCIe zone persistent UUID.
     * */
    const std::string stabilize_pcie_zone(const std::string& zone_uuid) const;


    /*!
     * Stabilize PCIe endpoint and all resources below.
     *
     * @param endpoint_uuid UUID of endpoint to stabilize.
     *
     * @return Endpoint persistent UUID.
     * */
    const std::string stabilize_pcie_endpoint(const std::string& endpoint_uuid) const;


    /*!
     * Stabilize system and all resources below.
     *
     * @param system_uuid UUID of system to stabilize.
     *
     * @return System persistent UUID.
     */
    const std::string stabilize_system(const std::string& system_uuid) const;


    /*!
     * Stabilize storage subsystem and all resources below.
     *
     * @param subsystem_uuid UUID of storage susbsystem to stabilize.
     *
     * @return Storage subsystem persistent UUID.
     * */
    const std::string stabilize_storage_subsystem(const std::string& subsystem_uuid) const;


    /*!
     * Stabilize metric definition
     *
     * @param definition_uuid UUID of metric definition to stabilize.
     *
     * @return Metric definition persistent UUID.
     * */
    const std::string stabilize_metric_definition(const std::string& definition_uuid) const;
};

}
}
