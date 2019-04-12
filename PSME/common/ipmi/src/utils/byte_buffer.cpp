/*!
 * @brief Utils for handling byte buffer
 *
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
 * @file utils/byte_buffer.cpp
 */

#include "ipmi/ipmi_interface.hpp"
#include "ipmi/utils/byte_buffer.hpp"

extern "C" {
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ether.h>
}

#include <algorithm>
#include <iterator>
#include <type_traits>

using namespace ipmi;

std::uint8_t ByteBuffer::get_uint8(const IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos) {
    return data.at(pos);
}


std::uint16_t ByteBuffer::get_uint16(const IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos) {
    return static_cast<std::uint16_t>(data.at(pos) | (data.at(pos + 1) << 8));
}


std::uint32_t ByteBuffer::get_uint32(const IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos) {
    return static_cast<std::uint32_t>(data.at(pos) | (data.at(pos + 1) << 8) | (data.at(pos + 2) << 16) | (data.at(pos + 3) << 24));
}


std::uint64_t ByteBuffer::get_uint64(const IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos) {
    uint64_t ret = 0;
    for (size_t i = 0; i < sizeof(std::uint64_t); i += 8) {
        ret |= data.at(pos + (i / 8)) << i;
    }
    return ret;
}


bool ByteBuffer::is_bit_set(const IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos, unsigned bit) {
    return (data.at(pos) & (0x01 << bit)) == (0x01 << bit);
}


void ByteBuffer::put_uint32(IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos, std::uint32_t val) {
    if (data.size() == pos) {
        data.push_back(static_cast<std::uint8_t>(val & 0xff));
        data.push_back(static_cast<std::uint8_t>((val >> 8) & 0xff));
        data.push_back(static_cast<std::uint8_t>((val >> 16) & 0xff));
        data.push_back(static_cast<std::uint8_t>((val >> 24) & 0xff));
    }
    else {
        data.at(pos) = static_cast<std::uint8_t>(val & 0xff);
        data.at(pos + 1) = static_cast<std::uint8_t>((val >> 8) & 0xff);
        data.at(pos + 2) = static_cast<std::uint8_t>((val >> 16) & 0xff);
        data.at(pos + 3) = static_cast<std::uint8_t>((val >> 24) & 0xff);
    }
}


void ByteBuffer::put_uint16(IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos, std::uint16_t val) {
    if (data.size() == pos) {
        data.push_back(static_cast<std::uint8_t>(val & 0xff));
        data.push_back(static_cast<std::uint8_t>((val >> 8) & 0xff));
    }
    else {
        data.at(pos) = static_cast<std::uint8_t>(val & 0xff);
        data.at(pos + 1) = static_cast<std::uint8_t>((val >> 8) & 0xff);
    }
}


void ByteBuffer::put_uint8(IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos, std::uint8_t val) {
    if (data.size() == pos) {
        data.push_back(static_cast<std::uint8_t>(val & 0xff));
    }
    else {
        data.at(pos) = static_cast<std::uint8_t>(val & 0xff);
    }
}

void ByteBuffer::put_uint64(IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos,
    std::uint64_t val) {
    if (data.size() == pos) {
        data.push_back(static_cast<std::uint8_t>(val & 0xff));
        data.push_back(static_cast<std::uint8_t>((val >> 8) & 0xff));
        data.push_back(static_cast<std::uint8_t>((val >> 16) & 0xff));
        data.push_back(static_cast<std::uint8_t>((val >> 24) & 0xff));
        data.push_back(static_cast<std::uint8_t>((val >> 32) & 0xff));
        data.push_back(static_cast<std::uint8_t>((val >> 40) & 0xff));
        data.push_back(static_cast<std::uint8_t>((val >> 48) & 0xff));
        data.push_back(static_cast<std::uint8_t>((val >> 56) & 0xff));
    } else {
        data.at(pos) = static_cast<std::uint8_t>(val & 0xff);
        data.at(pos + 1) = static_cast<std::uint8_t>((val >> 8) & 0xff);
        data.at(pos + 2) = static_cast<std::uint8_t>((val >> 16) & 0xff);
        data.at(pos + 3) = static_cast<std::uint8_t>((val >> 24) & 0xff);
        data.at(pos + 1) = static_cast<std::uint8_t>((val >> 32) & 0xff);
        data.at(pos + 2) = static_cast<std::uint8_t>((val >> 40) & 0xff);
        data.at(pos + 3) = static_cast<std::uint8_t>((val >> 48) & 0xff);
        data.at(pos + 3) = static_cast<std::uint8_t>((val >> 56) & 0xff);
    }
}


void ByteBuffer::put_ip(IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos, std::string val,
    size_t ip_size) {
    IpmiInterface::ByteBuffer output;
    if (val.empty()) {
        // if an IP address was not set in request, send all zeros
        for (decltype(ip_size) i = 0; i < ip_size; ++i) {
            output.push_back(0);
        }
    } else {
        static constexpr const int INET_PTON_SUCCESS{1};
        struct in_addr ipv4_address;
        if (inet_pton(AF_INET, val.c_str(), &ipv4_address) == INET_PTON_SUCCESS) {
            put_uint32(output, output.size(), ipv4_address.s_addr);
            // if an IPv4 address is sent, fill rest of field with zeros
            for (size_t i = 0; i < ip_size - sizeof(in_addr); ++i) {
                output.push_back(0);
            }
        } else {
            struct in6_addr ipv6_address;
            if (inet_pton(AF_INET6, val.c_str(), &ipv6_address) == INET_PTON_SUCCESS) {
                for (size_t i = 0; i < ip_size; ++i) {
                    output.insert(output.end(), ipv6_address.s6_addr, ipv6_address.s6_addr + ip_size);
                }
            }
        }
    }

    if (output.empty()) {
        throw std::runtime_error("Attempted to serialize an invalid IP address!");
    }

    if (data.size() == pos) {
        std::copy(output.cbegin(), output.cend(), std::back_inserter(data));
    } else {
        auto iter = data.begin();
        std::advance(iter, pos);
        std::swap_ranges(output.begin(), output.end(), iter);
    }
}

void ByteBuffer::put_mac(IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos, std::string val,
    size_t mac_size) {
    if (val.empty()) {
        // if an empty string has been passed, put all zeros
        for (decltype(mac_size) i = 0; i < mac_size; ++i) {
            data.push_back(0);
        }
        return;
    }

    // according to metadata, MAC address can use both colons and dashes: aa:bb-cc:dd-ee:ff
    // however, ether_aton function only accepts notation with colons.
    // So, we convert the dashes to colons before checking if the address is correct
    std::replace(val.begin(), val.end(), '-', ':');

    IpmiInterface::ByteBuffer output;
    const struct ether_addr* address = ether_aton(val.c_str());
    if (address == NULL) {
        throw std::runtime_error("Attempted to serialize an invalid MAC address!");
    } else {
        for (size_t i = 0; i < mac_size; ++i) {
            output.push_back(uint8_t(address->ether_addr_octet[i]));
        }
    }

    if (data.size() == pos) {
        std::copy(output.cbegin(), output.cend(), std::back_inserter(data));
    } else {
        auto iter = data.begin();
        std::advance(iter, pos);
        std::swap_ranges(output.begin(), output.end(), iter);
    }
}
