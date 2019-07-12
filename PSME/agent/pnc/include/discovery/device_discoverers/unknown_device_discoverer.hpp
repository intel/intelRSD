/*!
 * @brief A discoverer for PCIe devices not recognized by PSME PNC agent
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
 * @file unknown_device_discoverer.hpp
 */

#pragma once



#include "discovery/device_discoverers/abstract_pcie_device_discoverer.hpp"



namespace agent {
namespace pnc {
namespace discovery {
namespace device_discoverers {

class UnknownDeviceDiscoverer : public AbstractPcieDeviceDiscoverer {

public:

    UnknownDeviceDiscoverer(const tools::Toolset& tools,
                            const builders::BuilderFactoryPtr factory)
        : AbstractPcieDeviceDiscoverer(tools, factory) {}


    /*!
    * @brief Returns device type name
    * @return Device type name
    */
    const std::string& get_device_type_name() const override {
        static const std::string device_type_name = "unknown PCIe device";
        return device_type_name;
    }


    /*!
     * @brief Performs out-of-band discovery of device on a specific port
     * @param[in] gas Instance of GAS registers
     * @param[in] dsp_port_uuid Uuid of the Downstream Port
     * @return True if discovery was successful
     * */
    bool oob_port_device_discovery(const gas::GlobalAddressSpaceRegisters& gas,
                                   const Uuid& dsp_port_uuid) const override;


    /*!
     * @brief Performs in-band discovery of device on a specific port
     * @param[in] switch_uuid Uuid of the Pcie Switch
     * @param[in] dsp_port_uuid Uuid of the Downstream Port
     * @param[in] bridge_id Id of the bridge that should be used during discovery (as visible on pcie switch)
     * @param[in] device_uuid Uuid of the device created during out-of-band discovery
     * @return True if discovery was successful
     * */
    bool ib_port_device_discovery(const Uuid& switch_uuid,
                                  const Uuid& dsp_port_uuid,
                                  uint8_t bridge_id,
                                  const Uuid& device_uuid) const override;


private:

    /*!
     * @brief Handles new devices detected on a port, adds to model, adds links, sends events
     * @param[in] downstream_port_uuid UUID of the Port to perform discovery
     * @return Stabilized UUID of the device connected to Port with UUID specified in port_uuid parameter
     */
    Uuid discover_oob_add_and_stabilize(const Uuid& downstream_port_uuid) const override;


    /*!
     * @brief Checks if device type for this discoverer is recognized
     * @return True if device is of recognized type. False if it's unknown device
     */
    bool is_recognized_device() const override { return false; };


    /*!
     * @brief Performs critical state device discovery (oob device found but no sysfs data)
     * @param[in] device_uuid Uuid of the PCIe device
     */
    void critical_state_device_discovery(const Uuid& device_uuid) const override;


    /*!
     * @brief Set device into functional state
     * @param[in] pcie_function Discovered PcieFunction from sysfs function
     * @param[in] device_uuid Uuid of the PCIe device
     * */
    void set_functional_device(agent_framework::model::PcieFunction& pcie_function,
                               const Uuid& device_uuid) const override;


    /*!
     * @brief Discovers ib device data based on the sysfs data
     * @param[in] device_uuid Uuid of the PCIe device
     * @param[in] sysfs_device Sysfs data of the device
     */
    void sysfs_discovery(const Uuid& device_uuid,
                         const agent::pnc::sysfs::SysfsDevice& sysfs_device) const override;


    /*!
     * @brief Synchronizes properties of discovered device with database device
     * @param device_uuid UUID of the device to synchronize with database
     */
    void sync_device_properties_with_db(const Uuid& device_uuid) const override;


    /*!
     * @brief Add drive or processor to model based on sysfs data (if not added earlier during oob discovery)
     * @param[in] dsp_port_uuid Uuid of the Downstream Port
     * @param[in] serial_number serial number which will be used to stabilize discovered PCIe device UUID
     * @return stabilized discovered PCIe device UUID
     */
    Uuid add_from_sysfs(const Uuid& dsp_port_uuid,
                        const std::string& serial_number) const override;
};

}
}
}
}