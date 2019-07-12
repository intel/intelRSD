/*!
 * @brief Class for discovery of NVMe drives
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
 * @file drive_discoverer.hpp
 */

#pragma once



#include "discovery/device_discoverers/abstract_pcie_device_discoverer.hpp"
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/model_pnc.hpp"



namespace agent {
namespace pnc {
namespace discovery {
namespace device_discoverers {

class DriveDiscoverer : public AbstractPcieDeviceDiscoverer {

public:

    using BaseClass = AbstractPcieDeviceDiscoverer;


    DriveDiscoverer(const tools::Toolset& tools,
                    const builders::BuilderFactoryPtr factory)
        : AbstractPcieDeviceDiscoverer(tools, factory) {}


    /*!
    * @brief Returns device type name
    * @return Device type name
    */
    const std::string& get_device_type_name() const override {
        static const std::string device_type_name = "drive";
        return device_type_name;
    }


private:

    /*!
    * @brief Handles new drives detected on a port, adds to model, adds links, sends events
    * @param[in] downstream_port_uuid UUID of the Port to perform discovery
    * @return Stabilized UUID of the drive connected to Port with UUID specified in port_uuid parameter
    */
    Uuid discover_oob_add_and_stabilize(const Uuid& downstream_port_uuid) const override;


    /*!
     * @brief Out of band discovery of the drive
     * @param[in] downstream_port_uuid Uuid of the downstream port of the drive
     * @return Discovered drive
     */
    agent_framework::model::Drive discover_oob(const Uuid& downstream_port_uuid) const;


    /*!
     * @brief Ib discovery of the drive when no sysfs data are available
     * @param[in] drive Existing drive to be updated
     * @return Discovered entity
     * */
    agent_framework::model::Drive discover_no_sysfs_ib_drive(const agent_framework::model::Drive& drive) const;


    /*!
     * @brief Performs critical state device discovery (oob device found but no sysfs data)
     * @param[in] device_uuid Uuid of the PCIe device
     */
    void critical_state_device_discovery(const Uuid& drive_uuid) const override;


    /*!
     * @brief Set device into functional state
     * @param[in] discovered pcie function from sysfs function
     * @param[in] device_uuid Uuid of the PCIe device
     * @return Discovered entity
     * */
    void set_functional_device(agent_framework::model::PcieFunction& pcie_function,
                               const Uuid& device_uuid) const override;


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
    void sysfs_discovery(const Uuid& drive_uuid,
                         const agent::pnc::sysfs::SysfsDevice& sysfs_device) const override;


    /*!
     * @brief Ib discovery of the drive
     * @param[in] drive Existing drive to be updated
     * @param[in] sysfs_device Sysfs data of the device
     * @param[in] sysfs_drive Sysfs data of the drive
     * @return Discovered entity
     * */
    agent_framework::model::Drive discover_ib_drive(const agent_framework::model::Drive& drive,
                                                    const agent::pnc::sysfs::SysfsDevice& sysfs_device,
                                                    const agent::pnc::sysfs::SysfsDrive& sysfs_drive) const;


    /*!
     * @brief Synchronizes properties of discovered drive with database drive
     * @param device_uuid UUID of the drive to synchronize with database
     */
    void sync_device_properties_with_db(const Uuid& device_uuid) const override;


    /*!
     * @brief Add drive or processor to model based on sysfs data (if not added earlier during oob discovery)
     * @param[in] dsp_port_uuid Uuid of the Downstream Port
     * @param[in] serial_number serial number which will be used to stabilize discovered PCIe device UUID
     * @return Stabilized discovered PCIe device UUID
     */
    Uuid add_from_sysfs(const Uuid& dsp_port_uuid,
                        const std::string& serial_number) const override;


    /*!
     * @brief Stabilizes drive and updates drive-related model structures
     * @param drive Existing drive to be stabilized and used to model update
     * @return Stabilized discovered drive UUID
     */
    const Uuid stabilize_drive_update_model(const agent_framework::model::Drive& drive) const;
};

}
}
}
}
