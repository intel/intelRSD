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
 * @file pnc_tree_stabilizer.hpp
 * */



#pragma once



#include "agent-framework/tree_stability/tree_stabilizer.hpp"
#include "agent-framework/module/utils/uuid.hpp"
#include "tree_stability/pnc_key_generator.hpp"
#include "tree_stability/helpers/update_relations.hpp"
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
    virtual const Uuid stabilize(const Uuid& module_uuid) override;


    /*!
     * Stabilizes PCIe device and all resources below.
     *
     * @param device_uuid PCIe device temporary UUID.
     *
     * @return PCIe device persistent UUID.
     * */
    const Uuid stabilize_pcie_device(const Uuid& device_uuid) const;


    /*!
     * Stabilizes PCIe device and all resources below using external unique key associated with the physical device.
     *
     * @tparam Device type of the device supported by PNC agent (Drive, Processor)
     * @param device_uuid PCIe device temporary UUID.
     * @param external_device_unique_key external unique key of the physical device which is used by PCIe device.
     *
     * @return PCIe device persistent UUID.
     * */
    template<typename Device>
    const Uuid stabilize_pcie_device(const Uuid& in_device_uuid,
                                     const std::string& external_device_unique_key) const {
        auto& device_manager = agent_framework::module::get_manager<agent_framework::model::PcieDevice>();
        const auto& device = device_manager.get_entry(in_device_uuid);
        std::string device_uuid{in_device_uuid};

        try {
            const std::string& device_unique_key = PncKeyGenerator::generate_key(device, external_device_unique_key);
            auto& function_manager = agent_framework::module::get_manager<agent_framework::model::PcieFunction>();

            device_uuid = stabilize_single_resource(device_uuid, device_manager,
                                                    device_unique_key);

            for (const auto& function_uuid : function_manager.get_keys(in_device_uuid)) {
                function_manager.get_entry_reference(function_uuid)->set_parent_uuid(device_uuid);
                stabilize_pcie_function<Device>(function_uuid, external_device_unique_key);
            }

        }
        catch (const agent_framework::KeyValueMissingError&) {
            agent_framework::log_key_value_missing(agent_framework::model::PcieDevice::get_component().to_string(),
                                                   in_device_uuid);
        }

        return device_uuid;
    }


    /*!
     * @brief Stabilize PCIe function and all resource below.
     *
     * @tparam Device type of the device supported by PNC agent (Drive, Processor)
     * @param function_uuid UUID of PCIe function to stabilize.
     *
     * @return PCIe function persistent UUID.
     * */
    template<typename Device>
    const Uuid stabilize_pcie_function(const Uuid& in_function_uuid) const {
        auto& function_manager = agent_framework::module::get_manager<agent_framework::model::PcieFunction>();
        const auto& function = function_manager.get_entry(in_function_uuid);
        const auto& parent_device = agent_framework::module::get_manager<agent_framework::model::PcieDevice>().get_entry(
            function.get_parent_uuid());
        std::string function_uuid{in_function_uuid};

        try {
            const std::string& function_unique_key = PncKeyGenerator::generate_key(function, parent_device);

            function_uuid = stabilize_single_resource(function_uuid, function_manager,
                                                      function_unique_key);

            helpers::update_pcie_function_in_relations<Device>(in_function_uuid, function_uuid);
        }
        catch (const agent_framework::KeyValueMissingError&) {
            agent_framework::log_key_value_missing(agent_framework::model::PcieFunction::get_component().to_string(),
                                                   in_function_uuid);
        }

        return function_uuid;
    }


    /*!
     * @brief Stabilize PCIe function and all resource below using external unique key associated with the physical device.
     *
     * @tparam Device type of the device supported by PNC agent (Drive, Processor)
     * @param function_uuid UUID of PCIe function to stabilize.
     * @param external_device_unique_key external unique key of the physical device which is used by PCIe device.
     *
     * @return PCIe function persistent UUID.
     */
    template<typename Device>
    const Uuid stabilize_pcie_function(const Uuid& in_function_uuid,
                                       const std::string& external_device_unique_key) const {
        auto& function_manager = agent_framework::module::get_manager<agent_framework::model::PcieFunction>();
        const auto& function = function_manager.get_entry(in_function_uuid);
        std::string function_uuid{in_function_uuid};

        try {
            const std::string& function_unique_key = PncKeyGenerator::generate_key(function,
                                                                                   external_device_unique_key);

            function_uuid = stabilize_single_resource(function_uuid, function_manager,
                                                      function_unique_key);

            helpers::update_pcie_function_in_relations<Device>(in_function_uuid, function_uuid);
        }
        catch (const agent_framework::KeyValueMissingError&) {
            agent_framework::log_key_value_missing(agent_framework::model::PcieFunction::get_component().to_string(),
                                                   in_function_uuid);
        }

        return function_uuid;
    }


    /*!
     * Tries to stabilize PCIe device and all resources below using unique key from device model obtained during OOB discovery.
     *
     * @param device_uuid PCIe device temporary UUID.
     *
     * @return PCIe device persistent UUID.
     */
    const Uuid try_stabilize_pcie_device_using_oob_model(const Uuid& device_uuid);


    /*!
     * Stabilize chassis and all resources below.
     *
     * @param chassis_uuid UUID of chassis to stabilize.
     *
     * @return Chassis persistent UUID.
     * */
    const Uuid stabilize_chassis(const Uuid& chassis_uuid) const;


    /*!
     * Stabilize PCIe drive and all resources below.
     *
     * @param drive_uuid UUID of drive to stabilize.
     *
     * @return PCIe drive persistent UUID.
     * */
    const Uuid stabilize_drive(const Uuid& drive_uuid) const;


    /*!
     * Stabilize fabric and all resources below.
     *
     * @param fabric_uuid UUID of fabric to stabilize.
     *
     * @return Fabric persistent UUID.
     */
    const Uuid stabilize_fabric(const Uuid& fabric_uuid) const;


    /*!
     * Stabilize PCIe switch and all resource below.
     *
     * @param switch_uuid UUID of PCIe switch to stabilize.
     *
     * @return PCIe switch persistent UUID.
     * */
    const Uuid stabilize_pcie_switch(const Uuid& switch_uuid) const;


    /*!
     * Stabilize port and all resources below.
     *
     * @param port_uuid UUID of port to stabilize.
     *
     * @return Port persistent UUID.
     */
    const Uuid stabilize_port(const Uuid& port_uuid) const;


    /*!
     * Stabilize port metric
     *
     * @param in_port_uuid temporary UUID of the port
     * @param port_uuid already stabilized UUID of the port whose metric is to be stabilized
     */
    void stabilize_port_metric(const Uuid& in_port_uuid, const Uuid& port_uuid) const;


    /*!
     * Stabilize PCIe zone and all resource below.
     *
     * @param zone_uuid UUID of PCIe zone to stabilize.
     *
     * @return PCIe zone persistent UUID.
     * */
    const Uuid stabilize_pcie_zone(const Uuid& zone_uuid) const;


    /*!
     * Stabilize PCIe endpoint and all resources below.
     *
     * @param endpoint_uuid UUID of endpoint to stabilize.
     *
     * @return Endpoint persistent UUID.
     * */
    const Uuid stabilize_pcie_endpoint(const Uuid& endpoint_uuid) const;


    /*!
     * Stabilize system and all resources below.
     *
     * @param system_uuid UUID of system to stabilize.
     *
     * @return System persistent UUID.
     */
    const Uuid stabilize_system(const Uuid& system_uuid) const;


    /*!
     * Stabilize storage subsystem and all resources below.
     *
     * @param subsystem_uuid UUID of storage susbsystem to stabilize.
     *
     * @return Storage subsystem persistent UUID.
     * */
    const Uuid stabilize_storage_subsystem(const Uuid& subsystem_uuid) const;


    /*!
     * Stabilize metric definition
     *
     * @param definition_uuid UUID of metric definition to stabilize.
     *
     * @return Metric definition persistent UUID.
     * */
    const Uuid stabilize_metric_definition(const Uuid& definition_uuid) const;


    /*!
     * Stabilize processor and all resources below.
     *
     * @param processor_uuid UUID of processor to stabilize.
     *
     * @return Processor persistent UUID.
     */
    const Uuid stabilize_processor(const Uuid& processor_uuid) const;
};

}
}
