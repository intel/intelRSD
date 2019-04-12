/*!
* @brief definition of SMBus exceptions
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
* @file smbus_error.cpp
*/

#include "protocol/smbus/smbus_error.hpp"



using namespace agent::pnc::smbus;


InvalidSmbusFrameSize::InvalidSmbusFrameSize(const std::string& what_arg) : std::runtime_error(
    "Invalid SMBus frame size. " + what_arg) {}


InvalidSmbusFrameSize::~InvalidSmbusFrameSize() {}


InvalidSmbusCrc::InvalidSmbusCrc(const std::string& what_arg, std::uint64_t computed_crc,
                                 std::uint64_t received_crc) : std::runtime_error(
    "SMBus error: invalid CRC checksum. Computed CRC: " + std::to_string(computed_crc) + ", received CRC: " +
    std::to_string(received_crc) + ". " + what_arg) {}


InvalidSmbusCrc::~InvalidSmbusCrc() {}