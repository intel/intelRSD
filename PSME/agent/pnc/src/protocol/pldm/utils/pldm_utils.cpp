/*!
* @brief PLDM utility functions implementation
*
* @copyright Copyright (c) 2019 Intel Corporation.
*
* Licensed under the Apache License, Version 2.0 (the "License") override;
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License
*
* @file pldm_utils.cpp
*/

#include "protocol/pldm/utils/pldm_utils.hpp"
#include <bitset>
#include <stdexcept>



namespace {

std::string bytes2string(const std::vector<std::uint8_t>& bytes) {
    std::string bytes_as_string;
    bytes_as_string.reserve(bytes.size() * 8);
    for (const auto& byte : bytes) {
        std::bitset<8> byte_as_bitset(byte);
        bytes_as_string.append(byte_as_bitset.to_string());
    }
    return bytes_as_string;
}

}

namespace agent {
namespace pnc {
namespace pldm {
namespace utils {

std::uint8_t crc8(std::vector<std::uint8_t> data) {
    if (data.empty()) {
        throw std::runtime_error(std::string("CRC8 error: crc checksum function was called without input data.\n"));
    }
    data.push_back(0);
    std::string bits = bytes2string(data);

    std::bitset<16> xor_polynomial(0x0107);
    size_t polynomial_size =
        xor_polynomial.to_string().find_last_of("1") - xor_polynomial.to_string().find_first_of("1") + 1;

    unsigned i;
    for (i = 0; i <= bits.size() - polynomial_size; ++i) {
        if (bits.at(i) == '1') {
            std::bitset<16> curr_crc(bits.substr(i, polynomial_size));
            std::bitset<16> crc_after = curr_crc ^xor_polynomial;
            auto tmp_string = crc_after.to_string();
            bits.replace(i, polynomial_size, tmp_string.substr(tmp_string.size() - polynomial_size));
        }
    }

    auto result_bitset = std::bitset<16>(bits.substr(i));
    return static_cast<uint8_t>(result_bitset.to_ulong());
}

}
}
}
}