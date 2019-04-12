/*!
 * @brief Set OOB Control Boot Options command declaration.
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
 * @file set_oob_control_boot_options.hpp
 */

#pragma once



#include "set_oob_boot_options.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for IPMI SetOOBControl Boot Options.
 */
class SetOobControlBootOptions : public SetOobBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    SetOobControlBootOptions();


    /*! Copy constructor */
    SetOobControlBootOptions(const SetOobControlBootOptions&) = default;


    /*! Assignment operator */
    SetOobControlBootOptions& operator=(const SetOobControlBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~SetOobControlBootOptions();


    virtual void pack(std::vector<uint8_t>& data) const override;


    void set_bios_enabled(bool enabled) { m_bios_enabled = enabled; }

    virtual const char* get_command_name() const override {
        return "SetOobControlBootOptions";
    }

private:

    bool m_bios_enabled{false};

    static constexpr std::uint8_t CONTROL_PARAMETER_INDEX = 0x00;
    static constexpr std::size_t BIOS_BOOT_FLAG_BYTES = 4;
    static constexpr std::uint8_t ISCSI_DISABLED_IN_BIOS[BIOS_BOOT_FLAG_BYTES] = {0x00, 0x00, 0x00, 0x00};
    static constexpr std::uint8_t ISCSI_ENABLED_IN_BIOS[BIOS_BOOT_FLAG_BYTES] = {0x01, 0x00, 0x00, 0x00};

};

}

namespace response {

/*!
 * @brief Response message for IPMI SetOOBControl Boot option.
 */
class SetOobControlBootOptions : public SetOobBootOptions {
public:

    /*!
     * @brief Default constructor.
     */
    SetOobControlBootOptions();


    /*! Copy constructor */
    SetOobControlBootOptions(const SetOobControlBootOptions&) = default;


    /*! Assignment operator */
    SetOobControlBootOptions& operator=(const SetOobControlBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~SetOobControlBootOptions();


    virtual void unpack(const std::vector<uint8_t>& data) override;

    virtual const char* get_command_name() const override {
        return "SetOobControlBootOptions";
    }

private:
    static constexpr size_t RESPONSE_SIZE = 1;
};

}

}
}
}
