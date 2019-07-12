/*!
 * @brief Abstract class for discovery of PCIe devices
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
 * @file abstract_pcie_device_discoverer.hpp
 */

#include "gas/global_address_space_registers.hpp"
#include "agent-framework/module/model/model_pnc.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"
#include "tree_stability/pnc_stabilizer.hpp"
#include "tree_stability/pnc_tree_stabilizer.hpp"
#include "discovery/discovery_actions.hpp"
#include "tools/toolset.hpp"
#include "discovery/discovery_exceptions.hpp"
#include "discovery/builders/builder_factory.hpp"
#include "sysfs/sysfs_decoder.hpp"



#pragma once

namespace agent {
namespace pnc {
namespace discovery {
namespace device_discoverers {

class AbstractPcieDeviceDiscoverer {

public:

    AbstractPcieDeviceDiscoverer(const tools::Toolset& tools,
                                 const builders::BuilderFactoryPtr factory) :
        m_tools{tools},
        m_factory{factory} {}


    /*!
    * @brief Performs out-of-band discovery of device on a specific port
    * @param[in] gas Instance of GAS registers
    * @param[in] dsp_port_uuid Uuid of the Downstream Port
    * @return True if discovery was successful
    * */
    virtual bool oob_port_device_discovery(const gas::GlobalAddressSpaceRegisters& gas,
                                           const Uuid& dsp_port_uuid) const;


    /*!
    * @brief Performs in-band discovery of device on a specific port
     * @param[in] switch_uuid Uuid of the Pcie Switch
     * @param[in] dsp_port_uuid Uuid of the Downstream Port
     * @param[in] bridge_id Id of the bridge that should be used during discovery (as visible on pcie switch)
     * @param[in] device_uuid Uuid of the device created during out-of-band discovery
    * @return True if discovery was successful
    * */
    virtual bool ib_port_device_discovery(const Uuid& switch_uuid,
                                          const Uuid& dsp_port_uuid,
                                          uint8_t bridge_id,
                                          const Uuid& device_uuid) const;


    /*!
     * @brief Returns device type name (eg. "processor", "drive")
     * @return Device type name
    */
    virtual const std::string& get_device_type_name() const = 0;


protected:

    /*!
    * @brief Ib discovery of the pcie device
    * @param[in] manager_uuid Uuid of the parent manager
    * @param[in] chassis_uuid Uuid of the chassis containing the device
    * @param[in] sysfs_device Sysfs data of the device
    * @return Discovered entity
    * */
    agent_framework::model::PcieDevice discover_pcie_device(const Uuid& manager_uuid,
                                                            const Uuid& chassis_uuid,
                                                            const agent::pnc::sysfs::SysfsDevice& sysfs_device) const;


    /*!
     * @brief Ib discovery of the pcie function
     * @param[in] device_uuid Uuid of the parent device
     * @param[in] dsp_port_uuid Uuid of the downstream port of function
     * @param[in] sysfs_function Sysfs data of the function
     * @return Discovered entity
     * */
    agent_framework::model::PcieFunction discover_pcie_function(const Uuid& device_uuid,
                                                                const Uuid& dsp_port_uuid,
                                                                const agent::pnc::sysfs::SysfsFunction& sysfs_function) const;


    /*!
     * @brief Full Discovery of the device target endpoint
     * @param[in] gas Reference to Global Address Space Register
     * @param[in] device_uuid Uuid of the PCIe device
     * @param[in] dsp_port_uuid Uuid of the downstream port
     */
    void discover_target_endpoint(const gas::GlobalAddressSpaceRegisters& gas, const Uuid& device_uuid,
                                  const Uuid& dsp_port_uuid) const;

private:

    /*!
     * @brief Creates a link between a newly detected endpoint and its zone
     * @param[in] gas Reference to Global Address Space Register
     * @param[in] endpoint_uuid UUID of newly discovered endpoint
     * @param[in] port Downstream Port associated with Endpoint
     */
    void update_endpoint_zone_binding(const gas::GlobalAddressSpaceRegisters& gas,
                                      const Uuid& endpoint_uuid,
                                      const agent_framework::model::Port& port) const;


    /*!
     * @brief Adds and stabilizes endpoint
     * @param endpoint Endpoint to stabilize
     * @param[in] port Downstream Port associated with Endpoint
     * @param[in] device_uuid UUID of device connected to the port
     * @return Stabilized UUID of new Endpoint
     */
    Uuid add_and_stabilize_endpoint(agent_framework::model::Endpoint& endpoint,
                                    const agent_framework::model::Port& port,
                                    const Uuid& device_uuid) const;


    /*!
     * @brief Handles new devices detected on a port, adds to model, adds links, sends events
     * @param[in] downstream_port_uuid UUID of the Port to perform discovery
     * @return Stabilized UUID of the device connected to Port with UUID specified in port_uuid parameter
     */
    virtual Uuid discover_oob_add_and_stabilize(const Uuid& downstream_port_uuid) const = 0;


    /*!
     * @brief Performs critical state device discovery (oob device found but no sysfs data)
     * @param[in] device_uuid Uuid of the PCIe device
     */
    virtual void critical_state_device_discovery(const Uuid& device_uuid) const = 0;


    /*!
     * @brief Discovers pcie devices/functions based on the sysfs data
     * @param[in] dsp_port_uuid Uuid of the Downstream Port
     * @param[in] device_uuid Uuid of the PCIe device
     * @param[in] sysfs decoder
     * @param[in] sysfs_device Sysfs data of the device
     * @return UUID of the PCIe device
     */
    virtual Uuid sysfs_device_discovery(const Uuid& dsp_port_uuid,
                                        const Uuid& device_uuid,
                                        const agent::pnc::sysfs::SysfsDecoder& decoder,
                                        const agent::pnc::sysfs::SysfsDevice& sysfs_device) const;


    /*!
     * @brief Discovers ib device data based on the sysfs data
     * @param[in] device_uuid Uuid of the PCIe device
     * @param[in] sysfs_device Sysfs data of the device
     */
    virtual void sysfs_discovery(const Uuid& device_uuid,
                                 const agent::pnc::sysfs::SysfsDevice& sysfs_device) const = 0;


    /*!
     * @brief Set device into functional state
     * @param[in] pcie_function Discovered pcie function from sysfs function
     * @param[in] device_uuid Uuid of the PCIe device
     * @return Discovered entity
     * */
    virtual void set_functional_device(agent_framework::model::PcieFunction& pcie_function,
                                       const Uuid& device_uuid) const = 0;


    /*!
     * @brief Checks if device type for this discoverer is recognized
     * @return True if device is of recognized type. False if it's unknown device
     */
    virtual bool is_recognized_device() const { return true; }


    /*!
     * @brief Recreate target endpoint from db during endpoint discovery
     * @param[in] fabric_uuid Uuid of the parent fabric
     * @param[in] device_uuid Uuid of the PCIe device
     * @return Recreated endpoint entity
     */
    agent_framework::model::Endpoint recreate_target_endpoint_from_db(const Uuid& fabric_uuid,
                                                                      const Uuid& device_uuid,
                                                                      const Uuid& port_uuid) const;


    /*!
     * @brief Synchronizes properties of discovered device with database device
     * @param device_uuid UUID of the device to synchronize with database
     */
    virtual void sync_device_properties_with_db(const Uuid& device_uuid) const = 0;


    /*!
     * @brief Add drive or processor to model based on sysfs data (if not added earlier during oob discovery)
     * @param[in] dsp_port_uuid Uuid of the Downstream Port
     * @param[in] serial_number serial number which will be used to stabilize discovered PCIe device UUID
     * @return stabilized discovered PCIe device UUID
     */
    virtual Uuid add_from_sysfs(const Uuid& dsp_port_uuid,
                                const std::string& serial_number) const = 0;


protected:

    tools::Toolset m_tools{};
    builders::BuilderFactoryPtr m_factory{};
};

}
}
}
}
