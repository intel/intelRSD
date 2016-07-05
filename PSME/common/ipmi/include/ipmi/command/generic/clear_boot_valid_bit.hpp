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
 * @file clear_boot_valid_bit.hpp
 *
 * @brief Clear Boot Valid Bit Command Declaration.
 * */

#ifndef IPMI_COMMAND_GENERIC_CLEAR_BOOT_VALID_BIT_HPP
#define IPMI_COMMAND_GENERIC_CLEAR_BOOT_VALID_BIT_HPP

#include "ipmi/command/generic/set_system_boot_options.hpp"

namespace ipmi {
namespace command {
namespace generic {

namespace request {

/*!
 * @brief Request message for IPMI Clear Boot Valid Bit.
 */
class ClearBootValidBit: public request::SetSystemBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    ClearBootValidBit();

    /*! Copy constructor */
    ClearBootValidBit(const ClearBootValidBit&) = default;

    /*! Assignment operator */
    ClearBootValidBit& operator=(const ClearBootValidBit&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~ClearBootValidBit();

    virtual void pack(vector<uint8_t>& data) const;

private:

};

}

namespace response {

/*!
 * @brief Response message for IPMI Clear Boot Valid Bit.
 */
class ClearBootValidBit: public response::SetSystemBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    ClearBootValidBit();

    /*! Copy constructor */
    ClearBootValidBit(const ClearBootValidBit&) = default;

    /*! Assignment operator */
    ClearBootValidBit& operator=(const ClearBootValidBit&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~ClearBootValidBit();

    virtual void unpack(const vector<uint8_t>& data);

private:
    static constexpr size_t RESPONSE_SIZE = 1;
};

}

}
}
}
#endif	/* IPMI_COMMAND_GENERIC_CLEAR_BOOT_VALID_BIT_HPP */
