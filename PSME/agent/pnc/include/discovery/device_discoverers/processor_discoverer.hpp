/*!
 * @brief Class for discovery of PCIe device represented in Redfish as Processor
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file processor_discoverer.hpp
 */

#pragma once



#include "tools/toolset.hpp"
#include "discovery/device_discoverers/abstract_pcie_device_discoverer.hpp"



namespace agent {
namespace pnc {
namespace discovery {
namespace device_discoverers {

class ProcessorDiscoverer : public AbstractPcieDeviceDiscoverer {

public:

    using BaseClass = AbstractPcieDeviceDiscoverer;
    using SysfsFunctionsVec = std::vector<agent::pnc::sysfs::SysfsFunction>;
    using PcieFunctionsVec = std::vector<agent_framework::model::PcieFunction>;


    ProcessorDiscoverer(const tools::Toolset& tools,
                        const builders::BuilderFactoryPtr factory)
        : AbstractPcieDeviceDiscoverer(tools, factory) {}


    /*!
    * @brief Returns device type name
    * @return Device type name
    */
    const std::string& get_device_type_name() const override {
        static const std::string device_type_name = "processor";
        return device_type_name;
    }


private:

    /*!
     * @brief Handles new processors detected on a port, adds to model, adds links, sends events
     * @param[in] downstream_port_uuid UUID of the Port to perform discovery
     * @return Stabilized UUID of the processor connected to Port with UUID specified in port_uuid parameter
     */
    Uuid discover_oob_add_and_stabilize(const Uuid& downstream_port_uuid) const override;


    /*!
     * @brief Out of band discovery of the processor
     * @param[in] downstream_port_uuid Uuid of the downstream port of the processor
     * @return Discovered processor
     */
    agent_framework::model::Processor discover_oob(const Uuid& downstream_port_uuid) const;


    /*!
    * @brief Discovers pcie devices/functions based on the sysfs data
    * @param[in] dsp_port_uuid Uuid of the Downstream Port
    * @param[in] device_uuid Uuid of the PCIe device
    * @param[in] decoder sysfs decoder
    * @param[in] sysfs_device Sysfs data of the device
    * @return UUID of the PCIe device
    */
    Uuid sysfs_device_discovery(const Uuid& dsp_port_uuid,
                                const Uuid& device_uuid,
                                const agent::pnc::sysfs::SysfsDecoder& decoder,
                                const agent::pnc::sysfs::SysfsDevice& sysfs_device) const override;


    /*!
     * @brief Discovers ib device data based on the sysfs data
     * @param[in] device_uuid Uuid of the PCIe device
     * @param[in] sysfs_device Sysfs data of the device
     */
    void sysfs_discovery(const Uuid& device_uuid,
                         const agent::pnc::sysfs::SysfsDevice& sysfs_device) const override;


    /*!
     * @brief Discovers ib processor and update processor data
     * @param[in] processor Processor to be updated
     * @param[in] sysfs_device Sysfs data of the processor
     */
    agent_framework::model::Processor discovery_ib_processor(const agent_framework::model::Processor& processor,
                                                             const agent::pnc::sysfs::SysfsDevice& sysfs_device) const;


    /*!
     * @brief Performs critical state device discovery (oob device found but no sysfs data)
     * @param[in] device_uuid Uuid of the PCIe device
     */
    void critical_state_device_discovery(const Uuid& device_uuid) const override;


    /*!
     * @brief Ib discovery of the processor when no sysfs data are available
     * @param[in] processor Existing processor to be updated
     * @return Discovered entity
     * */
    agent_framework::model::Processor
    discover_no_sysfs_ib_processor(const agent_framework::model::Processor& processor) const;


    /*!
     * @brief Set device into functional state
     * @param[in] pcie_function Discovered PcieFunction from sysfs function
     * @param[in] device_uuid Uuid of the PCIe device
     * */
    void set_functional_device(agent_framework::model::PcieFunction& pcie_function,
                               const Uuid& device_uuid) const override;


    /*!
     * @brief Synchronizes properties of discovered processor with database processor
     * @param device_uuid UUID of the processor to synchronize with database
     */
    void sync_device_properties_with_db(const Uuid& device_uuid) const override;


    /*!
     * @brief Get pcie functions based on the sysfs device functions
     * @param[in] device_uuid Uuid of the PCIe device
     * @param[in] dsp_port_uuid Uuid of the Downstream Port
     * @param[in] functions Sysfs functions of the PCIe device
     * @return PCIe functions for given PCIe device
     */
    PcieFunctionsVec get_pcie_functions(const Uuid& device_uuid,
                                        const Uuid& dsp_port_uuid,
                                        const SysfsFunctionsVec& functions) const;


    /*!
     * @brief Check if PCIe devices is a processor (from sysfs)
     * @param[in] pcie_functions pcie functions of the PCIe device
     * @return True if device is processor, false otherwise
     */
    bool is_processing_accelerator_class_device(const PcieFunctionsVec& pcie_functions) const;


    /*!
     * @brief Add drive or processor to model based on sysfs data (if not added earlier during oob discovery)
     * @param[in] dsp_port_uuid Uuid of the Downstream Port
     * @param[in] serial_number serial number which will be used to stabilize discovered PCIe device UUID
     * @return stabilized discovered PCIe device UUID
     */
    Uuid add_from_sysfs(const Uuid& dsp_port_uuid,
                        const std::string& serial_number) const override;


    /*!
     * @brief Generate serial number from Downstream Port UUID
     * @param[in] dsp_port_uuid Uuid of the Downstream Port
     * @return generated serial number
     */
    std::string get_serial_number_based_on_dsp_port(const Uuid& dsp_port_uuid) const;


    /*!
     * @brief Stabilizes processor and updates processor-related model structures
     * @param processor Existing processor to be stabilized and used to model update
     * @return Stabilized discovered processor UUID
     */
    const Uuid stabilize_processor_update_model(const agent_framework::model::Processor& processor) const;
};

}
}
}
}
