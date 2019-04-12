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
 * @file ssdp_service_impl.cpp
 *
 * @brief SsdpService implementation.
 * */

#include "ssdp/ssdp_service_impl.hpp"
#include "ssdp/ssdp_packet_parser.hpp"
#include "ssdp/ssdp_packet_validator.hpp"
#include "ssdp/uri.hpp"

#include "logger/logger_factory.hpp"

#include <algorithm>
#include <set>
#include <sstream>

using namespace ssdp;

namespace {

constexpr const char SSDP_ALL_STR[] = "ssdp:all";
constexpr const int PACKET_SIZE = 1536;

bool supports_mcast_address(const SocketAddress& ssdp_address, const NetworkInterface& iface) {
    if (AF_INET == ssdp_address.af()) {
        return iface.supports_ipv4();
    }
    else if (AF_INET6 == ssdp_address.af()) {
        return iface.supports_ipv6();
    }
    return false;
}

void leave_or_join_mcast_group(MulticastSocket& listen_socket,
                               const SocketAddress& ssdp_address,
                               const NetworkInterface& cached_iface,
                               const NetworkInterface& updated_iface) {
    auto is_joined = supports_mcast_address(ssdp_address, cached_iface);
    auto should_join = supports_mcast_address(ssdp_address, updated_iface);
    if (!is_joined && should_join) {
        listen_socket.join_group(ssdp_address.get_host(), updated_iface);
    }
    else if (is_joined && !should_join) {
        listen_socket.leave_group(ssdp_address.get_host(), cached_iface);
    }
}

MulticastSocket open_ssdp_listen_socket(std::uint16_t port) {
    return MulticastSocket(SocketAddress(port), /*reuse_address*/true);
}

MulticastSocket open_ssdp_notify_socket(unsigned char ttl,
        const NetworkInterface& iface,
        const IpAddress& address) {
    SocketAddress socket_address(address, 0);
    MulticastSocket mcast_socket(socket_address);
    mcast_socket.set_loopback(false);
    mcast_socket.set_broadcast(true);
    mcast_socket.set_blocking(false);
    mcast_socket.set_interface(iface);
    mcast_socket.set_ttl(ttl);
    return mcast_socket;
}

constexpr unsigned long MAX_MX = 5;
constexpr uint DELAY_INCREMENT = 50;
constexpr uint SCHEDULED_DELAY = 250;

std::chrono::milliseconds calculate_delay(const std::string& mx_str) {
    unsigned long mx{1};
    try {
        auto mx_val = std::stoul(mx_str);
        if (mx_val > 0 && mx_val < MAX_MX) {
            mx = mx_val;
        }
    }
    catch(...) {/*ignore*/}

    return std::chrono::milliseconds(std::rand() / (1 + RAND_MAX / (1000 * mx)));
}

std::chrono::milliseconds get_check_interval(const SsdpServiceConfig& config) {
    if (config.get_announce_interval() > std::chrono::seconds(0)) {
        return config.get_announce_interval();
    }
    return std::chrono::minutes(30);
}

bool is_iface_ignored(const std::unordered_set<std::string>& iface_names, const std::string& name) {
    return iface_names.cend() == iface_names.find(name);
}

std::set<IpAddress> get_addresses_for_mcast(const SocketAddress& mcast_address, const NetworkInterface& iface) {
    std::set<IpAddress> addresses{};
    for (const auto& address : iface.get_address_list()) {
        const auto& ip_address = std::get<net::NetworkInterface::IP_ADDRESS>(address);
        if (mcast_address.family() == ip_address.get_address_family()) {
            addresses.emplace(ip_address);
        }
    }
    return addresses;
}

std::tuple<std::set<IpAddress>, std::set<IpAddress>> addresses_difference(
                const NetworkInterface& cached, const NetworkInterface& updated) {
    auto cached_addresses = get_addresses_for_mcast(SsdpService::SSDP_MCAST_ADDRESS, cached);
    auto updated_addresses = get_addresses_for_mcast(SsdpService::SSDP_MCAST_ADDRESS, updated);
    std::set<IpAddress> added{};
    std::set_difference(updated_addresses.begin(), updated_addresses.end(),
            cached_addresses.begin(), cached_addresses.end(),
            std::inserter(added, added.begin()));
    std::set<IpAddress> removed{};
    std::set_difference(cached_addresses.begin(), cached_addresses.end(),
            updated_addresses.begin(), updated_addresses.end(),
            std::inserter(removed, removed.begin()));
    return std::make_tuple(std::move(added), std::move(removed));
}

void update_mcast_notify_sockets_list(std::vector<MulticastSocket>& notify_sockets,
                                      const NetworkInterface& cached_iface,
                                      const NetworkInterface& updated_iface,
                                      unsigned char ttl) {
    const auto diff = addresses_difference(cached_iface, updated_iface);
    const auto& removed_addresses = std::get<1>(diff);
    notify_sockets.erase(std::remove_if(notify_sockets.begin(), notify_sockets.end(),
            [&removed_addresses](MulticastSocket & socket) {
                return removed_addresses.cend() != removed_addresses.find(socket.get_address().get_host());
            }
    ), notify_sockets.end());
    const auto& new_addresses = std::get<0>(diff);
    for (const auto& address : new_addresses) {
        notify_sockets.emplace_back(open_ssdp_notify_socket(ttl, updated_iface, address));
    }
}

}

SsdpServiceImpl::SsdpServiceImpl(std::shared_ptr<const SsdpServiceConfig> config)
    : m_config(config),
      m_ssdp_packet_factory(m_config),
      m_notification_pipe(net::PairedSocket::create_socket_pair()) {}

SsdpServiceImpl::~SsdpServiceImpl() {
    stop();
}

namespace {
bool same_subnet(const IpAddress& peer, const IpAddress& nic_ip, const IpAddress& mask) {
    return (peer & mask) == (nic_ip & mask);
}
}

IpAddress SsdpServiceImpl::find_lan_of_peer(const IpAddress& peer) {
    for (const auto& iface : m_network_interfaces) {
        for (const auto& address : iface.second.get_address_list()) {
            const auto& iface_ip = std::get<NetworkInterface::IP_ADDRESS>(address);
            const auto& iface_subnet_mask = std::get<NetworkInterface::SUBNET_MASK>(address);
            if (peer.get_address_family() == iface_ip.get_address_family()
                && same_subnet(peer, iface_ip, iface_subnet_mask)) {
                return iface_ip;
            }
        }
    }
    return IpAddress();
}

void SsdpServiceImpl::process_ssdp_m_search(const SsdpPacket& packet,
                                            const SocketAddress& sender,
                                            const IpAddress& lan_address) {
    const auto& st_header = packet.get_header(SsdpPacket::ST);

    if (st_header == SSDP_ALL_STR || st_header == m_config->get_service_urn()) {
        auto delay = calculate_delay(packet.get_header(SsdpPacket::MX));
        auto search_response = m_ssdp_packet_factory.create_ssdp_packet(
                                            SsdpPacket::Type::SEARCH_RESPONSE);
        URI service_uri{m_config->get_service_url()};
        service_uri.set_host(lan_address.to_string());
        search_response.set_header(SsdpPacket::AL, service_uri.to_string());
        ScheduledMessage msg(m_listen_socket, sender,
                std::make_shared<const std::string>(search_response.to_string()));
        m_queue.send_or_enqueue(msg, delay);
        m_queue.send_or_enqueue(msg, delay + Ms(DELAY_INCREMENT));
    }
    else {
        log_debug("ssdp",
                "ignoring SSDP " << packet.get_type().to_string() << " packet.");
    }
}

void SsdpServiceImpl::send_alive() {

    if (!m_config->is_announcement_enabled() || m_notify_sockets.empty()) {
        return;
    }

    log_debug("ssdp", "sending ssdp notifies");

    auto ssdp_alive_packet = m_ssdp_packet_factory.create_ssdp_packet(
                                                     SsdpPacket::Type::NOTIFY);
    URI service_uri{ssdp_alive_packet.get_header(SsdpPacket::AL)};

    for (auto& sock: m_notify_sockets) {
        service_uri.set_host(sock.get_address().get_host().to_string());
        ssdp_alive_packet.set_header(SsdpPacket::AL, service_uri.to_string());
        ScheduledMessage msg(sock, SsdpService::SSDP_MCAST_ADDRESS,
                std::make_shared<const std::string>(ssdp_alive_packet.to_string()));
        m_queue.send_or_enqueue(msg);
        m_queue.send_or_enqueue(msg, Ms(SCHEDULED_DELAY));
    }
}

void SsdpServiceImpl::send_byebye() {

    if (!m_config->is_announcement_enabled() || m_notify_sockets.empty()) {
        return;
    }

    log_info("ssdp", "sending ssdp byebye");

    try {
        auto bye_payload = std::make_shared<const std::string>(m_ssdp_packet_factory.create_ssdp_packet(
                SsdpPacketKey{SsdpPacket::Type::NOTIFY,
            SsdpPacket::SSDP_BYEBYE_STR}).to_string());

        for (auto& sock : m_notify_sockets) {
            ScheduledMessage msg(sock, SsdpService::SSDP_MCAST_ADDRESS, bye_payload);
            msg.try_send();
        }

        std::this_thread::sleep_for(Ms(SCHEDULED_DELAY));

        for (auto& sock : m_notify_sockets) {
            ScheduledMessage msg(sock, SsdpService::SSDP_MCAST_ADDRESS, bye_payload);
            msg.try_send();
        }
    }
    catch (const std::exception& e) {
        log_error("ssdp", "Failed to send byebye: " << e.what()
                << "\n" << get_debug_network_status_info());
    }
    catch (...) {
        log_error("ssdp", "Failed to send byebye.\n"
                << get_debug_network_status_info());
    }
}

void SsdpServiceImpl::process_ssdp_request() {
    try {
        SocketAddress sender;
        auto packet = receive_packet(sender);

        log_debug("ssdp", "from:" << sender << " packet:\n"
                << packet.to_string());

        auto lan_address = find_lan_of_peer(sender.get_host());
        if (lan_address.is_any_address()) {
            log_debug("ssdp",
                            "ignoring packet on non SSDP enabled interface.");
            return;
        }

        if (!validate_ssdp_packet(packet)) {
            log_debug("ssdp", "ignoring invalid SSDP packet.");
            return;
        }

        if (!packet.is_search_request()) {
            log_debug("ssdp",
                "ignoring SSDP " << packet.get_type().to_string() << " packet.");
            return;
        }

        process_ssdp_m_search(packet, sender, lan_address);
    }
    catch (std::exception e) {
        log_error("ssdp", e.what());
    }
    catch (...) {
        log_error("ssdp", "Unknown exception :(");
    }
}

SsdpPacket SsdpServiceImpl::receive_packet(SocketAddress& sender) {
    static std::vector<char> buffer(PACKET_SIZE);
    std::fill(buffer.begin(), buffer.end(), 0);

    auto n = m_listen_socket.receive_from(buffer.data(), buffer.size(), sender);
    SsdpPacketParser<decltype(buffer)> parser(buffer, n);
    return parser.parse();
}

void SsdpServiceImpl::init() {
    m_listen_socket = open_ssdp_listen_socket(
                            SsdpService::SSDP_MCAST_ADDRESS.get_port());

    try {
        const auto iface_list = NetworkInterface::get_interfaces();
        const auto& iface_names = m_config->get_nic_names();

        for (const auto& iface : iface_list) {
            if (!is_iface_ignored(iface_names, iface.get_name())
                    && iface.is_up()
                    && iface.is_running()) {
                try {
                    link_insert_or_update(m_network_interfaces.end(), iface);
                }
                catch (const std::exception& e) {
                    log_error("ssdp", "SSDP configuration on network interface "
                            << iface.get_name() << " failed: " << e.what());
                }
            }
        }
    }
    catch (const std::exception& e) {
        log_error("ssdp", "SSDP network configuration error: " << e.what());
    }

    log_info("ssdp", get_debug_network_status_info());

    std::srand(uint(std::time(0)));
}

void SsdpServiceImpl::start() {
    if (!m_config->is_ssdp_service_enabled()) {
        log_info("ssdp", "SSDP service is disabled.");
        return;
    }

    if (!is_running()) {
        m_is_running = true;
        m_thread = std::thread(&SsdpServiceImpl::execute, this);
    }
}

void SsdpServiceImpl::stop() {
    if (is_running()) {
        post_notification(ShutDownNotification{});
        m_is_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }
}

void SsdpServiceImpl::link_update(NetworkInterfaceMap::iterator it, const NetworkInterface& updated_iface) {
    if (it != m_network_interfaces.end()) {
        auto& cached_iface = it->second;

        if (m_config->is_announcement_enabled()) {
            const auto diff = addresses_difference(cached_iface, updated_iface);
            update_mcast_notify_sockets_list(m_notify_sockets, cached_iface, updated_iface, m_config->get_socket_ttl());
        }
        leave_or_join_mcast_group(m_listen_socket, SsdpService::SSDP_MCAST_ADDRESS, cached_iface, updated_iface);

        // update cache
        cached_iface = updated_iface;
    }
}

void SsdpServiceImpl::link_remove(NetworkInterfaceMap::iterator it) {
    if (it != m_network_interfaces.end()) {
        NetworkInterface empty;
        link_update(it, empty);
        m_network_interfaces.erase(it);
        log_info("ssdp", get_debug_network_status_info());
    }
}

void SsdpServiceImpl::link_insert_or_update(NetworkInterfaceMap::iterator it,
                                            const NetworkInterface& updated_iface) {
    if (it == m_network_interfaces.end()) {
        it = m_network_interfaces.emplace(updated_iface.get_index(), NetworkInterface{}).first;
    }
    link_update(it, updated_iface);
    log_info("ssdp", get_debug_network_status_info());
}

void SsdpServiceImpl::on_network_change(unsigned iface_index,
                                        net::NetworkChangeNotifier::ChangeType) {

    auto cached_iface = m_network_interfaces.find(iface_index);
    const auto ifaces = NetworkInterface::get_interfaces_map();
    auto ifaces_iterator = ifaces.find(iface_index);
    if (ifaces_iterator == ifaces.cend()) {
        link_remove(cached_iface);
    }
    else {
        const auto& updated_iface = ifaces_iterator->second;
        if (is_iface_ignored(m_config->get_nic_names(), updated_iface.get_name())) {
            return;
        }

        if (!(updated_iface.is_up() && updated_iface.is_running())) {
            link_remove(cached_iface);
        }
        else {
            link_insert_or_update(cached_iface, updated_iface);
        }
    }
}

void SsdpServiceImpl::post_notification(const Notification& notification) {
    if (is_running()) {
        auto bytes = notification.serialize();
        m_notification_pipe.first.send_bytes(bytes.data(), bytes.size());
    }
}

void SsdpServiceImpl::process_notification() {
    Notification::Bytes bytes(Notification::MAX_SERIALIZED_SIZE);
    auto n = m_notification_pipe.second.receive_bytes(bytes.data(), bytes.size());

    if(bytes.empty()) {
        log_error("ssdp", "Received empty notification");
        return;
    }

    const auto notification_type = static_cast<Notification::Type>(bytes[0]);
    switch (notification_type) {
    case Notification::Type::SHUT_DOWN:
        on_shutdown(bytes, n);
        break;
    case Notification::Type::NETWORK_CHANGE:
        on_network_change(bytes, n);
        break;
    default:
        log_error("ssdp", "Received unknown notification type:"
                << static_cast<int>(notification_type));
        break;
    }
}

void SsdpServiceImpl::on_network_change(const Notification::Bytes& bytes,
                                        const Notification::Bytes::size_type bytes_length) {
    NetworkChangeNotification notification;
    notification.deserialize(bytes, bytes_length);
    on_network_change(notification.get_iface_index(), notification.get_change_type());
}

void SsdpServiceImpl::on_shutdown(const Notification::Bytes& bytes,
                                  const Notification::Bytes::size_type bytes_length) {
    ShutDownNotification notification;
    notification.deserialize(bytes, bytes_length);
    m_is_running = false;
}

void SsdpServiceImpl::execute() {

    try {
        init();
        log_info("ssdp", "SSDP service started.");
    }
    catch (...) {
        log_error("ssdp", "Failed to initialize SSDP service.");
        return;
    }

    Socket::List readable{}, writable{}, except{};

    const auto check_interval = get_check_interval(*m_config);
    Ms timeout = check_interval;
    auto last_announce_time =
        std::chrono::time_point_cast<Ms>(std::chrono::steady_clock::now())
            - check_interval;

    auto& notification_socket = m_notification_pipe.second;
    while (is_running()) {
        try {
            readable.clear();
            writable.clear();
            except.clear();

            timeout = check_interval;
            auto now = std::chrono::time_point_cast<Ms>(std::chrono::steady_clock::now());
            auto elapsed = now - last_announce_time;
            if (elapsed >= check_interval) {
                send_alive();
                last_announce_time += elapsed;
            }
            else {
                timeout -= elapsed;
            }

            m_queue.select_ready_messages(writable, now, timeout);
            readable.push_back(m_listen_socket);
            readable.push_back(notification_socket);

            if (Socket::select(readable, writable, except, timeout)) {
                for (const auto& socket : readable) {
                    if (socket == m_listen_socket) {
                        process_ssdp_request();
                    }
                    else if (socket == notification_socket) {
                        process_notification();
                    }
                }
            }
            m_queue.send_ready_messages(writable);
        }
        catch (const std::exception& e) {
            log_error("ssdp", "Exception: " << e.what()
                    << "\n" << get_debug_network_status_info());
        }
        catch (...) {
            log_error("ssdp", "Unknown exception :("
                    << "\n" << get_debug_network_status_info());
        }
    }

    send_byebye();
    log_info("ssdp", "SSDP service stopped.");
}

std::string SsdpServiceImpl::get_debug_network_status_info() const {
    std::ostringstream ostream;
    ostream << "SSDP active on interfaces:";
    auto it = m_network_interfaces.cbegin();
    for (; it != m_network_interfaces.cend(); ++it) {
        ostream << "\n     " << it->second.get_name()
                << " #" << it->second.get_index();
        for (const auto& address : it->second.get_address_list()) {
            ostream << "\n      " << std::get<net::NetworkInterface::IP_ADDRESS>(address);
        }
    }
    ostream << "\n     open notification sockets:\n";
    for (auto socket = m_notify_sockets.cbegin(); socket != m_notify_sockets.cend(); ++socket) {
        ostream << "      " << socket->get_address() << "\n";
    }
    return ostream.str();
}
