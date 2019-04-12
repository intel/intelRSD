/*!
 * @brief Get SEL entry (event) Request/Response.
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
 * @file get_sel_entry.hpp
 */

#pragma once

#include "ipmi/command/generic/sel/sel_defs.hpp"
#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

#include <string>
#include <vector>

namespace ipmi {
namespace command {
namespace generic {

namespace request {

/*! Request message for GetSelEntry command */
class GetSelEntry : public Request {
public:
    /*! Default constructor */
    GetSelEntry(SelEntryId entry, SelReservationId reservation = 0, std::uint8_t offset = 0, std::uint8_t amount = 0xff);

    /*! Copy constructor */
    GetSelEntry(const GetSelEntry&) = default;

    /*! Assignment operator */
    GetSelEntry& operator=(const GetSelEntry&) = default;

    /*! Destructor */
    virtual ~GetSelEntry();

    virtual const char* get_command_name() const override {
        return "GetSelEntry";
    }

private:
    /*!
     * @brief Packs data from object to output vector
     * @param[out] data vector where request data will be packed.
     */
    virtual void pack(IpmiInterface::ByteBuffer& data) const override;

    const SelEntryId entry_id;
    const SelReservationId reservation_id;
    const std::uint8_t record_offset;
    const std::uint8_t to_read;
};

}

namespace response {

/*!
 * @brief Response message for GetSelEntry command.
 */
class GetSelEntry : public Response {
public:
    /*! Default constructor */
    GetSelEntry();

    /*! Copy constructor */
    GetSelEntry(const GetSelEntry&) = default;

    /*! Assignment operator */
    GetSelEntry& operator=(const GetSelEntry&) = default;

    /*! Destructor */
    virtual ~GetSelEntry();

    /*!
     * @brief Get next entry ID
     * @return next ID of entry to be get from the SEL.
     */
    SelEntryId get_next_entry_id() const {
        return next_entry_id;
    }

    /*!
     * @brief Check if no more entries are in the log
     * @return true if no more reading is to be done
     */
    bool is_last_record() const;

    /*!
     * @brief Get just read entry from SEL
     * @return const data to be parsed
     */
    const IpmiInterface::ByteBuffer& get_entry() const {
        return entry;
    }

    virtual const char* get_command_name() const override {
        return "GetSelEntry";
    }

private:
    /*!
     * @brief Custom command completion codes handling
     * @param completion_code to select exception to throw
     * @throws ResponseError type exception.
     */
    [[noreturn]] void throw_error_on_completion_code(CompletionCode completion_code) const override;

    /*!
     * @brief Unpacks data from vector to object.
     * @param data bytes to be parsed and stored in the response,
     */
    virtual void unpack(const IpmiInterface::ByteBuffer& data) override;

    SelEntryId next_entry_id{};
    IpmiInterface::ByteBuffer entry{};
};

}

}
}
}
