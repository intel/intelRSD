/*!
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
 *
 * @file ssdp_service.hpp
 *
 * @brief Declaration of SsdpService.
 * */

#pragma once

#include "ssdp_service_config.hpp"
#include "net/socket_address.hpp"
#include "net/network_change_notifier.hpp"

#include <memory>

namespace ssdp {

class SsdpServiceImpl;

/*!
 * @brief Main class providing SSDP service functionality.
 *
 * Starts thread listening for SSDP datagrams on specified interfaces.
 * Responds on M-SEARCH messages and announces (if via configuration enabled)
 * availability of rest service.
 */
class SsdpService final : public net::NetworkChangeNotifier::NetworkChangeListener {
public:
    /*! SSDP Multicast address */
    static net::SocketAddress SSDP_MCAST_ADDRESS;

    /*!
     * @brief Constructor.
     * @param config Service configuration object.
     */
    SsdpService(const SsdpServiceConfig& config);

    /*!
     * @brief Destructor
     */
    ~SsdpService();

    /*!
     * @brief Starts SSDP service thread processing SSDP packets.
     */
    void start();

    /*!
     * @brief Stops SSDP service thread.
     */
    void stop();

    void on_network_change(unsigned iface_index,
                net::NetworkChangeNotifier::ChangeType change_type) override;

private:
    std::unique_ptr<SsdpServiceImpl> m_impl;
};

}
