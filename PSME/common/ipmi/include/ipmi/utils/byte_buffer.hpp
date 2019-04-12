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
 * @file utils/byte_buffer.hpp
 */

#pragma once

#include "ipmi/ipmi_interface.hpp"

namespace ipmi {

class ByteBuffer {
public:
    /*!
     * @brief Put requested 32bit unsigned value in the buffer
     *
     * Buffer is extended only if value is to be appended at the end. Otherwise
     * value is unconditionally set, it might cause a generic exception if no such
     * entries in the buffer. LSB byte first.
     *
     * @param data buffer to put the value into
     * @param pos position in the data where value is to be put
     * @param val value to be put
     */
    static void put_uint32(IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos, std::uint32_t val);


    /*!
     * @brief Put requested 16bit unsigned value in the buffer
     *
     * Buffer is extended only if value is to be appended at the end. Otherwise
     * value is unconditionally set, it might cause a generic exception if no such
     * entries in the buffer. LSB byte first.
     *
     * @param data buffer to put the value into
     * @param pos position in the data where value is to be put
     * @param val value to be put
     */
    static void put_uint16(IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos, std::uint16_t val);


    /*!
     * @brief Put requested 8bit unsigned value in the buffer
     * @param data buffer to put the value into
     * @param pos position in the data where value is to be put
     * @param val value to be put
     */
    static void put_uint8(IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos, std::uint8_t val);


    /*!
     * @brief Put requested 64bit unsigned value in the buffer
     * @param data buffer to put the value into
     * @param pos position in the data where value is to be put
     * @param val value to be put
     */
    static void put_uint64(IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos, std::uint64_t val);


    /*!
     * @brief Put requested IP address in the buffer
     * @param data buffer to put the value into
     * @param pos position in the data where value is to be put
     * @param val stringified version of the ip address to be put
     * @param ip_size the size of the ip address to be put in bytes
     */
    static void put_ip(IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos, std::string val,
        size_t ip_size);

    /*!
     * @brief Put requested MAC address in the buffer
     * @param data buffer to put the value into
     * @param pos position in the data where value is to be put
     * @param val stringified version of the mac address to be put
     * @param mac_size the size of the mac address to be put in bytes
     */
    static void put_mac(IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos, std::string val,
        size_t mac_size);


    /*!
     * @brief Get 8bit unsigned value from data buffer
     * @warning data availability is not checked, in case wrong pos is given, generic exception is thrown
     * @param data buffer to get bytes
     * @param pos position of first (LSB) byte
     * @return 16 bit unsigned value
     */
    static std::uint8_t get_uint8(const IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos);


    /*!
     * @brief Get 16bit unsigned value from data buffer
     *
     * LSB byte first.
     *
     * @warning data availability is not checked, in case wrong pos is given, generic exception is thrown
     * @param data buffer to get bytes
     * @param pos position of first (LSB) byte
     * @return 16 bit unsigned value
     */
    static std::uint16_t get_uint16(const IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos);


    /*!
     * @brief Get 32bit unsigned value from data buffer
     *
     * LSB byte first.
     *
     * @warning data availability is not checked, in case wrong pos is given, generic exception is thrown
     * @param data buffer to get bytes
     * @param pos position of first (LSB) byte
     * @return 32 bit unsigned value
     */
    static std::uint32_t get_uint32(const IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos);


    /*!
     * @brief Get 64bit unsigned value from data buffer
     *
     * LSB byte first.
     *
     * @warning data availability is not checked, in case wrong pos is given, generic exception is thrown
     * @param data buffer to get bytes
     * @param pos position of first (LSB) byte
     * @return bit unsigned value
     */
    static std::uint64_t get_uint64(const IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos);


    /*!
     * @brief Check if bit is set
     * @warning data availability is not checked, in case wrong pos is given, generic exception is thrown
     * @param data buffer to get byte
     * @param pos position of byte
     * @param bit to be checked (0..7)
     * @return true if bit is set
     */
    static bool is_bit_set(const IpmiInterface::ByteBuffer& data, IpmiInterface::ByteBuffer::size_type pos, unsigned bit);
};

}
