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

/*! Class handling the drive discovery process */
class PortStateWorker {
public:

    /*!
     * @brief Default Constructor
     * @param[in] dm Discovery manager that will be used for all discovery tasks
     * @param[in] t Toolset that will be used for other tasks
     * */
    PortStateWorker(const discovery::DiscoveryManager& dm, const tools::Toolset& t): m_dm(dm), m_tools(t) {}

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
     * @brief finds drive by its dsp port uuid
     * @param[in] port_uuid Uuid of the dsp port
     * */
    virtual std::string get_drive_by_dsp_port(const std::string& port_uuid) const;

private:
    discovery::DiscoveryManager m_dm;
    tools::Toolset m_tools;

};

using PortStateWorkerPtr = std::shared_ptr<PortStateWorker>;

}
}
}
