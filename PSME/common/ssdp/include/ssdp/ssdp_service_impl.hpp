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
 * @file ssdp_service_impl.hpp
 *
 * @brief SsdpService Implementation
 * */

#pragma once

#include "ssdp/ssdp_service.hpp"
#include "ssdp/ssdp_packet_factory.hpp"
#include "ssdp/message_queue.hpp"
#include "ssdp/notification.hpp"

#include "net/network_interface.hpp"
#include "net/multicast_socket.hpp"
#include "net/paired_socket.hpp"

#include <memory>
#include <set>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <atomic>

namespace ssdp {

using net::MulticastSocket;
using net::NetworkInterface;
using net::SocketAddress;
using net::IpAddress;

class SsdpServiceConfig;

/*! SSDP Service implementation */
class SsdpServiceImpl {
public:
    /*!
     * @brief Constructor
     * @param config SsdpService configuration object
     */
    SsdpServiceImpl(std::shared_ptr<const SsdpServiceConfig> config);

    /*!
     * Destructor
     */
    virtual ~SsdpServiceImpl();

    /*!
     * @brief Starts SSDP service thread processing SSDP packets.
     */
    void start();

    /*!
     * @brief Stops SSDP service thread.
     */
    void stop();

    /*!
     * Posts notification for processing on ssdp thread.
     * @param[in] notification Notification object.
     */
    void post_notification(const Notification& notification);

private:
    friend class SsdpServiceImplTest;

    SsdpPacket receive_packet(SocketAddress& sender);

    void execute();

    void send_alive();

    void send_byebye();

    IpAddress find_lan_of_peer(const IpAddress& peer);

    void process_ssdp_request();

    void process_ssdp_m_search(const SsdpPacket& packet,
                               const SocketAddress& sender,
                               const IpAddress& lan_address);

    void init();

    bool is_running() const {
        return m_is_running;
    }

    void process_notification();
    void on_network_change(const Notification::Bytes&, const Notification::Bytes::size_type);
    void on_shutdown(const Notification::Bytes&, const Notification::Bytes::size_type);

    using NetworkInterfaceMap = std::unordered_map<unsigned, NetworkInterface>;
    void on_network_change(unsigned iface_index, net::NetworkChangeNotifier::ChangeType change_type);
    void link_update(NetworkInterfaceMap::iterator it, const NetworkInterface& iface);
    void link_insert_or_update(NetworkInterfaceMap::iterator it, const NetworkInterface& iface);
    void link_remove(NetworkInterfaceMap::iterator it);

    std::string get_debug_network_status_info() const;

    std::atomic<bool> m_is_running{false};
    net::PairedSockets m_notification_pipe; // self-pipe trick
    std::thread m_thread{};

    std::shared_ptr<const SsdpServiceConfig> m_config;
    SsdpPacketFactory m_ssdp_packet_factory;
    /*! Collection of network interfaces on which SSDP traffic is listened on */
    NetworkInterfaceMap m_network_interfaces{};
    std::vector<MulticastSocket> m_notify_sockets{};
    MulticastSocket m_listen_socket{};

    using Ms = MessageQueue<ScheduledMessage>::Ms;
    MessageQueue<ScheduledMessage> m_queue{};
};

}
