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
 * @file rmm/utils/fru_reader.cpp
 */

#include "utils/fru_reader.hpp"

#include "ipmi/command/generic/get_fru_inventory_area_info.hpp"
#include "ipmi/command/generic/read_fru_data.hpp"
#include "fru_eeprom/fru_eeprom_parser.hpp"

#include "logger/logger.hpp"

#include <exception>
#include <thread>

using namespace agent::rmm::utils;
using namespace ipmi::command::generic;
using namespace fru_eeprom::parser;

namespace {

std::string strip_spaces(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) {
        return std::string{};
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

std::string get_null_terminated_string(const std::uint8_t* data, const std::uint32_t size) {
    if (nullptr == data) {
        return std::string{};
    }
    std::string str{};
    if (data != nullptr && size > 0) {
        for (size_t i = 0; i < size; ++i) {
            if (data[i] != '\0') {
                str += data[i];
            }
        }
    }
    return strip_spaces(str);
}

}

bool FruReader::read_and_parse(std::shared_ptr<ipmi::IpmiController> ipmi, const ipmi::BridgeInfo& bridge_info,
        const uint8_t device_id) {

    try {
        uint16_t fru_size = get_fru_size(ipmi, bridge_info, device_id);
        read_fru_data(ipmi, bridge_info, device_id, fru_size);
        parse_fru_data();
        log_fru();
    }
    catch (FruEepromParser::FruEepromException& e) {
        log_error("fru-reader", "Unable to parse fru info: " << e.what());
        return false;
    }
    catch (std::runtime_error& e) {
        log_error("fru-reader", "Unable to read fru info: " << e.what());
        return false;
    }
    catch (...) {
        log_error("fru-reader", "Unable to read FruInventoryArea info");
        return false;
    }
    return true;
}

uint16_t FruReader::get_fru_size(std::shared_ptr<ipmi::IpmiController> ipmi, const ipmi::BridgeInfo& bridge_info,
        const uint8_t device_id) const {
    request::GetFruInventoryAreaInfo req{};
    response::GetFruInventoryAreaInfo rsp{};
    req.set_device_id(device_id);
    ipmi->send(req, bridge_info, rsp);
    log_debug("fru-reader", "Fru info size: " << unsigned(rsp.get_size()));
    if (response::GetFruInventoryAreaInfo::FruAccessMode::BYTE != rsp.get_access_mode()) {
        log_error("fru-reader", "Fru info offsets are not byte aligned, fru info not supported!");
        throw std::runtime_error("Invalid alignment of the fru info offsets");
    }
    return rsp.get_size();
}

void FruReader::read_fru_data(std::shared_ptr<ipmi::IpmiController> ipmi, const ipmi::BridgeInfo& bridge_info,
        const uint8_t device_id, const uint16_t total_bytes) {
    /* This is a limitation, at maximum 28 bytes can read in single command */
    constexpr uint8_t MAX_BYTES_TO_READ = 0x1C;
    constexpr uint8_t MAX_TRIES = 5;
    constexpr uint8_t SLEEP_TIME_MS = 10;
    request::ReadFruData req{};
    response::ReadFruData rsp{};
    req.set_device_id(device_id);
    m_data.clear();

    uint16_t bytes_read = 0;
    uint16_t bytes_to_read = MAX_BYTES_TO_READ;
    while (bytes_read < total_bytes) {
        /* prepare request */
        if (bytes_to_read > total_bytes - bytes_read) {
            bytes_to_read = uint16_t(total_bytes - bytes_read);
        }
        req.set_offset(bytes_read);
        req.set_bytes_to_read(uint8_t(bytes_to_read));
        /* get results */
        for (int i = 0; i < MAX_TRIES; ++i) {
            try {
                ipmi->send(req, bridge_info, rsp);
                auto read_data = rsp.get_data();
                bytes_read = uint16_t(bytes_read + read_data.size());

                /* append to m_data */
                m_data.reserve(m_data.size() + read_data.size());
                m_data.insert(m_data.end(), read_data.begin(), read_data.end());
                break;
            }
            catch (const std::runtime_error& e) {
                log_error("fru-reader", "Failed reading Fru chunk, retry: " << unsigned(i) << "/" << unsigned(MAX_TRIES)
                    << " : "<< e.what());
                std::this_thread::sleep_for(std::chrono::milliseconds{SLEEP_TIME_MS});
            }
        }
    }
}

void FruReader::parse_fru_data() {
    FruEepromParser parser(&m_data[0], m_data.size());
    parser.parse();
    auto board = parser.get_board();
    m_serial_number = get_null_terminated_string(reinterpret_cast<uint8_t*>(board.serial_number.data), board.serial_number.data_len);
    m_part_number = get_null_terminated_string(reinterpret_cast<uint8_t*>(board.part_number.data), board.part_number.data_len);
    m_model = get_null_terminated_string(reinterpret_cast<uint8_t*>(board.product_name.data), board.product_name.data_len);
    m_manufacturer = get_null_terminated_string(reinterpret_cast<uint8_t*>(board.manufacturer.data), board.manufacturer.data_len);
}

void FruReader::log_fru() {
    log_debug("fru-reader", "Read fru data:");
    log_debug("fru-reader", "\tManufacturer: " << m_manufacturer);
    log_debug("fru-reader", "\tModel: " << m_model);
    log_debug("fru-reader", "\tPart Number: " << m_part_number);
    log_debug("fru-reader", "\tSerial Number: " << m_serial_number);
}
