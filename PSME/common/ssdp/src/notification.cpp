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
 * @file notification.cpp
 *
 * @brief Notification implementation.
 * */

#include "ssdp/notification.hpp"

#include "logger/logger_factory.hpp"

namespace {

constexpr const std::uint8_t VALUE_BITS = 7;
constexpr const std::uint8_t VALUE_MASK = 0x7F;
constexpr const std::uint8_t NEXT_BYTE_FLAG = 0x80;

/*!
 * Encodes an integer using the most significant bit algorithm.
 * Most significant bit of each byte specifies that the following byte is part of the value.

 * @param value The input value.
 * @param output Buffer to store encoded integer value.
 * @return The number of bytes used in the output buffer.
 */
template<typename int_t = std::uint32_t>
size_t encode_int(int_t value, std::vector<std::uint8_t>& output) {
    size_t outputSize = 0;
    while (value > VALUE_MASK) {
        output.emplace_back(static_cast<std::uint8_t>(value & VALUE_MASK) | NEXT_BYTE_FLAG);
        value >>= VALUE_BITS;
        outputSize++;
    }
    output.emplace_back(static_cast<std::uint8_t>(value & VALUE_MASK));
    ++outputSize;
    return outputSize;
}

/*!
 * Decodes an integer using the most significant bit algorithm.
 *
 * @param[out] value Decoded integer value.
 * @param[in] input Buffer containing encoded integer.
 * @param[in] input_length Buffer length
 * @return Number of consumed bytes from input.
 */
template<typename int_t = std::uint32_t>
size_t decode_int(int_t& value, const std::uint8_t* input, size_t input_length) {
    value = 0;
    size_t i{0};
    while (i < input_length) {
        value |= (input[i] & VALUE_MASK) << (VALUE_BITS * i);
        if(!(input[i++] & NEXT_BYTE_FLAG)) {
            break;
        }
    }
    return i;
}

void check_idx(const size_t idx, const size_t length) {
    if (idx >= length) {
        throw std::runtime_error("Notification deserialization failed.");
    }
}

}

namespace ssdp {

constexpr const Notification::Bytes::size_type Notification::MAX_SERIALIZED_SIZE;

Notification::Notification(Type type) : m_type{type} {}

Notification::~Notification() {}

Notification::Type Notification::get_type() const {
    return m_type;
}

ShutDownNotification::ShutDownNotification() : Notification(Notification::Type::SHUT_DOWN) {}

ShutDownNotification::~ShutDownNotification() {}

Notification::Bytes ShutDownNotification::serialize() const {
    Notification::Bytes bytes{};
    bytes.emplace_back(std::uint8_t(get_type()));
    return bytes;
}

void ShutDownNotification::deserialize(const Bytes& bytes, const Bytes::size_type length) {
    if (bytes.empty()
        || bytes.size() < length
        || get_type() != static_cast<Notification::Type>(bytes[0])) {
        throw std::runtime_error("Notification deserialization failed.");
    }
}

NetworkChangeNotification::NetworkChangeNotification()
 : Notification(Notification::Type::NETWORK_CHANGE) {
}

NetworkChangeNotification::NetworkChangeNotification(unsigned iface_index,
        net::NetworkChangeNotifier::ChangeType change_type)
 : Notification(Notification::Type::NETWORK_CHANGE),
   m_iface_index{iface_index},
   m_change_type{change_type} {
}

NetworkChangeNotification::~NetworkChangeNotification() {}

int NetworkChangeNotification::get_iface_index() const {
    return m_iface_index;
}

net::NetworkChangeNotifier::ChangeType NetworkChangeNotification::get_change_type() const {
    return m_change_type;
}

Notification::Bytes NetworkChangeNotification::serialize() const {
    Notification::Bytes bytes{};
    bytes.emplace_back(std::uint8_t(get_type()));
    encode_int<decltype(m_iface_index)>(m_iface_index, bytes);
    bytes.emplace_back(std::uint8_t(get_change_type()));
    return bytes;
}

void NetworkChangeNotification::deserialize(const Bytes& bytes, const Bytes::size_type length) {
    Bytes::size_type idx{0};
    if (bytes.empty()
        || bytes.size() < length
        || get_type() != static_cast<Notification::Type>(bytes[idx++])) {
        throw std::runtime_error("Notification deserialization failed.");
    }
    check_idx(idx, length);
    idx += decode_int<decltype(m_iface_index)>(m_iface_index, bytes.data() + idx, length - idx);
    check_idx(idx, length);
    m_change_type = static_cast<net::NetworkChangeNotifier::ChangeType>(bytes[idx++]);
}

}
