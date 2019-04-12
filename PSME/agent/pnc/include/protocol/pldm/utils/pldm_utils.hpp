/*!
* @brief PLDM utility functions used to support pldm protocol processing
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
* @file pldm_utils.hpp
*/

#pragma once



#include <cstdint>
#include <vector>



namespace agent {
namespace pnc {
namespace pldm {
namespace utils {

/*!
 * @brief Computes CRC8 checksum according to SMBus specification
 * @param[in] data vector of smbus frame bytes
 * @return crc8 checksum value
 */
std::uint8_t crc8(std::vector<std::uint8_t> data);

}
}
}
}
