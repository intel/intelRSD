#include "net/network_interface.hpp"
#include "net/socket_address.hpp"

#include <iostream>

int main() {

    const auto ifaces = net::NetworkInterface::get_interfaces();

    for (const auto& iface: ifaces) {
        std::string sep("");
        std::cout << iface.get_name() << " [" << iface.get_index() << "]: ";
        std::cout << "<";
        if (iface.is_up()) {
            std::cout << sep << "UP";
            sep = ",";
        }

        if (iface.is_running()) {
            std::cout << sep << "RUNNING";
            sep = ",";
        }

        if (iface.is_loopback()) {
            std::cout << sep << "LOOPBACK";
            sep = ",";
        }

        if (iface.is_p2p()) {
            std::cout << sep << "P2P";
            sep = ",";
        }

        if (iface.supports_ipv4()) {
            std::cout << sep << "IPv4";
            sep = ",";
        }

        if (iface.supports_ipv6()) {
            std::cout << sep << "IPv6";
            sep = ",";
        }

        if (iface.supports_broadcast()) {
            std::cout << sep << "BCAST";
            sep = ",";
        }

        if (iface.supports_multicast()) {
            std::cout << sep << "MCAST";
            sep = ",";
        }

        if (!iface.is_loopback()) {
            std::cout << sep << std::dec << iface.get_mtu();
            sep = ",";
        }

        std::cout << ">" << std::endl;

        if (!iface.is_loopback()) {
            std::cout << "  HWaddr " << iface.get_hw_address() << std::endl;
        }

        for (const auto& address : iface.get_address_list()) {
            std::cout << "  " << std::get<net::NetworkInterface::IP_ADDRESS>(address);
            auto addr = std::get<net::NetworkInterface::SUBNET_MASK>(address);
            if (!addr.is_any_address()) {
                std::cout << '/' << addr << " (" << addr.prefix_length() << ')';
            }
            addr = std::get<net::NetworkInterface::BROADCAST_ADDRESS>(address);
            if (!addr.is_any_address()) {
                std::cout << (iface.is_p2p() ? "  dest  " : "  bcast  ") << addr;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    return 0;
}
