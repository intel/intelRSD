/*!
 * @brief Get SEL reservation ("lock") Request/Response.
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
 * @file reserve_sel.hpp
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

/*! Request message for ReserveSel command */
class ReserveSel : public Request {
public:
    /*! Default constructor */
    ReserveSel();

    /*! Copy constructor */
    ReserveSel(const ReserveSel&) = default;

    /*! Assignment operator */
    ReserveSel& operator=(const ReserveSel&) = default;

    /*! Destructor */
    virtual ~ReserveSel();

    virtual const char* get_command_name() const override {
        return "ReserveSel";
    }

private:
    /*!
     * @brief Packs data from object to output vector
     * @param[out] data vector where request data will be packed.
     */
    virtual void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {

/*! Response message for ReserveSel command */
class ReserveSel : public Response {
public:
    /*! Default constructor */
    ReserveSel();

    /*! Copy constructor */
    ReserveSel(const ReserveSel&) = default;

    /*! Assignment operator */
    ReserveSel& operator=(const ReserveSel&) = default;

    /*! Destructor */
    virtual ~ReserveSel();


    /*!
     * @brief Get reservation id to be used in another request
     * @return reservation id
     */
    SelReservationId get_reservation_id() const {
        return reservation_id;
    }

    virtual const char* get_command_name() const override {
        return "ReserveSel";
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

    SelReservationId reservation_id{};
};

}

}
}
}
