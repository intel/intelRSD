/*!
 * @brief Declaration of Get SDR Request/Response.
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
 * @file command/generic/get_sdr.hpp
 */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

namespace ipmi {
namespace command {
namespace generic {

namespace request {

/*!
 * @brief Request message for Get SDR command.
 */
class GetSdr : public Request {
public:
    static constexpr std::uint16_t SDR_FIRST_RECORD_ID = 0x0000;
    static constexpr std::uint16_t SDR_LAST_RECORD_ID = 0xffff;

    static constexpr std::uint8_t GET_SDR_ENTIRE_RECORD = 0xff;
    static constexpr std::uint8_t SDR_HEADER_LENGTH = 0x05;

    /*!
     * @brief Default constructor.
     */
    GetSdr();

    /*! Copy constructor. */
    GetSdr(const GetSdr&) = default;

    /*! Assignment operator */
    GetSdr& operator=(const GetSdr&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetSdr();

    const char* get_command_name() const override {
        return "GetSdr";
    }

    /*!
     * @brief Sets reservation id.
     * @param reservation_id reservation id.
     */
    void set_reservation_id(std::uint16_t reservation_id) {
        m_reservation_id = reservation_id;
    }

    /*!
     * @brief Sets record id.
     * @param record_id record id.
     */
    void set_record_id(std::uint16_t record_id) {
        m_record_id = record_id;
    }

    /*!
     * @brief Sets offset into record at which read starts.
     * @param offset offset into record.
     */
    void set_offset(std::uint8_t offset) {
        m_offset_into_record = offset;
    }

    /*!
     * @brief Sets number of bytes to read.
     * @param length number of bytes to read.
     */
    void set_bytes_to_read(std::uint8_t length) {
        m_bytes_to_read = length;
    }

private:
    std::uint16_t m_reservation_id{0};
    std::uint16_t m_record_id{0};
    std::uint8_t m_offset_into_record{0};
    std::uint8_t m_bytes_to_read{SDR_HEADER_LENGTH};

    void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {

/*!
 * @brief Response message for Get SDR command.
 */
class GetSdr : public Response {
public:

    /*!
     * @brief Default constructor.
     */
    GetSdr();

    /*! Copy constructor. */
    GetSdr(const GetSdr&) = default;

    /*! Assignment operator */
    GetSdr& operator=(const GetSdr&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetSdr();

    const char* get_command_name() const override {
        return "GetSdr";
    }

    /*!
     * @brief Gets next record id.
     * @return Next record id (LS Byte first).
     */
    std::uint16_t get_next_record_id() const {
        return m_next_record_id;
    }

    /*!
     * @brief Gets record data as ByteBuffer.
     * @return Record data.
     */
    const IpmiInterface::ByteBuffer& get_record_data() const {
        return m_record_data;
    }

private:
    static constexpr std::size_t MIN_RESPONSE_SIZE = 3;
    static constexpr std::size_t OFFSET_RECORD_DATA = 3;

    std::uint16_t m_next_record_id{};
    IpmiInterface::ByteBuffer m_record_data{};

    void unpack(const IpmiInterface::ByteBuffer& data) override;
};
}

}
}
}

