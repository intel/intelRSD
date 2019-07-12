/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file state_machine/port_state_worker.hpp
 * @brief Class responsible for performing actions during state machine transitions
 * */

#pragma once



#include "tools/toolset.hpp"
#include "discovery/discovery_manager.hpp"

#include <memory>



namespace agent {
namespace pnc {
namespace state_machine {

/*! Class handling the PCIe device discovery process */
class PortStateWorker {
public:

    /*!
     * @brief Default Constructor
     * @param[in] dm Discovery manager that will be used for all discovery tasks
     * @param[in] t Toolset that will be used for other tasks
     * */
    PortStateWorker(const discovery::DiscoveryManager& dm, const tools::Toolset& t) : m_dm(dm), m_tools(t) {}


    /*! Enable copy */
    PortStateWorker(const PortStateWorker& rhs) = default;


    /*! Destructor */
    virtual ~PortStateWorker();


    /*!
     * @brief Gets id of the bridge for a specific port_uuid
     * @param[in] switch_uuid Uuid of the switch_uuid
     * @param[in] port_uuid Uuid of the queried port
     * @return Id of the bridge
     * */
    virtual uint8_t get_bridge_id(const std::string& switch_uuid, const std::string& port_uuid) const;


    /*!
     * @brief Performs inband discovery on a specific bridge
     * @param[in] switch_uuid Uuid of the switch_uuid
     * @param[in] port_uuid Uuid of the port
     * @param[in] bridge_id Id of the logical bridge
     * @param[in] drive_uuid Uuid of the existing device after oob discovery
     * */
    virtual void ib_discovery(const std::string& switch_uuid, const std::string& port_uuid, uint8_t bridge_id,
                              const std::string& drive_uuid) const;


    /*!
     * @brief Binds port to the management host
     * @param[in] switch_uuid Uuid of the switch_uuid
     * @param[in] port_uuid Uuid of the port
     * @return Id of the logical bridge that port was bound to
     * */
    virtual uint8_t bind_to_host(const std::string& switch_uuid, const std::string& port_uuid) const;


    /*!
     * @brief Unbinds device on a specific logical bridge from the management host
     * @param[in] switch_uuid Uuid of the switch_uuid
     * @param[in] bridge_id Logical bridge id
     * */
    virtual void unbind_from_host(const std::string& switch_uuid, uint8_t bridge_id) const;


    /*!
     * @brief Performs full discovery on a port
     * @param[in] switch_uuid Uuid of the switch_uuid
     * @param[in] port_uuid Uuid of the port
     * @param[in] bridge_id Logical bridge id
     * */
    virtual void full_discovery(const std::string& switch_uuid, const std::string& port_uuid,
                                uint8_t bridge_id) const;


    /*!
     * @brief Performs oob discovery on a port
     * @param[in] switch_uuid Uuid of the switch_uuid
     * @param[in] port_uuid Uuid of the port
     * @return Discovered device uuid
     * */
    virtual std::string oob_discovery(const std::string& switch_uuid, const std::string& port_uuid) const;


    /*!
     * @brief Removes resources that were visible on a port
     * @param[in] switch_uuid Uuid of the switch_uuid
     * @param[in] port_uuid Uuid of the port
     * */
    virtual void remove(const std::string& switch_uuid, const std::string& port_uuid) const;


    /*!
     * @brief Finds device by its dsp port uuid
     * @param[in] port_uuid Uuid of the dsp port
     * @return UUID of device connected to dsp port specified by port_uuid
     * */
    virtual Uuid get_device_uuid_by_dsp_port(const Uuid& port_uuid) const;


    /*!
     * @brief Locks all entities on the port so they will not be changed by GAMI commands
     * @param[in] port_uuid Uuid of the dsp port
     * */
    virtual void lock_port(const Uuid& port_uuid) const;


    /*!
     * @brief Unlocks all entities on the port
     * @param[in] port_uuid Uuid of the dsp port
     * */
    virtual void unlock_port(const Uuid& port_uuid) const;


    using ZoneEndpointPair = std::tuple<std::string, std::string>;
    using ZoneEndpointVector = std::vector<ZoneEndpointPair>;


    /*!
     * @brief Gets list of all bindings on the specified port
     * @param port_uuid Uuid of the dsp port
     * @return Vector of zone/endpoint uuids pairs
     * */
    virtual ZoneEndpointVector get_bindings_on_port(const std::string& port_uuid) const;


    /*!
     * @brief Gets list of all endpoints on the specified port
     * @param port_uuid Uuid of the dsp port
     * @return List of uuids of all endpoints on port
     * */
    virtual std::vector<std::string> get_enabled_endpoints_on_port(const std::string& port_uuid) const;


    /*!
     * @brief Attaches endpoint to the zone
     * @param switch_uuid Uuid of the switch
     * @param endpoint_uuid Uuid of the endpoint
     * @param zone_uuid Uuid of the zone
     */
    virtual void attach_endpoint_to_zone(const std::string& switch_uuid, const std::string& endpoint_uuid,
                                         const std::string& zone_uuid) const;


    /*!
     * @brief Unbinds device on specific port from non-management host
     * @param switch_uuid Uuid of the switch
     * @param port_uuid Uuid of the dsp port
     */
    virtual void unbind_port(const Uuid& switch_uuid, const Uuid& port_uuid);


    /*!
     * @brief Checks if dsp port is included in any valid zone
     * @param port_uuid Uuid of the dsp port
     * @return true if port is included in any valid zone, false otherwise
     */
    bool is_port_in_valid_zone(const Uuid& port_uuid);


private:
    discovery::DiscoveryManager m_dm;
    tools::Toolset m_tools;

};

using PortStateWorkerPtr = std::shared_ptr<PortStateWorker>;

}
}
}
