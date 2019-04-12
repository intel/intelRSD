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
 * @file utils/hex_dump.cpp
 */

#include "utils/hex_dump.hpp"
#include <iomanip>
#include <sstream>

namespace {
void hex_dump_helper(std::ostream& out, const unsigned char* buffer, std::uint64_t index, std::uint64_t width) {
    std::uint64_t i;
    for (i = 0; i < index; i++) {
        out << std::hex << std::setw(2) << int(buffer[i]) << " ";
    }
    for (auto spacer = index; spacer < width; spacer++) {
        out << "   ";
    }
    out << ": ";
    for (i = 0; i < index; i++) {
        if (std::isprint(buffer[i])) {
            out << buffer[i];
        }
        else {
            out << ".";
        }
    }
    out << "\n";
}
}

namespace utils {

std::string hex_dump(const unsigned char* buffer, std::uint64_t begin, const std::uint64_t end, int width) {
    int bb_index{0};
    std::stringstream str;
    str << std::setfill('0') << std::hex << std::setw(2);
    for(; begin < end; ++begin) {
        if (bb_index >= width) {
            hex_dump_helper(str, &buffer[begin - bb_index], bb_index, width);
            bb_index = 0;
        }
        bb_index++;
    }
    if (bb_index) {
        hex_dump_helper(str, &buffer[begin - bb_index], bb_index, width);
    }
    return str.str();
}

}
