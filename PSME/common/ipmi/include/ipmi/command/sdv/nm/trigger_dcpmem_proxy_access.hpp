/*!
 * @brief Intel Optane DC Persistent Memory Trigger Proxy Access IPMI command request and response.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file ipmi/command/sdv/nm/trigger_dcpmem_proxy_access.hpp
 */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/sdv/enums.hpp"
#include "reading.hpp"

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

constexpr const std::uint8_t CPU_NUMBER_MASK = 0x07; // (BIT2 | BIT1 | BIT0)
constexpr const std::uint8_t CPU_NUMBER_OFFSET = 0;
constexpr const std::uint8_t DIMM_INDEX_MASK = 0x38; // (BIT5 | BIT4 | BIT3)
constexpr const std::uint8_t DIMM_INDEX_OFFSET = 3;
constexpr const std::uint8_t SMBUS_ID_MASK = 0x40;  // (BIT6)
constexpr const std::uint8_t SMBUS_ID_OFFSET = 6;

enum class ReadingsId  : std::uint8_t {
    DCPMEM_MAILBOX_DATA = 0x00,
    DCPMEM_FW_UPDATE = 0x01
};

/*!
 * @brief Request message for TriggerDcpmemProxyAccess command.
 */
class TriggerDcpmemProxyAccess : public Request {
public:
    /*!
     * @brief Constructor.
     */
    TriggerDcpmemProxyAccess();


    TriggerDcpmemProxyAccess(const TriggerDcpmemProxyAccess&) = default;


    TriggerDcpmemProxyAccess(TriggerDcpmemProxyAccess&&) = default;


    TriggerDcpmemProxyAccess& operator=(const TriggerDcpmemProxyAccess&) = default;


    virtual ~TriggerDcpmemProxyAccess();


    const char* get_command_name() const override {
        return "TriggerDcpmemProxyAccess";
    }


    /*
     * @brief Selects Readings ID
     * @param readings_id byte
     */
    void select_readings_id(ReadingsId readings_id) {
        m_readings_id = readings_id;
    }


    /*
     * @brief Sets CPU number for DCPMEM readings.
     * @param cpu_number BIT[2:0] - represent CPU number (starting form 0, up to 8 sockets)
    */
    void set_cpu_number(std::uint8_t cpu_number) {
        m_readings_subid = static_cast<std::uint8_t>((m_readings_subid & ~CPU_NUMBER_MASK) | (cpu_number << CPU_NUMBER_OFFSET));
    }


    /*
     * @brief Sets DIMM Index for DCPMEM readings.
     * @param dimm_index BIT[5:3] - represent DIMM index (3-bit part of the SMBus address named SA)
    */
    void set_dimm_index(std::uint8_t dimm_index) {
        m_readings_subid = static_cast<std::uint8_t>((m_readings_subid & ~DIMM_INDEX_MASK) | (dimm_index << DIMM_INDEX_OFFSET));
    }


    /*
     * @brief Sets SMBus identifier for DCPMEM readings.
     * @param smbus_identifier BIT[6] - represent SMBus identifier (valid values are 0 and 1)
    */
    void set_smbus_identifier(std::uint8_t smbus_identifier) {
        m_readings_subid = static_cast<std::uint8_t>((m_readings_subid & ~SMBUS_ID_MASK) | (smbus_identifier << SMBUS_ID_OFFSET));
    }


    /*
     * @brief Sets OpCode for DCMPEM Module Mailbox
     * @param opcode - represent Opcode of DCPMEM command to trigger
    */
    void set_opcode(std::uint8_t opcode) {
        m_opcode = opcode;
    }


    /*
     * @brief Sets Sub-OpCode for DCMPEM Module Mailbox
     * @param subopcode - represent Sub-Opcode of DCPMEM command to trigger
    */
    void set_subopcode(std::uint8_t subopcode) {
        m_subopcode = subopcode;
    }


    /*
     * @brief Sets offset of data to write
     * @param write_data_offset - represent write offset (aligned to DWORD)
    */
    void set_write_offset(std::uint8_t write_data_offset) {
        m_write_data_offset = write_data_offset;
    }


    /*
     * @brief Sets size of data to write
     * @param write_data_size - represent write data size (up to 60B, aligned to DWORD)
    */
    void set_write_size(std::uint8_t write_data_size) {
        m_write_data_size = write_data_size;
    }


    /*
     * @brief Sets offset of data to read
     * @param read_data_offset - represent read offset (aligned to DWORD)
    */
    void set_read_offset(std::uint8_t read_data_offset) {
        m_read_data_offset = read_data_offset;
    }


    /*
     * @brief Sets size of data to read
     * @param read_data_size - represent read data size (aligned to DWORD)
    */
    void set_read_size(std::uint8_t read_data_size) {
        m_read_data_size = read_data_size;
    }

private:
    ReadingsId m_readings_id{ReadingsId::DCPMEM_MAILBOX_DATA}; // Currently supported DCPMEM_MAILBOX_DATA (0x00) only
    std::uint8_t m_readings_subid{0}; // SMBus_ID << 6 | DIMM_Index << 3 | CPU_Number

    std::uint8_t m_opcode{0}; // Opcode for DCPMEM module mailbox
    std::uint8_t m_subopcode{0}; // Sub-Opcode for DCPMEM module mailbox
    std::uint8_t m_write_data_offset{0};
    std::uint8_t m_write_data_size{0};
    std::uint8_t m_read_data_offset{0};
    std::uint8_t m_read_data_size{0};

    void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {

/*!
 * @brief Response message for TriggerDcpmemProxyAccess command.
 */
class TriggerDcpmemProxyAccess : public Response {
public:

    /*!
     * @brief Constructor.
     */
    TriggerDcpmemProxyAccess();


    TriggerDcpmemProxyAccess(const TriggerDcpmemProxyAccess&) = default;


    TriggerDcpmemProxyAccess(TriggerDcpmemProxyAccess&&) = default;


    TriggerDcpmemProxyAccess& operator=(const TriggerDcpmemProxyAccess&) = default;


    virtual ~TriggerDcpmemProxyAccess();


    const char* get_command_name() const override {
        return "TriggerDcpmemProxyAccess";
    }

private:
    static constexpr const std::size_t RESPONSE_SIZE = 4;

    void unpack(const IpmiInterface::ByteBuffer& data) override;

    void throw_error_on_completion_code(CompletionCode completion_code) const override;
};

}

}
}
}
