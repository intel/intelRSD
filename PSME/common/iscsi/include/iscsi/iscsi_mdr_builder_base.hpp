/*!
 * @brief Common facilities used when building MDR structures.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file iscsi_mdr_builder_base.hpp
 */

#pragma once

#include "ipmi/ipmi_interface.hpp"
#include "ipmi/utils/byte_buffer.hpp"
#include "iscsi/structs/iscsi_mdr_header.hpp"
#include "iscsi/structs/iscsi_mdr_target.hpp"

#include <vector>
#include <string>
#include <functional>

namespace iscsi {
namespace builder {

/*!
 * @brief The iSCSI MDR structure builder base with common functions.
 */
class IscsiMdrBuilderBase {
public:
    /*!
     * @brief Deleted constructor.
     */
    IscsiMdrBuilderBase() = delete;

protected:
    using StringContainer = std::vector<std::string>;

    /*!
     * @brief Push back a string into an IPMI byte buffer.
     *
     * @param buffer the buffer to append to.
     * @param strings the strings container.
     * @param str the string to append.
     * @param str_idx the string index distribution function.
     */
    template<typename T>
    static void push_string(ipmi::IpmiInterface::ByteBuffer& buffer, StringContainer& strings, const T& str,
        std::function<uint8_t()> str_idx) {
        if (str.has_value()) {
            push_string(buffer, strings, str.value(), str_idx);
        } else {
            push_string(buffer, strings, static_cast<std::string>(""), str_idx);
        }
    }

    /*!
     * @brief Push back an optional IP address into an IPMI byte buffer.
     *
     * @param buffer the buffer to append to.
     * @param ip the optional IP address.
     * @param ip_size the size of the IP address.
     */
    template<typename T>
    static void push_ip(ipmi::IpmiInterface::ByteBuffer& buffer, const T& ip, size_t ip_size) {
        if (ip.has_value()) {
            push_ip(buffer, static_cast<std::string>(ip), ip_size);
        } else {
            push_ip(buffer, static_cast<std::string>(""), ip_size);
        }
    }

    /*!
     * @brief Push back an optional MAC address into an IPMI byte buffer.
     *
     * @param buffer the buffer to append to.
     * @param mac the optional MAC address.
     * @param mac_size the size of the MAC address.
     */
    template<typename T>
    static void push_mac(ipmi::IpmiInterface::ByteBuffer& buffer, const T& mac, size_t mac_size) {
        if (mac.has_value()) {
            push_mac(buffer, static_cast<std::string>(mac), mac_size);
        } else {
            push_mac(buffer, static_cast<std::string>(""), mac_size);
        }
    }

    /*!
     * @brief Push back an optional byte into an IPMI byte buffer.
     *
     * @param buffer the buffer to append to.
     * @param val the optional byte.
     */
    template<typename T>
    static void push_uint8(ipmi::IpmiInterface::ByteBuffer& buffer, const T& val) {
        if (val.has_value()) {
            push_uint8(buffer, static_cast<std::uint16_t>(val));
        } else {
            push_uint8(buffer, static_cast<std::uint8_t>(0));
        }
    }

    /*!
     * @brief Push back an optional uint16_t into an IPMI byte buffer.
     *
     * @param buffer the buffer to append to.
     * @param val the optional uint16_t.
     */
    template<typename T>
    static void push_uint16(ipmi::IpmiInterface::ByteBuffer& buffer, const T& val) {
        if (val.has_value()) {
            push_uint16(buffer, static_cast<std::uint16_t>(val));
        } else {
            push_uint16(buffer, static_cast<std::uint16_t>(0));
        }
    }

    /*!
     * @brief Push back strings into an IPMI byte buffer and double null terminate them.
     *
     * @param buffer the buffer to append to.
     * @param strings the string to be appended.
     */
    static void append_strings(ipmi::IpmiInterface::ByteBuffer& buffer, const StringContainer& strings);

};

/*!
 * @brief Push back an IP address into an IPMI byte buffer.
 *
 * @param buffer the buffer to append to.
 * @param ip the IP address.
 * @param ip_size the size of the IP address.
 */
template<>
void IscsiMdrBuilderBase::push_ip(ipmi::IpmiInterface::ByteBuffer& buffer, const std::string& ip, size_t ip_size);

/*!
 * @brief Push back an optional MAC address into an IPMI byte buffer.
 *
 * @param buffer the buffer to append to.
 * @param mac the optional MAC address.
 * @param mac_size the size of the MAC address.
 */
template<>
void IscsiMdrBuilderBase::push_mac(ipmi::IpmiInterface::ByteBuffer& buffer, const std::string& mac, size_t mac_size);

/*!
 * @brief Push back a uint16_t into an IPMI byte buffer.
 *
 * @param buffer the buffer to append to.
 * @param val the uint16_t.
 */
template<>
void IscsiMdrBuilderBase::push_uint16(ipmi::IpmiInterface::ByteBuffer& buffer, const std::uint16_t& val);

/*!
 * @brief Push back a byte into an IPMI byte buffer.
 *
 * @param buffer the buffer to append to.
 * @param val the byte.
 */
template<>
void IscsiMdrBuilderBase::push_uint8(ipmi::IpmiInterface::ByteBuffer& buffer, const std::uint8_t& val);

/*!
 * @brief Push back a string into an IPMI byte buffer.
 *
 * @param buffer the buffer to append to.
 * @param strings the strings container.
 * @param str the string to append.
 * @param str_idx the string index distribution function.
 */
template<>
void IscsiMdrBuilderBase::push_string(ipmi::IpmiInterface::ByteBuffer& buffer, StringContainer& strings,
    const std::string& str, std::function<uint8_t()> str_idx);

}  // namespace builder
}  // namespace iscsi
