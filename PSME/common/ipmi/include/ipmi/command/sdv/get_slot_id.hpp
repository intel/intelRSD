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
 * @file get_slot_id.hpp
 *
 * @brief Implementation of GetSlotId Request and Response.
 * */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

#include <cstdint>
#include <stdexcept>
#include <vector>

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Represents request message for Get Slot Id command.
 */
class GetSlotId : public Request {
public:

    /*!
     * @brief Default constructor.
     */
    GetSlotId();

    /*! Copy constructor. */
    GetSlotId(const GetSlotId&) = default;

    /*! Assignment operator */
    GetSlotId& operator=(const GetSlotId&) = default;

    void pack(std::vector<std::uint8_t>& data) const final {
        // GetSlotId without request parameters.
        (void)data;
    }

    /*!
     * @brief Default destructor
     */
    virtual ~GetSlotId();

    virtual const char* get_command_name() const override {
        return "GetSlotId";
    }
};

}

namespace response {

/*!
 * @brief Represents response message for Get Slot Id command.
 */
class GetSlotId final : public Response {
public:

    /*!
     * @brief Default constructor.
     */
    GetSlotId();

    /*! Copy constructor. */
    GetSlotId(const GetSlotId&) = default;

    /*! Assignment operator */
    GetSlotId& operator=(const GetSlotId&) = default;

    void unpack(const std::vector<std::uint8_t>& data) final;

    /*!
     * @brief Default destructor
     */
    virtual ~GetSlotId();

    /*!
     * @brief Gets Slot Id
     * @return Slot Id
     */
    std::uint8_t get_slot_id() const;

    /*!
     * @brief Gets Board Id
     * @return Board Id
     */
    std::uint8_t get_board_id() const;

    /*!
     * @brief Gets Revision Id
     * @return Revision Id
     */
    std::uint8_t get_revision_id() const;

    /*!
     * @brief Gets Reserved Id
     * @return Reserved Id
     */
    std::uint8_t get_reserved_id() const;

    virtual const char* get_command_name() const override {
        return "GetSlotId";
    }

private:
    std::uint8_t m_slot_id{};
    std::uint8_t m_board_id{};
    std::uint8_t m_revision_id{};
    std::uint8_t m_reserved_id{};

    static constexpr std::size_t OFFSET_SLOT_ID = 1;
    static constexpr std::size_t OFFSET_BOARD_ID = 2;
    static constexpr std::size_t OFFSET_REVISION_ID = 3;
    static constexpr std::size_t OFFSET_RESERVED_ID = 4;
    static constexpr std::size_t RESPONSE_SIZE = 5;
};
}

}
}
}


