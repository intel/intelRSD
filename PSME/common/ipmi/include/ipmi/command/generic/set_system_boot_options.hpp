/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 *
 * @file set_system_boot_options.hpp
 *
 * @brief Set System Boot Options Command Declaration.
 * This is a base class for all commands which have something to do with boot,
 * BIOS configuration and early startup options. This class should be extended
 * to more specialized classes which can perform concrete operation on platform.
 *
 * */

#ifndef IPMI_COMMAND_GENERIC_SET_SYSTEM_BOOT_OPTIONS_HPP
#define IPMI_COMMAND_GENERIC_SET_SYSTEM_BOOT_OPTIONS_HPP

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

namespace ipmi {
namespace command {
namespace generic {

namespace request {

/*!
 * @brief Request message for IPMI Set System Boot Options.
 */
class SetSystemBootOptions: public Request {
public:

    /*!
     * @brief Represents ParameterValidity field in request.
     */
    enum class ParameterValidity: std::uint8_t {
        VALID = 0x00,
        INVALID = 0x01,
    };

    /*!
     * @brief Represents Parameter Selector. Indicates which options is going to
     * set.
     */
    enum class ParameterSelector: std::uint8_t {
        SET_IN_PROGRES = 0x00,
        SERVICE_PARTITION_SELECTOR = 0x01,
        SERVICE_PARTITION_SCAN = 0x02,
        BMC_BOOT_FLAG_VALID_BIT_CLEARING = 0x03,
        BOOT_INFO_ACKNOWLEDGE = 0x04,
        BOOT_FLAGS = 0x05,
        BOOT_INITIATOR_INFO = 0x06,
        BOOT_INITIATOR_MAILBOX = 0x07,
    };

    /*!
     * @brief Default constructor.
     */
    SetSystemBootOptions();

    /*! Copy constructor */
    SetSystemBootOptions(const SetSystemBootOptions&) = default;

    /*! Assignment operator */
    SetSystemBootOptions& operator=(const SetSystemBootOptions&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~SetSystemBootOptions();

    virtual void pack(vector<uint8_t>& data) const = 0;
};

}

namespace response {

/*!
 * @brief Response message for IPMI Set System Boot Options.
 */
class SetSystemBootOptions: public Response {
public:

    /*!
     * @brief Default constructor.
     */
    SetSystemBootOptions();

    /*! Copy constructor */
    SetSystemBootOptions(const SetSystemBootOptions&) = default;

    /*! Assignment operator */
    SetSystemBootOptions& operator=(const SetSystemBootOptions&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~SetSystemBootOptions();

    virtual void unpack(const vector<uint8_t>& data) = 0;

private:
    static constexpr size_t RESPONSE_SIZE = 1;
};

}

}
}
}
#endif	/* IPMI_COMMAND_GENERIC_SET_SYSTEM_BOOT_OPTIONS_HPP */
