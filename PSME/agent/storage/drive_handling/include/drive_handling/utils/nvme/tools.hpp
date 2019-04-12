/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file tools.hpp
 */

#pragma once



#include <string>

namespace agent {
namespace storage {
namespace utils {

/*!
 * @brief Converts 8-bit unsigned integer to string representation.
 * @param array Array of 8-bit unsigned integers to convert.
 * @param size Size of the array.
 * @return String representation.
 */
std::string convert_uint8_to_string(const std::uint8_t* array, std::size_t size);

/*!
 * @brief Build drive name with namespace identifier.
 * @param name Drive name.
 * @param namespace_id Drive's namespace identifier
 * @return Drive name with namespace ID.
 */
std::string get_namespace_device_name(const std::string& name, std::uint32_t namespace_id);


/*!
 * @brief Convert 128-bit unsigned integer to long double.
 * @param value Value to convert.
 * @return Long double value.
 */
long double convert_uint128_to_long_double(const std::uint64_t value[2]);



}
}
}