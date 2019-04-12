/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file send_psu_command.cpp
 * @brief SendPsuCommand IPMI command request and response implementation.
 * */

#include "ipmi/command/sdv/rmm/send_psu_command.hpp"
#include "ipmi/command/sdv/enums.hpp"
#include "ipmi/utils/sdv/linear_to_double.hpp"
#include "ipmi/utils/byte_buffer.hpp"

using namespace ipmi::command::sdv;
using namespace ipmi::sdv;

/* SendPsuCommand */

request::SendPsuCommand::SendPsuCommand(OperationCode op_code, Command cmd)
    : Request(INTEL, SEND_PSU_COMMAND), m_operation_code{op_code}, m_command{cmd} {}

void request::SendPsuCommand::set_psu_id(std::uint8_t psu_id) {
    m_psu_id = psu_id;
}

void request::SendPsuCommand::pack(IpmiInterface::ByteBuffer& data) const {
    data.push_back(m_psu_id);
    data.push_back(static_cast<std::uint8_t>(m_operation_code));
    data.push_back(static_cast<std::uint8_t>(m_command));
    pack_fields(data);
}

void request::SendPsuCommand::pack_fields(IpmiInterface::ByteBuffer&) const {
    /* default empty implementation for read commands */
}


/* SendPsuClearFaults */
request::SendPsuClearFaults::SendPsuClearFaults()
    : SendPsuCommand(OperationCode::WRITE_BYTE, Command::CLEAR_FAULTS) {}

request::SendPsuClearFaults::~SendPsuClearFaults() {}

constexpr const std::uint8_t request::SendPsuClearFaults::REQUEST_DATA_SIZE;

void request::SendPsuClearFaults::pack_fields(IpmiInterface::ByteBuffer& data) const {
    data.push_back(REQUEST_DATA_SIZE);
}

response::SendPsuClearFaults::SendPsuClearFaults()
    : Response(INTEL + 1, SEND_PSU_COMMAND, RESP_SIZE) {}

response::SendPsuClearFaults::~SendPsuClearFaults() {}

void response::SendPsuClearFaults::unpack(const IpmiInterface::ByteBuffer&) {
    /* no data in the response */
}


/* SendPsuReadStatusWord */

request::SendPsuReadStatusWord::SendPsuReadStatusWord()
    : SendPsuCommand(OperationCode::READ_WORD, Command::STATUS_WORD) {}

request::SendPsuReadStatusWord::~SendPsuReadStatusWord() {}

response::SendPsuReadStatusWord::SendPsuReadStatusWord()
    : Response(INTEL + 1, SEND_PSU_COMMAND, RESP_SIZE) {}

response::SendPsuReadStatusWord::~SendPsuReadStatusWord() {}

void response::SendPsuReadStatusWord::unpack(const IpmiInterface::ByteBuffer& data) {
    m_status_word = std::uint16_t(data[OFFSET_DATA] << 8 | data[OFFSET_DATA + 1]);
    m_warnings_present = ((data[OFFSET_DATA] & LOW_WARNING_MASK) != 0)
                      || ((data[OFFSET_DATA + 1] & HIGH_WARNING_MASK) != 0);
}

std::uint16_t response::SendPsuReadStatusWord::get_status_word() const {
    return m_status_word;
}


/* SendPsuReadMfrPsuSta */

request::SendPsuReadMfrPsuSta::SendPsuReadMfrPsuSta()
    : SendPsuCommand(OperationCode::READ_BLOCK, Command::MFR_PSU_STA) {}

request::SendPsuReadMfrPsuSta::~SendPsuReadMfrPsuSta() {}

response::SendPsuReadMfrPsuSta::SendPsuReadMfrPsuSta()
    : Response(INTEL + 1, SEND_PSU_COMMAND, RESP_SIZE) {}

response::SendPsuReadMfrPsuSta::~SendPsuReadMfrPsuSta() {}

void response::SendPsuReadMfrPsuSta::unpack(const IpmiInterface::ByteBuffer& data) {
    static constexpr const std::size_t ALM_STA_POSITION = OFFSET_DATA;
    static constexpr const std::size_t AC_STATUS_POSITION = ALM_STA_POSITION + 4;

    m_alm_sta = ipmi::ByteBuffer::get_uint32(data, ALM_STA_POSITION);
    m_ac_status = ipmi::ByteBuffer::get_uint8(data, AC_STATUS_POSITION);
}

bool response::SendPsuReadMfrPsuSta::ac_has_failed() const {
    static constexpr const std::uint8_t AC_FAILED = 0;
    return m_ac_status == AC_FAILED;
}

bool response::SendPsuReadMfrPsuSta::errors_present() const {
    /* both inputs have under/over voltage */
    if (((m_alm_sta & VIN0_UV_FAULT) && (m_alm_sta & VIN1_UV_FAULT)) ||
        ((m_alm_sta & VIN0_OV_FAULT) && (m_alm_sta & VIN1_OV_FAULT))) {
        return true;
    }
    return (0 != (m_alm_sta & (
        PSON_FAULT | SHORT_PIN_FAULT | IOUT_OC_FAULT |
        STDBY_OV_FAULT | STDBY_UV_FAULT | VOUT_UV_FAULT | VOUT_OV_FAULT | HW_OV_FAULT |
        OT_FAULT | DD_OT_FAULT | PFC_OT_FAULT | NTC_OT_FAULT | SR2_OT_FAULT | SR1_OT_FAULT)));
}

bool response::SendPsuReadMfrPsuSta::warnings_present() const {
    return (0 != (m_alm_sta & (
        FAN_WARN | FAN_FAULT | OT_WARN |
        VIN1_OV_FAULT | VIN1_UV_FAULT | VIN0_OV_FAULT | VIN0_UV_FAULT)));
}


/* SendPsuReadPowerOutput */

request::SendPsuReadPowerOutput::SendPsuReadPowerOutput()
    : SendPsuCommand(OperationCode::READ_WORD, Command::READ_POUT) {}

request::SendPsuReadPowerOutput::~SendPsuReadPowerOutput() {}

response::SendPsuReadPowerOutput::SendPsuReadPowerOutput()
    : Response(INTEL + 1, SEND_PSU_COMMAND, RESP_SIZE) {}

response::SendPsuReadPowerOutput::~SendPsuReadPowerOutput() {}

void response::SendPsuReadPowerOutput::unpack(const IpmiInterface::ByteBuffer& data) {
    m_output_power = std::uint16_t(data[OFFSET_DATA + 1] << 8 | data[OFFSET_DATA]);
}

double response::SendPsuReadPowerOutput::get_power_output_value() const {
    return linear_to_double(m_output_power);
}


/* SendPsuReadPowerCapacity */

request::SendPsuReadPowerCapacity::SendPsuReadPowerCapacity()
    : SendPsuCommand(OperationCode::READ_WORD, Command::MFR_POUT_MAX) {}

request::SendPsuReadPowerCapacity::~SendPsuReadPowerCapacity() {}

response::SendPsuReadPowerCapacity::SendPsuReadPowerCapacity()
    : Response(INTEL + 1, SEND_PSU_COMMAND, RESP_SIZE) {}

response::SendPsuReadPowerCapacity::~SendPsuReadPowerCapacity() {}

void response::SendPsuReadPowerCapacity::unpack(const IpmiInterface::ByteBuffer& data) {
    m_power_capacity = std::uint16_t(data[OFFSET_DATA + 1] << 8 | data[OFFSET_DATA]);
}

double response::SendPsuReadPowerCapacity::get_power_capacity_value() const {
    return linear_to_double(m_power_capacity);
}


/* SendPsuReadSerial */

request::SendPsuReadSerial::SendPsuReadSerial()
    : SendPsuCommand(OperationCode::READ_BLOCK, Command::MFR_SERIAL) {}

request::SendPsuReadSerial::~SendPsuReadSerial() {}

response::SendPsuReadSerial::SendPsuReadSerial()
    : Response(INTEL + 1, SEND_PSU_COMMAND, MIN_RESP_SIZE) {}

response::SendPsuReadSerial::~SendPsuReadSerial() {}

void response::SendPsuReadSerial::unpack(const IpmiInterface::ByteBuffer& data) {
    m_serial = std::string{data.begin() + 1, data.end()};
}

std::string response::SendPsuReadSerial::get_serial() const {
    return m_serial;
}


/* SendPsuReadModel */

request::SendPsuReadModel::SendPsuReadModel()
    : SendPsuCommand(OperationCode::READ_BLOCK, Command::MFR_MODEL) {}

request::SendPsuReadModel::~SendPsuReadModel() {}

response::SendPsuReadModel::SendPsuReadModel()
    : Response(INTEL + 1, SEND_PSU_COMMAND, MIN_RESP_SIZE) {}

response::SendPsuReadModel::~SendPsuReadModel() {}

void response::SendPsuReadModel::unpack(const IpmiInterface::ByteBuffer& data) {
    m_model = std::string{data.begin() + 1, data.end()};
}

std::string response::SendPsuReadModel::get_model() const {
    return m_model;
}


/* SendPsuReadManufacturer */

request::SendPsuReadManufacturer::SendPsuReadManufacturer()
    : SendPsuCommand(OperationCode::READ_BLOCK, Command::MFR_ID) {}

request::SendPsuReadManufacturer::~SendPsuReadManufacturer() {}

response::SendPsuReadManufacturer::SendPsuReadManufacturer()
    : Response(INTEL + 1, SEND_PSU_COMMAND, MIN_RESP_SIZE) {}

response::SendPsuReadManufacturer::~SendPsuReadManufacturer() {}

void response::SendPsuReadManufacturer::unpack(const IpmiInterface::ByteBuffer& data) {
    m_manufacturer = std::string{data.begin() + 1, data.end()};
}

std::string response::SendPsuReadManufacturer::get_manufacturer() const {
    return m_manufacturer;
}


/* SendPsuReadRevision */

request::SendPsuReadRevision::SendPsuReadRevision()
    : SendPsuCommand(OperationCode::READ_BLOCK, Command::MFR_REVISION) {}

request::SendPsuReadRevision::~SendPsuReadRevision() {}

response::SendPsuReadRevision::SendPsuReadRevision()
    : Response(INTEL + 1, SEND_PSU_COMMAND, MIN_RESP_SIZE) {}

response::SendPsuReadRevision::~SendPsuReadRevision() {}

void response::SendPsuReadRevision::unpack(const IpmiInterface::ByteBuffer& data) {
    m_revision = std::string{data.begin() + 1, data.end()};
}

std::string response::SendPsuReadRevision::get_revision() const {
    return m_revision;
}

/* SendPsuWriteOperation */

constexpr const std::uint8_t request::SendPsuWriteOperation::REQUEST_DATA_SIZE;

request::SendPsuWriteOperation::SendPsuWriteOperation()
    : SendPsuCommand(OperationCode::WRITE_BYTE, Command::OPERATION) {}

request::SendPsuWriteOperation::~SendPsuWriteOperation() {}

void request::SendPsuWriteOperation::pack_fields(IpmiInterface::ByteBuffer& data) const {
    data.push_back(REQUEST_DATA_SIZE);
    data.push_back(static_cast<std::uint8_t>(m_state));
}

void request::SendPsuWriteOperation::set_state(State state) {
    m_state = state;
}

response::SendPsuWriteOperation::SendPsuWriteOperation()
    : Response(INTEL + 1, SEND_PSU_COMMAND, RESPONSE_SIZE) {}

response::SendPsuWriteOperation::~SendPsuWriteOperation() {}

void response::SendPsuWriteOperation::unpack(const IpmiInterface::ByteBuffer&) {}


/* SendPsuReadOperation */

request::SendPsuReadOperation::SendPsuReadOperation()
    : SendPsuCommand(OperationCode::READ_BYTE, Command::OPERATION) {}

request::SendPsuReadOperation::~SendPsuReadOperation() {}

response::SendPsuReadOperation::SendPsuReadOperation()
    : Response(INTEL + 1, SEND_PSU_COMMAND, RESPONSE_SIZE) {}

response::SendPsuReadOperation::~SendPsuReadOperation() {}

void response::SendPsuReadOperation::unpack(const IpmiInterface::ByteBuffer& data) {
    constexpr uint8_t OPERATION_BYTE_OFFSET = 1;
    m_is_enabled = ENABLED_STATE == data[OPERATION_BYTE_OFFSET];
    m_operation = data[OPERATION_BYTE_OFFSET];
}
