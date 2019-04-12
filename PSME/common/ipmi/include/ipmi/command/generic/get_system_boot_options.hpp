/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file get_system_boot_options.hpp
 *
 * @brief IPMI Get Chassis Boot Options Command.
 * */

#pragma once



#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "enums.hpp"
#include "boot_options.hpp"

#include <cstdint>
#include <vector>
#include <map>



namespace ipmi {
namespace command {
namespace generic {

namespace request {

/*!
 * @brief Request message for IPMI Get Chassis Boot Options Command.
 */
class GetSystemBootOptions : public Request {
public:

    /*!
     * @brief Default constructor.
     */
    GetSystemBootOptions();


    /*! Copy constructor. */
    GetSystemBootOptions(const GetSystemBootOptions&) = default;


    /*! Assignment operator */
    GetSystemBootOptions& operator=(const GetSystemBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~GetSystemBootOptions();


    virtual void pack(std::vector<std::uint8_t>& data) const override;

    virtual const char* get_command_name() const override {
        return "GetSystemBootOptions";
    }


private:
    static constexpr std::uint8_t REQ_PARAM_SEL = 0x05;
    static constexpr std::uint8_t REQ_SET_SEL = 0x00;
    static constexpr std::uint8_t REQ_BLOCK_SEL = 0x00;
};
}

namespace response {

/*!
 * @brief Response message for IPMI Get Chassis Boot Options Command.
 *
 * Response includes information about current boot order overrides:
 * mode (is it one-time override or is it continuous) and target device.
 * Keep in mind that in any case, overrides are only temporary and last approximately 30s.
 * After that time this method always returns Disabled/None. This is a proper behaviour of the BMC.
 * In case of One-time overrides, after 30s, overrides are removed. In case of
 * continuous overrides, the boot order is altered thus no overrides are in fact present.
 * */
class GetSystemBootOptions : public Response {
public:
    /*!
     * @brief Default constructor.
     */
    GetSystemBootOptions();


    /*! Copy constructor. */
    GetSystemBootOptions(const GetSystemBootOptions&) = default;


    /*! Assignment operator */
    GetSystemBootOptions& operator=(const GetSystemBootOptions&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~GetSystemBootOptions();


    /*!
     * Returns current boot override setting
     * @return Boot override setting (Once/Continuous/Disabled)
     */
    BootOverride get_boot_override() const {
        return m_boot_override;
    }


    /*!
     * Returns current boot override target
     * @return Boot override target (Hdd/Pxe/None/Unknown)
     */
    BootOverrideTarget get_boot_override_target() const {
        return m_boot_override_target;
    }


    /*!
     * Returns current boot mode
     * @return Boot mode (Hdd/Pxe/None/Unknown)
     */
    BootMode get_boot_mode() const {
        return m_boot_mode;
    }


    virtual void unpack(const std::vector<std::uint8_t>& data) override;


    virtual const char* get_command_name() const override {
        return "GetSystemBootOptions";
    }

private:

    BootOverride m_boot_override{BootOverride::Disabled};
    BootOverrideTarget m_boot_override_target{BootOverrideTarget::None};
    BootMode m_boot_mode{BootMode::Legacy};

    static constexpr std::size_t RESPONSE_SIZE = 8;
    static constexpr std::size_t OFFSET_PERSISTENT = 3;
    static constexpr std::size_t OFFSET_DEVICE = 4;

};
}

}
}
}
