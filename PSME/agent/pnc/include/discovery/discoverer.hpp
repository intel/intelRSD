/*!
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * @file discoverer.hpp
 *
 * @brief Discoverer interface - performs discovery steps for various model objects
 * */

#pragma once

#include "tools/toolset.hpp"
#include "sysfs/sysfs_dto.hpp"
#include "gas/global_address_space_registers.hpp"
#include "agent-framework/module/enum/chassis.hpp"

#include "discovery/builders/builder_factory.hpp"
#include "discovery/discovery_exceptions.hpp"

namespace agent {
namespace pnc {
namespace discovery {

/*!
 * @brief Discoverer class
 * */
class Discoverer {

public:

    /*!
     * @brief Default constructor.
     * @param[in] platform Platform type
     * @param[in] factory Builder factory to be used
     * */
    Discoverer(agent_framework::model::enums::PlatformType platform, builders::BuilderFactoryPtr factory):
        m_platform(platform), m_factory(factory) {}

    /* Enable copy*/
    Discoverer(const Discoverer&) = default;
    Discoverer& operator=(const Discoverer&) = default;

    /*!
     * @brief Default destructor.
     * */
    virtual ~Discoverer();

    /*!
     * @brief Full discovery of the fabric
     * @param[in] manager_uuid Uuid of the parent manager
     * @return Discovered entity
     * */
    virtual agent_framework::model::Fabric discover_fabric(const std::string& manager_uuid) const;

    /*!
     * @brief Full discovery of the fabric
     * @param[in] manager_uuid Uuid of the parent manager
     * @param[in] chassis_uuid Uuid of the chassis containing the system
     * @return Discovered entity
     * */
    virtual agent_framework::model::System discover_system(const std::string& manager_uuid,
        const std::string& chassis_uuid) const;

    /*!
     * @brief Full discovery of the storage subsystem
     * @param[in] system_uuid Uuid of the parent system
     * @return Discovered entity
     * */
    virtual agent_framework::model::StorageSubsystem discover_storage_subsystem(const std::string& system_uuid) const;

    /*!
     * @brief Full switch discovery
     * @param[in] fabric_uuid Uuid of the parent fabric
     * @param[in] t tools::Toolset used to perform discovery
     * @param[in] chassis_uuid Uuid of the chassis containing the switch
     * @param[in] sysfs_switch Data read from the sysfs
     * @return Discovered entity
     * */
    virtual agent_framework::model::Switch discover_switch(const std::string& fabric_uuid, const tools::Toolset& t,
        const std::string& chassis_uuid, const sysfs::SysfsSwitch& sysfs_switch) const;

    /*!
     * @brief Full zone discovery
     * @param[in] fabric_uuid Uuid of the parent fabric
     * @param[in] t tools::Toolset used to perform discovery
     * @param[in] switch_uuid Uuid of the switch
     * @param[in] gas Instance of the GAS registers used to perform discovery
     * @param[in] zone_id Id of the zone
     * @return Discovered entity
     * */
    virtual agent_framework::model::Zone discover_zone(const std::string& fabric_uuid, const tools::Toolset& t,
        const std::string& switch_uuid, gas::GlobalAddressSpaceRegisters& gas, uint8_t zone_id) const;

    /*!
     * @brief Full port discovery
     * @param[in] switch_uuid Uuid of the parent switch
     * @param[in] t tools::Toolset used to perform discovery
     * @param[in] gas Instance of the GAS registers used to perform discovery
     * @param[in] cmd PortBindingInfo command result called for all ports
     * @param[in] entry_id Id of the entry in the PortBindingInfo command result
     * @return Discovered entity
     * */
    virtual agent_framework::model::Port discover_port(const std::string& switch_uuid, const tools::Toolset& t,
        gas::GlobalAddressSpaceRegisters& gas, const gas::mrpc::PortBindingInfo& cmd, uint8_t entry_id) const;

    /*!
     * Template to update port. Speeds and status are updated.
     * @param port modified port. Raw reference is modified
     * @param gas registers to be read
     * @param tools toolset to be used
     * @return true if port was modified (any value was changed)
     */
    virtual bool update_port(agent_framework::model::Port& port,
                             const gas::GlobalAddressSpaceRegisters& gas, const tools::Toolset& tools) const;

    /*!
     * @brief Full discovery of the host/root complex endpoint_uuid
     * @param[in] fabric_uuid Uuid of the parent fabric
     * @return Discovered entity
     * */
    virtual agent_framework::model::Endpoint discover_host_endpoint(const std::string& fabric_uuid) const;

    /*!
     * @brief Full discovery of the drive endpoint_uuid
     * @param[in] fabric_uuid Uuid of the parent fabric
     * @param[in] drive_uuid Uuid of the drive
     * @return Discovered entity
     * */
    virtual agent_framework::model::Endpoint discover_drive_endpoint(const std::string& fabric_uuid,
        const std::string& drive_uuid) const;

    /*!
     * @brief Full discovery of the unknown target endpoint_uuid
     * @param[in] fabric_uuid Uuid of the parent fabric
     * @return Discovered entity
     * */
    virtual agent_framework::model::Endpoint discover_unknown_target_endpoint(const std::string& fabric_uuid) const;

    /*!
     * @brief Ib discovery of the pcie device
     * @param[in] manager_uuid Uuid of the parent manager
     * @param[in] chassis_uuid Uuid of the chassis containing the device
     * @param[in] sysfs_device Sysfs data of the device
     * @return Discovered entity
     * */
    virtual agent_framework::model::PcieDevice discover_pcie_device(const std::string& manager_uuid,
        const std::string& chassis_uuid, const sysfs::SysfsDevice& sysfs_device) const;

    /*!
     * @brief Ib discovery of the pcie function
     * @param[in] device_uuid Uuid of the parent device
     * @param[in] dsp_port_uuid Uuid of the downstream port of function
     * @param[in] sysfs_function Sysfs data of the function
     * @return Discovered entity
     * */
    virtual agent_framework::model::PcieFunction discover_pcie_function(const std::string& device_uuid,
        const std::string& dsp_port_uuid, const sysfs::SysfsFunction& sysfs_function) const;

    /*!
     * @brief Oob discovery of the drive
     * @param[in] chassis_uuid Uuid of the chassis containing the drive
     * @param[in] t Toolsed used in the discovery
     * @param[in] port_uuid Uuid of the downstream port of the drive
     * @return Discovered entity
     * */
    virtual agent_framework::model::Drive discover_oob_drive(const std::string& chassis_uuid, const tools::Toolset& t,
        const std::string& port_uuid) const;

    /*!
     * @brief Ib discovery of the drive
     * @param[in] drive Existing drive to be updated
     * @param[in] sysfs_device Sysfs data of the device
     * @param[in] sysfs_drive Sysfs data of the drive
     * @return Discovered entity
     * */
    virtual agent_framework::model::Drive discover_ib_drive(const agent_framework::model::Drive& drive,
        const sysfs::SysfsDevice& sysfs_device, const sysfs::SysfsDrive& sysfs_drive) const;

    /*!
     * @brief Ib discovery of the drive when no sysfs data are available
     * @param[in] drive Existing drive to be updated
     * @return Discovered entity
     * */
    virtual agent_framework::model::Drive discover_no_sysfs_ib_drive(const agent_framework::model::Drive& drive) const;

    /*!
     * Implementation of Chassis discovery
     * @param t tools::Toolset used to perform discovery
     * @param chassis Chassis instance to be updated
     * @return Discovered entity
     */
    virtual agent_framework::model::Chassis discover_chassis(
            const tools::Toolset& t, const agent_framework::model::Chassis& chassis) const;

protected:
    builders::BuilderFactoryPtr m_factory{};
    agent_framework::model::enums::PlatformType  m_platform;

};

using DiscovererPtr = std::shared_ptr<Discoverer>;

}
}
}
