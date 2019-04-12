/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file rmm/net_reader/net_reader.cpp
 */

#include "net_reader/net_reader.hpp"

#include "net/network_interface.hpp"
#include "logger/logger_factory.hpp"

#include <sstream>

using namespace agent::rmm::net_reader;
using namespace ::sysfs;

namespace {

constexpr char PATH_CLASS_NET[] = "/sys/class/net";
constexpr char ATTRIBUTE_SPEED[] = "speed";
constexpr char ATTRIBUTE_DUPLEX[] = "duplex";
constexpr char ATTRIBUTE_MAC_ADDRESS_TYPE[] = "addr_assign_type";
constexpr char FULL_DUPLEX[] = "full";
constexpr char HALF_DUPLEX[] = "half";
constexpr char PERMANENT_MAC_ADDRESS[] = "0";

std::vector<std::string> split_string(const std::string& str, char split) {
    std::vector<std::string> res{};
    std::istringstream sstr{str};
    std::string line{};
    while (std::getline(sstr, line, split)) {
        res.push_back(line);
    }
    return res;
}

void trim_white_characters(std::string& str) {
    std::stringstream stream;
    stream << str;
    stream.clear();
    stream >> str;
}


std::string convert_hw_address_to_string(const net::NetworkInterface::HWAddress& hw) {
    std::stringstream sstream{};
    sstream << hw;
    return sstream.str();
}

bool is_mac_permanent(const std::string& mac_source_str) {
    std::string mac_source{mac_source_str};
    trim_white_characters(mac_source);

    if (PERMANENT_MAC_ADDRESS == mac_source) {
        return true;
    }
    else {
        return false;
    }
}

OptionalField<bool> convert_duplex_to_optional_bool(const std::string& duplex_str) {
    std::string duplex = duplex_str;
    trim_white_characters(duplex);
    if (duplex == FULL_DUPLEX) {
        return true;
    }
    if (duplex == HALF_DUPLEX) {
        return false;
    }
    return OptionalField<bool>{};
}

void fill_address_list(RawNetworkInterface& raw_interface, const net::NetworkInterface& interface) {
    for (const auto& address : interface.get_address_list()) {
        auto ip_address = std::get<net::NetworkInterface::AddressType::IP_ADDRESS>(address);
        if (ip_address.get_address_family() == net::AddressFamily::IPv4) {
            RawIpAddress raw_address{};
            raw_address.ip = ip_address.to_string();
            raw_address.netmask = std::get<net::NetworkInterface::AddressType::SUBNET_MASK>(address).to_string();
            raw_interface.addresses.emplace_back(std::move(raw_address));
        }

    }
}

void update_interface(RawNetworkInterface& raw_interface, bool has_vlan, uint16_t vlan_id,
    const net::NetworkInterface& interface) {

    if (has_vlan) {
        raw_interface.vlans.push_back(vlan_id);
    }
    if (!has_vlan) {
        /* Always take data from the 'parent' interface with no vlans */
        raw_interface.mtu = interface.get_mtu();
        raw_interface.mac_address = convert_hw_address_to_string(interface.get_hw_address());
    }
    fill_address_list(raw_interface, interface);


}

RawNetworkInterface create_interface(const std::string& name, bool has_vlan, uint16_t vlan_id,
        const net::NetworkInterface& interface) {

    RawNetworkInterface raw_interface{};
    raw_interface.name = name;
    raw_interface.mtu = interface.get_mtu();
    raw_interface.mac_address = convert_hw_address_to_string(interface.get_hw_address());
    if (has_vlan) {
        raw_interface.vlans.push_back(vlan_id);
    }
    fill_address_list(raw_interface, interface);
    return raw_interface;
}

void add_or_update_interface(std::vector<RawNetworkInterface>& raw_interfaces, const std::string& name, bool has_vlan,
        uint16_t vlan_id, const net::NetworkInterface& interface) {

    bool found = false;
    for (auto& raw_interface : raw_interfaces) {
        if (raw_interface.name == name) {
            update_interface(raw_interface, has_vlan, vlan_id, interface);
            found = true;
            continue;
        }
    }
    if (!found) {
        raw_interfaces.emplace_back(create_interface(name, has_vlan, vlan_id, interface));
    }
}

std::vector<RawNetworkInterface> process_net_data() {
    auto interfaces = net::NetworkInterface::get_interfaces();
    std::vector<RawNetworkInterface> raw_interfaces{};
    for (const auto& interface : interfaces) {

        auto namevlan = split_string(interface.get_name(), '.');

        std::string name{};
        uint16_t vlan_id{};
        bool has_vlan{false};

        if (namevlan.size() < 1 || interface.is_loopback()) {
            continue;
        }
        if (namevlan.size() >= 1) {
            name = namevlan[0];
        }
        if (namevlan.size() >= 2) {
            try {
                vlan_id = uint16_t(std::stoi(namevlan[1]));
                has_vlan = true;
            }
            catch (...) {
                log_error("net-reader", "Invalid interface name");
                log_debug("net-reader", "Cannot convert vlan number to integer: " << namevlan[1]);
            }
        }

        /* Here we are sure that namevlan consists of a name and, optionally, a vlan id */
        add_or_update_interface(raw_interfaces, name, has_vlan, vlan_id, interface);
    }
    return raw_interfaces;
}

void process_sysfs_attribute(std::shared_ptr<AbstractSysfsInterface> sysfs_interface,
    const Path& attribute_path, std::function<void(const std::string&)> worker_func) {

    try {
        auto file = sysfs_interface->get_file(attribute_path);
        worker_func(file.value);
    }
    catch (const std::exception& e) {
        // This attribute may not be available, log and carry on
        log_debug("net-reader", "Unable to process attribute " << attribute_path.to_string() << ": " << e.what());
    }
}

void process_sysfs_data(std::shared_ptr<AbstractSysfsInterface> sysfs_interface,
        std::vector<RawNetworkInterface>& raw_interfaces) {

    for (auto& raw_interface : raw_interfaces) {

        process_sysfs_attribute(sysfs_interface, Path(PATH_CLASS_NET) / raw_interface.name / ::ATTRIBUTE_SPEED,
            [&raw_interface] (const std::string& value) {
                try {
                    raw_interface.speed = std::stoul(value);
                }
                catch (...) {
                    log_error("net-reader", "Cannot parse network interface sys data");
                    log_debug("net-reader", "Cannot convert speed to integer: " << value);
                }
            }
        );

        process_sysfs_attribute(sysfs_interface, Path(PATH_CLASS_NET) / raw_interface.name / ::ATTRIBUTE_DUPLEX,
            [&raw_interface] (const std::string& value) {
                raw_interface.is_full_duplex = convert_duplex_to_optional_bool(value);
            }
        );

        process_sysfs_attribute(sysfs_interface, Path(PATH_CLASS_NET) / raw_interface.name / ::ATTRIBUTE_MAC_ADDRESS_TYPE,
            [&raw_interface] (const std::string& value) {
                if (is_mac_permanent(value)) {
                    raw_interface.factory_mac_address = raw_interface.mac_address;
                }
            }
        );
    }
}

}

NetReader::~NetReader() {}

std::vector<RawNetworkInterface> NetReader::get_interfaces() const {

    auto raw_interfaces = process_net_data();
    process_sysfs_data(m_sysfs_interface, raw_interfaces);

    return raw_interfaces;
}
