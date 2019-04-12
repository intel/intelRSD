/*!
 * @brief Simple printers for commonly used types.
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
 * @file printers.hpp
 */

#pragma once

#include <string>

namespace mdr {

/*!
 * @brief Produce a string from a serialized IP address.
 *
 * The IP address in bytes must be compatible with output from
 * inet_pton.
 *
 * @param address The serialized representation of an IP address.
 * @return A string represetation of an IP address.
 */
std::string print_ip(const uint8_t* address);

/*!
 * @brief Produce a string from a serialized MAC address.
 *
 * @param address The MAC address as a byte array.
 * @return A string representation of the MAC address.
 */
std::string print_mac(const uint8_t* address);

}  // namespace mdr
