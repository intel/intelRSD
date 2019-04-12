/*!
* @brief SMBus exceptions declaration
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
* @file smbus_error.hpp
*/

#pragma once



#include <exception>
#include <stdexcept>
#include <string>



/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! PLDM namespace */
namespace smbus {

/*!
 * @brief SMBus exception type that will be thrown when SMBus frane size is invalid.
 */
class InvalidSmbusFrameSize : public std::runtime_error {
public:
    /*!
     * @brief Constructs exception object
     * @param[in] what_arg Describes what happened
     */
    InvalidSmbusFrameSize(const std::string& what_arg);


    InvalidSmbusFrameSize(const InvalidSmbusFrameSize&) = default;


    InvalidSmbusFrameSize& operator=(const InvalidSmbusFrameSize&) = default;


    InvalidSmbusFrameSize(InvalidSmbusFrameSize&&) = default;


    virtual ~InvalidSmbusFrameSize();
};

/*!
 * @brief Base Exception type that will be thrown when computed CRC checksum does not equal received CRC checksum.
 */
class InvalidSmbusCrc : public std::runtime_error {
public:
    /*!
     * @brief Constructs exception object
     * @param[in] what_arg Describes what happened
     * @param[in] computed_crc CRC checksum computed from received bytes
     * @param[in] received_crc CRC checksum received
     */
    InvalidSmbusCrc(const std::string& what_arg, std::uint64_t computed_crc, std::uint64_t received_crc);


    InvalidSmbusCrc(const InvalidSmbusCrc&) = default;


    InvalidSmbusCrc& operator=(const InvalidSmbusCrc&) = default;


    InvalidSmbusCrc(InvalidSmbusCrc&&) = default;


    virtual ~InvalidSmbusCrc();
};

}
}
}