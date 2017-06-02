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
 * @file discovery_manager.hpp
 *
 * @brief Initial discovery implementation.
 * */

#pragma once
#include "agent-framework/discovery/discovery.hpp"
#include "sysfs/sysfs_dto.hpp"
#include "sysfs/sysfs_decoder.hpp"
#include "gas/global_address_space_registers.hpp"
#include "tools/toolset.hpp"
#include "discovery/discoverer.hpp"

#include <memory>

namespace agent {
namespace pnc {
namespace discovery {

/*!
 * @brief Implementation of initial discovery. Gets component id, translates it
 * to IP and port number. After that performs discovery of the module.
 */
class DiscoveryManager final : public ::agent_framework::discovery::Discovery {

public:

    /*!
     * @brief Default constructor.
     * @param[in] h Discovery helper to be used
     * @param[in] t Toolset to be used
     */
    DiscoveryManager(DiscovererPtr h, const tools::Toolset& t): m_tools(t), m_discoverer(h) {}

    /*!
     * Enable copy
     */
    DiscoveryManager(const DiscoveryManager&) = default;

    /*!
     * @brief Default destructor.
     */
    ~DiscoveryManager();

    /*!
     * @brief Constructs a discovery manager for the platform
     * @param[in] t Toolset used to get manager
     * @return Built discovery manager
     * */
    static DiscoveryManager create(const tools::Toolset& t);

    /*!
     * @brief Discovery procedure
     * */
    void discovery(const std::string&) override;

    /*!
     * @brief Performs out-of-band discovery of resources on a specific port
     * @param[in] gas Instance of GAS registers
     * @param[in] switch_uuid Uuid of the Pcie Switch
     * @param[in] dsp_port_uuid Uuid of the Downstream Port
     * @return True if discovery was successful
     * */
    bool oob_port_device_discovery(const gas::GlobalAddressSpaceRegisters& gas, const std::string& switch_uuid,
        const std::string& dsp_port_uuid) const;

    /*!
     * @brief Performs in-band discovery of resources on a specific port, visible under specific sysfs logical bridge
     * @param[in] switch_uuid Uuid of the Pcie Switch
     * @param[in] dsp_port_uuid Uuid of the Downstream Port
     * @param[in] bridge_id Id of the bridge that should be used during discovery (as visible on pcie switch)
     * @param[in] drive_uuid Uuid of the drive created during out-of-band discovery
     * @return True if discovery was successful
     * */
    bool ib_port_device_discovery(const std::string& switch_uuid, const std::string& dsp_port_uuid,
            uint8_t bridge_id, const std::string& drive_uuid) const;

    /*!
     * @brief Function reads smart for the drive and updates its status
     * @param[in] port_uuid Uuid of the physical dsp port of the drive
     * @param[in] drive_uuid Uuid of the drive whose status is to be updated
     * @return True if update was successful
     * */
    bool update_drive_status(const std::string& port_uuid, const std::string& drive_uuid)const;

    /*!
     * @brief Function updates port status and width/speed of the link
     * @param[in] gas Instance of GAS registers
     * @param[in] port_uuid Uuid of the physical port to be updated
     * @return True if update was successful
     * */
    bool update_port_status(const gas::GlobalAddressSpaceRegisters& gas, const std::string& port_uuid) const;

    /*!
     * @brief Handles removal of the resources on a specific port
     * @param[in] gas Instance of GAS registers
     * @param[in] port_uuid Uuid of the port
     * @return True if removal was successful
     * */
    bool remove_devices_on_port(const gas::GlobalAddressSpaceRegisters& gas, const std::string& port_uuid) const;

private:
    tools::Toolset m_tools{};
    DiscovererPtr m_discoverer{};

    /*! Discovers all zones on a specified switch */
    void discover_zones(const std::string& fabric_uuid, const std::string& switch_uuid,
        gas::GlobalAddressSpaceRegisters& gas) const;

    /*! Discovers all ports on a specified switch */
    void discover_ports(const std::string& fabric_uuid, const std::string& switch_uuid,
        gas::GlobalAddressSpaceRegisters& gas) const;

    /*! Removes list of pcie functions & pcie devices, deletes links, logs it and sends events */
    void remove_pcie_devices_by_function_uuids(const std::vector<std::string>& function_uuids) const;

    /*! Removes list of drives, deletes links, logs it and sends events */
    void remove_drives_by_uuids(const std::vector<std::string>& drive_uuids) const;

    /*! Degenerates endpoints by port uuid and list of drives, deletes links, logs it and sends events */
    void degenerate_endpoints_by_drive_uuids(const gas::GlobalAddressSpaceRegisters& gas,
        const std::vector<std::string>& drive_uuids) const;

    /*! Handles new drives detected on a port, adds to model, adds links, sends events */
    std::string add_and_stabilize_drive(const agent_framework::model::Drive& drive,
        const agent_framework::model::Port& port) const;

    /*! Handles new endpoints detected on a port, adds to model, adds links, sends events, regenerates if needed */
    std::string add_and_stabilize_endpoint(agent_framework::model::Endpoint& endpoint,
        const agent_framework::model::Port& port, bool was_drive_found, const std::string& drive_uuid) const;

    /*! Creates a link between a newly detected endpoint and its zone */
    void update_endpoint_zone_binding(const gas::GlobalAddressSpaceRegisters& gas,
        const std::string& endpoint_uuid, const agent_framework::model::Port& port) const;

    /*! Discovers pcie devices/functions based on the sysfs data */
    void sysfs_device_discovery(const std::string& dsp_port_uuid, const std::string drive_uuid,
        const sysfs::SysfsDecoder& decoder, const sysfs::SysfsDevice& sysfs_device) const;

    /*! Discovers ib drive data based on the sysfs data */
    void sysfs_drive_discovery(const std::string drive_uuid, const sysfs::SysfsDecoder& decoder,
        const sysfs::SysfsDevice& sysfs_device) const;

    /*! Performs critical state drive discovery (oob drive found but no sysfs data) */
    void critical_state_drive_discovery(const std::string& drive_uuid) const;

};

}
}
}
