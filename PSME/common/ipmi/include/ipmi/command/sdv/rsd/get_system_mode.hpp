/*!
 * @brief GetSystemMode command interface.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file get_system_mode.hpp
 */

#pragma once



#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/sdv/enums.hpp"



namespace ipmi {
namespace command {
namespace sdv {
namespace rsd {

namespace request {

/*!
 * @brief GetSystemMode request.
 */
class GetSystemMode : public Request {
public:
    GetSystemMode();


    GetSystemMode(const GetSystemMode&) = default;


    GetSystemMode(GetSystemMode&&) = default;


    GetSystemMode& operator=(const GetSystemMode&) = default;


    GetSystemMode& operator=(GetSystemMode&&) = default;


    virtual const char* get_command_name() const override {
        return "GetSystemMode";
    }


    virtual ~GetSystemMode();


private:
    virtual void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {

/*!
 * @brief GetSystemMode response.
 */
class GetSystemMode : public Response {
public:

    GetSystemMode();


    GetSystemMode(const GetSystemMode&) = default;


    GetSystemMode(GetSystemMode&&) = default;


    GetSystemMode& operator=(const GetSystemMode&) = default;


    GetSystemMode& operator=(GetSystemMode&&) = default;


    virtual ~GetSystemMode();


    /*!
     * @brief Check set system mode.
     * @return System mode.
     */
    SystemMode get_system_mode() const {
        return m_admin_mode;
    }


    /*!
     * @brief Check if RackScale mode is enabled.
     * @return True if yes, false otherwise.
     */
    bool is_rackscale_mode_enabled() const {
        return m_is_rack_scale_mode_enabled;
    }


    /*!
     * @brief Check if RackScale extension byte is present in the response.
     * @return True if extension byte is present, false otherwise.
     */
    bool is_rackscale_extension_byte_present() const {
        return m_is_rackscale_extension_byte_present;
    }


    virtual const char* get_command_name() const override {
        return "GetSystemMode";
    }


private:
    void unpack(const IpmiInterface::ByteBuffer& data) override;


    static constexpr size_t RESPONSE_SIZE = 4;
    static constexpr size_t RACKSCALE_EXTENSION_BYTE_OFFSET = 1;
    static constexpr size_t ADMIN_MODE_OFFSET = 2;
    static constexpr size_t RACKSCALE_MODE_OFFSET = 3;

    SystemMode m_admin_mode{};
    bool m_is_rackscale_extension_byte_present{false};
    bool m_is_rack_scale_mode_enabled{false};
};

}
}

}
}
}
