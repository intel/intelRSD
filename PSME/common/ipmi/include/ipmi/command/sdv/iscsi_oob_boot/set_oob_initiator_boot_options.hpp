/*!
 * @brief Set OOB Initiator Boot Options command declaration.
 *
 * @copyright Copyright (c) 2016-2019 Intel Corporation
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
 * @file set_oob_initiator_boot_options.hpp
 */

#pragma once



#include "set_oob_boot_options.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for IPMI SetOOBInitiator Boot Options.
 */
class SetOobInitiatorBootOptions : public SetOobBootOptions {
public:

    static constexpr std::uint16_t GRANTLEY_DEFAULT_WAIT_TIME = 40;
    static constexpr std::uint16_t PURLEY_DEFAULT_WAIT_TIME = 200;
    static constexpr std::uint16_t DEFAULT_RETRY_COUNT = 16;

    /*!
     * @brief Default constructor.
     */
    SetOobInitiatorBootOptions();


    /*! Copy constructor */
    SetOobInitiatorBootOptions(const SetOobInitiatorBootOptions&) = default;


    /*! Assignment operator */
    SetOobInitiatorBootOptions& operator=(const SetOobInitiatorBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~SetOobInitiatorBootOptions();


    virtual void pack(std::vector<uint8_t>& data) const override;


    void set_offset(uint16_t offset) { m_offset = offset; }


    void set_length(uint16_t len) { m_length = len; }


    void set_wait_time(uint16_t wait_time) { m_wait_time = wait_time; }


    void set_retry_count(uint16_t count) { m_retry_count = count; }

    virtual const char* get_command_name() const override {
        return "SetOobInitiatorBootOptions";
    }

private:

    uint16_t m_offset{0};
    uint16_t m_length{0};
    uint16_t m_wait_time{0};
    uint16_t m_retry_count{0};

    static constexpr std::uint8_t INITIATOR_PARAMETER_INDEX = 0x00;

};

}

namespace response {

/*!
 * @brief Response message for IPMI SetOOBInitiator Boot option.
 */
class SetOobInitiatorBootOptions : public SetOobBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    SetOobInitiatorBootOptions();


    /*! Copy constructor */
    SetOobInitiatorBootOptions(const SetOobInitiatorBootOptions&) = default;


    /*! Assignment operator */
    SetOobInitiatorBootOptions& operator=(const SetOobInitiatorBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~SetOobInitiatorBootOptions();


    virtual void unpack(const std::vector<uint8_t>& data) override;

    virtual const char* get_command_name() const override {
        return "SetOobInitiatorBootOptions";
    }

private:
    static constexpr size_t RESPONSE_SIZE = 1;
};

}

}
}
}
