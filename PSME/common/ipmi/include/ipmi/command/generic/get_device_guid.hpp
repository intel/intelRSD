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
 * @file get_device_guid.hpp
 *
 * @brief Implementation of GetDeviceGuid Request/Response.
 * */

#ifndef IPMI_COMMAND_GENERIC_GET_DEVICE_GUID_HPP
#define	IPMI_COMMAND_GENERIC_GET_DEVICE_GUID_HPP

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

#include <string>
#include <map>
#include <vector>

namespace ipmi {
namespace command {
namespace generic {

namespace request {

/*!
 * @brief Represents request message for GetDeviceGuid command.
 */
class GetDeviceGuid: public Request {
public:

    /*!
     * @brief Default constructor.
     */
    GetDeviceGuid();

    /*! Copy constructor */
    GetDeviceGuid(const GetDeviceGuid &) = default;

    /*! Assignment operator */
    GetDeviceGuid &operator=(const GetDeviceGuid &) = default;

    /*!
     * @brief Destructor.
     */
    virtual ~GetDeviceGuid();

    /*!
     * @brief Packs data from object to output vector
     * @param[out] data vector where data will be packed.
     */
    virtual void pack(std::vector <std::uint8_t> &data) const;

};
}

namespace response {

/*!
 * @brief Represents response message for GetDeviceGuid command.
 */
class GetDeviceGuid: public Response {
public:

    /*!
     * @brief Default constructor.
     */
    GetDeviceGuid();

    /*! Copy constructor */
    GetDeviceGuid(const GetDeviceGuid&) = default;

    /*! Assignment operator */
    GetDeviceGuid& operator=(const GetDeviceGuid&) = default;

    /*!
     * @brief Destructor.
     */
    virtual ~GetDeviceGuid();

    /*!
     * @brief Unpacks data from vector to object.
     * @param data bytes to be copied to object,
     */
    virtual void unpack(const vector<std::uint8_t>& data);

    const std::string& get_guid() const {
        return m_guid;
    }

private:
    static constexpr std::uint32_t OFFSET_NODE = 1;
    static constexpr std::uint32_t OFFSET_CLK_SEQ_AND_RESERVED = 7;
    static constexpr std::uint32_t OFFSET_TIME_HIGH_AND_VER = 9;
    static constexpr std::uint32_t OFFSET_TIME_MID = 11;
    static constexpr std::uint32_t OFFSET_TIME_LOW = 13;

    static constexpr std::size_t RESPONSE_SIZE = 16;
    static constexpr std::uint32_t GUID_LENGTH = 37;

    std::string m_guid{};

};
}

}
}
}
#endif  /* IPMI_COMMAND_GENERIC_GET_DEVICE_GUID_HPP */
