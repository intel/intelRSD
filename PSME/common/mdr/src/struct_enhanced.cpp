/*!
 * @brief Definition of util function for reading StructEnhanced from blobs.
 *
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
 * @file struct_enhanced.cpp
 */


#include "mdr/struct_enhanced.hpp"

void mdr::read_auxiliary_strings(const uint8_t* buf, const size_t buf_size,
                            uint64_t& offset, std::vector<std::string>& strings) {
    while (offset + 1 < buf_size && std::uint8_t(*(buf + offset + 1) | *(buf + offset)) != 0) {
        uint64_t str_len = offset;
        while (str_len < buf_size && (*(buf + str_len) != 0)) {
            str_len++;
        }
        strings.emplace_back(reinterpret_cast<const char*>(buf + offset), (str_len - offset));
        offset = str_len;
        if (offset + 1 < buf_size && std::uint8_t(*(buf + offset + 1) | *(buf + offset)) != 0) {
            offset++; // jump to the next string (skip single null that separates strings)
        }
    }
    // skip two null bytes
    offset += 2;
}