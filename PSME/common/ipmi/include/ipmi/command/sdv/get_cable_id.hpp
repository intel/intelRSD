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
 * @file get_cable_id.hpp
 *
 * @brief Get Cable Id request and response.
 * */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

#include <cstdint>
#include <vector>
#include <string>

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for Get Cable Id command.
 */
class GetCableId : public Request {
public:

    static constexpr std::size_t MAX_CHANNEL_NUMBER = 4;

    /*!
     * @brief Constructor.
     */
    GetCableId();

    /*! Copy constructor. */
    GetCableId(const GetCableId&) = default;

    /*! Assignment operator */
    GetCableId& operator=(const GetCableId&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetCableId();

    virtual void pack(std::vector<std::uint8_t>& data) const override;

    /*!
     * @brief Set channel on which the cable id will be read
     *
     * @param channel from 0 to 4
     * */
    virtual void set_channel(size_t channel);

    virtual const char* get_command_name() const override {
        return "GetCableId";
    }

private:
    std::size_t CHANNEL = 0x00;
    static constexpr std::size_t OFFSET_CABLE_ID = 0xC4;
    static constexpr std::size_t CABLE_ID_LENGTH = 16;

};
}

namespace response {

/*!
 * @brief Response message for Get Cable Id command. Gets Cable Id.
 */
class GetCableId : public Response {
public:

    /*!
     * @brief Constructor.
     */
    GetCableId();

    /*! Copy constructor. */
    GetCableId(const GetCableId&) = default;

    /*! Assignment operator */
    GetCableId& operator=(const GetCableId&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetCableId();

    virtual void unpack(const std::vector<std::uint8_t>& data) override;

    /*!
     * @brief Gets cable ID
     * @return Cable ID
     */
    const std::string& get_cable_id() const {
        return m_cable_id;
    }

    virtual const char* get_command_name() const override {
        return "GetCableId";
    }

private:
    static constexpr std::size_t RESPONSE_SIZE = 17;
    static constexpr std::size_t CHANNEL = 0x0;
    static constexpr std::size_t OFFSET_CABLE_ID = 0x01;
    static constexpr std::size_t CABLE_ID_LENGTH = 16;

    std::string m_cable_id{};
    std::string extract_cable_id(const std::vector<std::uint8_t>& data);
};
}

}
}
}
