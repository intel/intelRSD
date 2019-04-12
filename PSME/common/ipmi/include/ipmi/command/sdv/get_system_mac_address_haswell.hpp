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
 * @file get_system_mac_address_haswell.hpp
 *
 * @brief Implementation of GetSystemMacAddressHaswellHaswell Request/Response.
 * */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

#include <string>
#include <map>
#include <vector>

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Represents request message for Get command.
 */
class GetSystemMacAddressHaswell : public Request {
public:

    /*!
     * @brief Default constructor.
     */
    GetSystemMacAddressHaswell();

    /*! Copy constructor */
    GetSystemMacAddressHaswell(const GetSystemMacAddressHaswell&) = default;

    /*! Assignment operator */
    GetSystemMacAddressHaswell& operator=(const GetSystemMacAddressHaswell&) = default;

    /*!
     * @brief Destructor.
     */
    virtual ~GetSystemMacAddressHaswell();

    /*!
     * @brief Packs data from object to output vector
     * @param[out] data vector where data will be packed.
     */
    virtual void pack(std::vector<std::uint8_t>& data) const override;

    /*!
     * @brief Sets ethernet interface index.
     * @param index ethernet interface index.
     */
    void set_interface_id(std::uint16_t index) {
        m_interface_id = index;
    }

    virtual const char* get_command_name() const override {
        return "GetSystemMacAddressHaswell";
    }

private:
    std::uint16_t m_interface_id{};
};
}

namespace response {

/*!
 * @brief Represents response message for GetDeviceId command.
 */
class GetSystemMacAddressHaswell : public Response {
public:

    /*!
     * @brief Default constructor.
     */
    GetSystemMacAddressHaswell();

    /*! Copy constructor */
    GetSystemMacAddressHaswell(const GetSystemMacAddressHaswell&) = default;

    /*! Assignment operator */
    GetSystemMacAddressHaswell& operator=(const GetSystemMacAddressHaswell&) = default;

    /*!
     * @brief Destructor.
     */
    ~GetSystemMacAddressHaswell();

    /*!
     * @brief Gets system mac address.
     * @return m_system_mac_address
     */
    std::string get_system_mac_address() const {
        return m_system_mac_address;
    }

    /*!
     * @brief Unpacks data from vector to object.
     * @param data bytes to be copied to object,
     */
    void unpack(const std::vector<std::uint8_t>& data) override;

    virtual const char* get_command_name() const override {
        return "GetSystemMacAddressHaswell";
    }

private:
    std::string m_system_mac_address{};

    static constexpr std::size_t RESPONSE_SIZE = 9;
    static constexpr std::size_t MAC_ADDRESS_LENGTH = 18;
    static constexpr std::size_t MAC_ADDRESS_OFFSET = 3;
};
}


}
}
}
