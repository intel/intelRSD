/*!
 * @brief Delete SEL entry (event) Request/Response.
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
 * @file delete_sel_entry.hpp
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

/*! Request message for DeleteSelEntry command */
class DeleteSelEntry : public Request {
public:
    /*! Default constructor */
    DeleteSelEntry(SelEntryId entry, SelReservationId reservation = 0);

    /*! Copy constructor */
    DeleteSelEntry(const DeleteSelEntry&) = default;

    /*! Assignment operator */
    DeleteSelEntry& operator=(const DeleteSelEntry&) = default;

    /*! @brief Destructor */
    virtual ~DeleteSelEntry();

    virtual const char* get_command_name() const override {
        return "DeleteSelEntry";
    }

private:
    /*!
     * @brief Packs data from object to output vector
     * @param[out] data vector where request data will be packed.
     */
    virtual void pack(IpmiInterface::ByteBuffer& data) const override;

    const SelEntryId entry_id;
    const SelReservationId reservation_id;
};

}

namespace response {

/*! Response message for DeleteSelEntry command */
class DeleteSelEntry : public Response {
public:
    /*! Default constructor */
    DeleteSelEntry();

    /*! Copy constructor */
    DeleteSelEntry(const DeleteSelEntry&) = default;

    /*! Assignment operator */
    DeleteSelEntry& operator=(const DeleteSelEntry&) = default;

    /*! Destructor */
    virtual ~DeleteSelEntry();

    /*!
     * @brief Get removed entry (event) ID.
     * @return removed entry (event) ID.
     */
    SelEntryId get_removed_entry() const {
        return removed_entry;
    }

    virtual const char* get_command_name() const override {
        return "DeleteSelEntry";
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

    SelEntryId removed_entry{};
};

}

}
}
}
