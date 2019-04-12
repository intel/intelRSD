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
 * @file iscsi_mdr_builder_base.cpp
 */

#include "iscsi/iscsi_mdr_builder_base.hpp"

namespace iscsi {
namespace builder {

template<>
void IscsiMdrBuilderBase::push_string(ipmi::IpmiInterface::ByteBuffer& buffer, StringContainer& strings,
    const std::string& str, std::function<uint8_t()> str_idx) {
    if (!str.empty()) {
        ipmi::ByteBuffer::put_uint8(buffer, buffer.size(), str_idx());
        strings.push_back(str);
    } else {
        ipmi::ByteBuffer::put_uint8(buffer, buffer.size(), 0);
    }
}

template<>
void IscsiMdrBuilderBase::push_ip(ipmi::IpmiInterface::ByteBuffer& buffer, const std::string& ip, size_t ip_size) {
    ipmi::ByteBuffer::put_ip(buffer, buffer.size(), ip, ip_size);
}

template<>
void IscsiMdrBuilderBase::push_mac(ipmi::IpmiInterface::ByteBuffer& buffer, const std::string& mac, size_t mac_size) {
    ipmi::ByteBuffer::put_mac(buffer, buffer.size(), mac, mac_size);
}

void IscsiMdrBuilderBase::append_strings(ipmi::IpmiInterface::ByteBuffer& buffer, const StringContainer& strings) {
    if (strings.empty()) {
        buffer.push_back(0);
        buffer.push_back(0); // double null terminator
    } else {
        for (const auto& string : strings) {
            std::copy(string.cbegin(), string.cend(), std::back_inserter(buffer));
            buffer.push_back(0); //null terminator
        }
        buffer.push_back(0); // double null terminator
    }
}

template<>
void IscsiMdrBuilderBase::push_uint8(ipmi::IpmiInterface::ByteBuffer& buffer, const std::uint8_t& val) {
    ipmi::ByteBuffer::put_uint8(buffer, buffer.size(), val);
}

template<>
void IscsiMdrBuilderBase::push_uint16(ipmi::IpmiInterface::ByteBuffer& buffer, const std::uint16_t& val) {
    ipmi::ByteBuffer::put_uint16(buffer, buffer.size(), val);
}

}  // namespace builder
}  // namespace iscsi
