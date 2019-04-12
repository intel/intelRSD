/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file nvmf-discovery/connection_listener.hpp
 */

#pragma once

#include "fabric.hpp"

namespace nvmf_discovery {

/*! forward declaration */
struct Interface;

/*!
 * Represents passive fabric endpoint listening for incoming connections.
 */
class ConnectionListener final {
public:
    /*!
     * Constructor
     *
     * @param provider fi_info
     */
    ConnectionListener(FiInfo provider);

    /*!
     * Creates passive endpoint and transitions
     * it to state accepting incoming connection requests.
     */
    void listen();

    /*!
     * Waits for connection event within specified time.
     * @param timeout Event wait timeout in milliseconds
     * @return FiInfo containing connection request.
     * If no connection request arrive within specified time nullptr is returned.
     */
    FiInfo wait_for_connection_event(TimeoutMs timeout) noexcept;

    /*!
     * @return Fabric access domain
     */
    fid_domain& get_access_domain() {
        return *m_domain;
    }

    /*!
     * @return Fabric domain
     */
    fid_fabric& get_fabric_domain() {
        return *m_fabric;
    }

private:
    FiInfo m_provider{};
    FidFabric m_fabric{};
    FidDomain m_domain{};
    FidPep m_pep{};
    FidEq m_peq{};
};

/*!
 * Obtains fabric interface information structure
 *
 * @param iface Structure specifying OFI provider, transport type,
 * name or fabric address to resolve, service name or port number of address
 * @return FiInfo fabric interface information structure
 */
FiInfo obtain_connection_listener_provider(const Interface& iface);

}
