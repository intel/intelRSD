/*!
 * @section LICENSE
 *
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
 * @file ipmb/network_utils.hpp
 *
 * @section DESCRIPTION
 * */

#pragma once

#include "json-wrapper/json-wrapper.hpp"

#include <string>
#include <vector>

namespace agent {
namespace chassis {
namespace ipmb {
namespace network_utils {

enum SourceOption : std::uint8_t {
    NONE = 0x0,
    STATIC = 0x1,
    DHCP = 0x2
};


std::string get_ip(const std::string& interface);
std::string get_mask(const std::string& interface);
SourceOption get_source(const std::string& interface);

std::string get_network_interface();

void set_ip(const std::string& interface, const std::string& ip);
void set_mask(const std::string& interface, const std::string& mask);
void set_source(const std::string& interface, const SourceOption option);

using NetworkBytes = std::vector<std::uint8_t>;

/*!
 * Converts IP address from the character string to vector of bytes.
 * @param addr IP v4 address in dot form: a.b.c.d
 * @return vector of 4 bytes. 'a' element will be the first in vector and 'd' will be the last.
 * @throw invalid_arguments if string contains malformed IP and cannot be converted to bytes.
 */
NetworkBytes addr_to_bytes(const std::string& addr);
std::string bytes_to_addr(const NetworkBytes& bytes);

/*!
 * @brief Parse file
 * @param in_file input stream with configuration
 * @return JSON representation of parsed file
 */
json::Json parse_ini(std::istream& in_file);

/*!
 * @brief Store given configuration in the stream
 * @param out_file stream where config should be stored
 * @param ini  config to be stored
 * @warning "public" only on test purposes
 */
void save_sections(std::ostream& out_file, const json::Json& ini);


/*!
 * @brief Store given document as configuration file
 * @param out_file file where config should be placed
 * @param ini config to be stored
 */
void save_ini(std::ostream& out_file, const json::Json& ini);

}
}
}
}


