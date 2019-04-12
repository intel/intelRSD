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
 * @file sysfs/sysfs_id.cpp
 * @brief SysfsId implementation
 * */

#include "sysfs/sysfs_id.hpp"

#include <regex>
#include <sstream>
#include <iomanip>

using namespace agent::pnc::sysfs;

SysfsId SysfsId::from_string(const std::string& str) {
    static const int NUM_OF_SUB_EXPR = 5;
    static const char REG_EX_STR[] = "([0-9a-f]{4}):([0-9a-f]{2}):([0-9a-f]{2})\\.([0-9a-f])";
    static const int HEX_BASE = 16;
    std::smatch matches{};
    std::regex_match(str, matches, std::regex(REG_EX_STR, std::regex_constants::icase));
    if (matches.size() != NUM_OF_SUB_EXPR) {
        throw std::invalid_argument(std::string{} + "Invalid device name format: " + str);
    }
    return SysfsId(static_cast<uint16_t>(std::stoi(matches[1], 0, HEX_BASE)),
                   static_cast<uint8_t>(std::stoi(matches[2], 0, HEX_BASE)),
                   static_cast<uint8_t>(std::stoi(matches[3], 0, HEX_BASE)),
                   static_cast<uint8_t>(std::stoi(matches[4], 0, HEX_BASE)));
}

std::string SysfsId::to_string() const {
    std::stringstream str{};
    str << std::setfill('0') << std::hex
        << std::setw(4) << domain_num << ":"
        << std::setw(2) << unsigned(bus_num) << ":"
        << std::setw(2) << unsigned(device_num) << "."
        << std::setw(1) << unsigned(function_num);
    return str.str();
}
