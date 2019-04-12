/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file ipmi_i2c_interface.cpp
 * @brief IPMI based I2c interface implementation
 * */

#include "i2c/ipmi_i2c_access_interface.hpp"

#include "ipmi/command/generic/master_write_read.hpp"

#include <logger/logger_factory.hpp>

using namespace agent::pnc::i2c;
using namespace ipmi::command;

IpmiI2cAccessInterface::IpmiI2cAccessInterface(const std::string& ip, const std::uint32_t& port, const std::string& user,
        const std::string& pass) : m_mc(new ipmi::manager::ipmitool::ManagementController) {
    m_mc->set_ip(ip);
    m_mc->set_port(port);
    m_mc->set_username(user);
    m_mc->set_password(pass);
}

IpmiI2cAccessInterface::~IpmiI2cAccessInterface() {}

void IpmiI2cAccessInterface::write(std::uint8_t port, std::uint16_t address, std::uint16_t,
        std::uint8_t* data, std::uint16_t size, bool) const {

    std::vector<std::uint8_t> data_in{};
    for (unsigned i = 0; i < size; ++i) {
        data_in.push_back(data[i]);
    }
    generic::request::MasterWriteRead request(port, uint8_t(address), 0, data_in);
    generic::response::MasterWriteRead response{0};
    m_mc->send(request, response);
}

void IpmiI2cAccessInterface::read(std::uint8_t port, std::uint16_t address, std::uint16_t offset, std::uint8_t* data,
        std::uint16_t size, bool is_16bit_offset) const {

    std::vector<std::uint8_t> data_out{};

    static const uint8_t SINGLE_READ_SIZE = 16;
    uint16_t cur_offset = offset;
    uint16_t remaining_bytes = size;

    while (remaining_bytes != 0) {
        uint8_t bytes_to_read = SINGLE_READ_SIZE;
        if (remaining_bytes < SINGLE_READ_SIZE) {
            bytes_to_read = uint8_t(remaining_bytes);
        }

        std::vector<std::uint8_t> data_in{};
        if (is_16bit_offset) {
            data_in.push_back(uint8_t(cur_offset >> 8));
            data_in.push_back(uint8_t(cur_offset));
        }
        else {
            data_in.push_back(uint8_t(cur_offset));
        }
        generic::request::MasterWriteRead request(port, uint8_t(address), bytes_to_read, data_in);
        generic::response::MasterWriteRead response{bytes_to_read};
        m_mc->send(request, response);
        auto result = response.get_data();
        for (const auto& element : result) {
            data_out.push_back(element);
        }
        remaining_bytes = uint16_t(remaining_bytes - bytes_to_read);
        cur_offset = uint16_t(cur_offset + bytes_to_read);
    }
    if (data_out.size() != size) {
        throw std::runtime_error("IPMI reading failed. Size: " + std::to_string(int(data_out.size())) + ", expected: " + std::to_string(int(size)));
    }
    for (unsigned i = 0; i < size; ++i) {
        data[i] = data_out[i];
    }

}
