/*!
 * @brief Get OOB Initiator Boot Options command declaration.
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
 * @file get_oob_initiator_boot_options.hpp
 */

#pragma once



#include "get_oob_boot_options.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for IPMI GetOOBInitiator Boot Options.
 */
class GetOobInitiatorBootOptions : public GetOobBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    GetOobInitiatorBootOptions(uint8_t index = 0);


    /*! Copy constructor */
    GetOobInitiatorBootOptions(const GetOobInitiatorBootOptions&) = default;


    /*! Assignment operator */
    GetOobInitiatorBootOptions& operator=(const GetOobInitiatorBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~GetOobInitiatorBootOptions();


    virtual void pack(std::vector<uint8_t>& data) const override;

    virtual const char* get_command_name() const override {
        return "GetOobInitiatorBootOptions";
    }
};

}

namespace response {

/*!
 * @brief Response message for IPMI GetOOBInitiator Boot option.
 */
class GetOobInitiatorBootOptions : public GetOobBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    GetOobInitiatorBootOptions();


    /*! Copy constructor */
    GetOobInitiatorBootOptions(const GetOobInitiatorBootOptions&) = default;


    /*! Assignment operator */
    GetOobInitiatorBootOptions& operator=(const GetOobInitiatorBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~GetOobInitiatorBootOptions();


    virtual void unpack(const std::vector<uint8_t>& data) override;


    uint16_t get_name_offset() const { return m_name_offset; }


    uint16_t get_name_length() const { return m_name_length; }


    uint16_t get_wait_time() const { return m_wait_time; }


    uint16_t get_retry_count() const { return m_retry_count; }


    size_t v2_payload_size() override { return V2_RESPONSE_SIZE; }


    std::string what() override { return "Initiator"; }

    virtual const char* get_command_name() const override {
        return "GetOobInitiatorBootOptions";
    }

private:

    void set_name_offset(uint16_t offset) { m_name_offset = offset; }


    void set_name_length(uint16_t len) { m_name_length = len; }


    void set_wait_time(uint16_t wait_time) { m_wait_time = wait_time; }


    void set_retry_count(uint16_t retry_count) { m_retry_count = retry_count; }


    uint16_t m_name_offset{};
    uint16_t m_name_length{};
    uint16_t m_wait_time{};
    uint16_t m_retry_count{};

    static constexpr size_t OFFSET_NAME = OFFSET_INDEX + 1;
    static constexpr size_t OFFSET_NAME_LEN = OFFSET_NAME + 2;
    static constexpr size_t OFFSET_WAIT_TIME = OFFSET_NAME_LEN + 2;
    static constexpr size_t OFFSET_RETRY_COUNT = OFFSET_WAIT_TIME + 2;
    static constexpr size_t V2_RESPONSE_SIZE = OFFSET_RETRY_COUNT + 2;

};

}

}
}
}
