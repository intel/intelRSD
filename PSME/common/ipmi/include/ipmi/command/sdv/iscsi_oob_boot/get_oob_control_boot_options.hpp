/*!
 * @brief Get OOB Control Boot Options command declaration.
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
 * @file get_oob_control_boot_options.hpp
 */

#pragma once


#include "get_oob_boot_options.hpp"


namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for IPMI GetOOBControl Boot Options.
 */
class GetOobControlBootOptions: public GetOobBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    GetOobControlBootOptions(uint8_t index = 0);

    /*! Copy constructor */
    GetOobControlBootOptions(const GetOobControlBootOptions&) = default;

    /*! Assignment operator */
    GetOobControlBootOptions& operator=(const GetOobControlBootOptions&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetOobControlBootOptions();

    virtual void pack(std::vector<uint8_t>& data) const override;

    virtual const char* get_command_name() const override {
        return "GetOobControlBootOptions";
    }
};

}

namespace response {

/*!
 * @brief Response message for IPMI GetOOBControl Boot option.
 */
class GetOobControlBootOptions : public GetOobBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    GetOobControlBootOptions();

    /*! Copy constructor */
    GetOobControlBootOptions(const GetOobControlBootOptions&) = default;

    /*! Assignment operator */
    GetOobControlBootOptions& operator=(const GetOobControlBootOptions&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetOobControlBootOptions();

    virtual void unpack(const std::vector<uint8_t>& data) override;

    bool get_bios_enabled() const { return m_bios_enabled; }

    size_t v2_payload_size() override { return V2_RESPONSE_SIZE; }

    std::string what() override { return "Control"; }

    virtual const char* get_command_name() const override {
        return "GetOobControlBootOptions";
    }

private:

    void set_bios_enabled(bool enabled) { m_bios_enabled = enabled; }

    bool m_bios_enabled{false};

    static constexpr size_t OFFSET_BIOS_ENABLED = OFFSET_INDEX + 1;
    static constexpr size_t BIOS_ENABLED_MASK = 1;
    static constexpr size_t V2_RESPONSE_SIZE = OFFSET_BIOS_ENABLED + 4;
};

}

}
}
}
