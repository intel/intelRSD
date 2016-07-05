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
 * @file get_system_mac_address_haswell.hpp
 *
 * @brief Implementation of GetSystemMacAddressHaswellHaswell Request/Response.
 * */

#ifndef IPMI_COMMAND_SDV_GET_SYSTEM_MAC_ADDRESS_HASWELL_HPP
#define	IPMI_COMMAND_SDV_GET_SYSTEM_MAC_ADDRESS_HASWELL_HPP

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

#include <string>
#include <map>
#include <vector>

using std::size_t;
using std::uint8_t;
using std::string;
using std::to_string;
using std::vector;
using std::map;

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Represents request message for Get command.
 */
class GetSystemMacAddressHaswell: public Request {
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

    /**
     * @brief Packs data from object to output vector
     * @param[out] data vector where data will be packed.
     */
    virtual void pack(vector<uint8_t>& data) const;

    /*!
     * @brief Sets ethernet interafce index.
     * @param index etherenet interafce index.
     */
    void set_interface_id(uint16_t index) {
        m_interface_id = index;
    }

private:
    uint16_t m_interface_id{};
};
}

namespace response {

/*!
 * @brief Represents response message for GetDeviceId command.
 */
class GetSystemMacAddressHaswell: public Response {
public:

    /**
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
    std::string get_system_mac_address() {
        return m_system_mac_address;
    }

    /*!
     * @brief Sets system mac address.
     * @param system_mac_address of device.
     */
    void set_system_mac_address(const std::string system_mac_address){
        m_system_mac_address = system_mac_address;
    }

    /*!
     * @brief Unpacks data from vector to object.
     * @param data bytes to be copied to object,
     */
    void unpack(const vector<uint8_t>& data);

private:
    std::string m_system_mac_address{};

    static constexpr size_t RESPONSE_SIZE = 9;
    static constexpr size_t MAC_ADDRESS_LENGTH = 18;
    static constexpr size_t MAC_ADDRESS_OFFSET = 3;
};
}


}
}
}

#endif	/* IPMI_COMMAND_SDV_GET_SYSTEM_MAC_ADDRESS_HASWELL_HPP */
