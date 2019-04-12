/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file tools.cpp
 */

#include "drive_handling/utils/nvme/tools.hpp"

#include <sstream>

namespace {
constexpr char NAMESPACE_AFFIX[] = "n";
constexpr char PARTITION_AFFIX[] = "p";
}

namespace agent {
namespace storage {
namespace utils {

std::string convert_uint8_to_string(const std::uint8_t* array, std::size_t size) {
    std::stringstream ss{};
    for (size_t i = 0; i < size; ++i) {
        if (array[i] == '\0') {
            break;
        }
        ss << array[i];
    }
    std::string s = ss.str();
    std::size_t first = s.find_first_not_of(' ');
    std::size_t last = s.find_last_not_of(' ');
    if (std::string::npos == first || std::string::npos == last || first > last) {
        return "";
    }
    return s.substr(first, last - first + 1);
}


std::string get_namespace_device_name(const std::string& name, std::uint32_t namespace_id) {
    std::stringstream ss{};
    ss << name << ::NAMESPACE_AFFIX << namespace_id;
    return ss.str();
}


long double convert_uint128_to_long_double(const std::uint64_t value[2]) {
    long double ret{};
    static constexpr int size = 16;
    const std::uint8_t* bytes = reinterpret_cast<const std::uint8_t*>(value);
    for (int i = 0; i < size; ++i) {
        ret *= 256;
        ret += bytes[size - 1 - i];
    }

    return ret;
}

}
}
}
