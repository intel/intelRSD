/*!
 * @brief Declaration of Reserve SDR Repository Request/Response.
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
 * @file command/generic/reserve_sdr_repository.hpp
 */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

namespace ipmi {
namespace command {
namespace generic {

namespace request {

/*!
 * @brief Request message for Reserve SDR Repository command.
 */
class ReserveSdrRepository : public Request {
public:
    /*!
     * @brief Default constructor.
     */
    ReserveSdrRepository();

    /*! Copy constructor. */
    ReserveSdrRepository(const ReserveSdrRepository&) = default;

    /*! Assignment operator */
    ReserveSdrRepository& operator=(const ReserveSdrRepository&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~ReserveSdrRepository();

    const char* get_command_name() const override {
        return "ReserveSdrRepository";
    }

private:
    void pack(IpmiInterface::ByteBuffer&) const override {
        /* no request data */
    }
};

}

namespace response {

/*!
 * @brief Response message for Reserve SDR Repository command.
 */
class ReserveSdrRepository : public Response {
public:

    /*!
     * @brief Default constructor.
     */
    ReserveSdrRepository();

    /*! Copy constructor. */
    ReserveSdrRepository(const ReserveSdrRepository&) = default;

    /*! Assignment operator */
    ReserveSdrRepository& operator=(const ReserveSdrRepository&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~ReserveSdrRepository();

    const char* get_command_name() const override {
        return "ReserveSdrRepository";
    }

    /*!
     * @brief Gets reservation id.
     * @return Reservation id.
     */
    std::uint16_t get_reservation_id() const {
        return m_reservation_id;
    }

private:
    static constexpr std::size_t RESPONSE_SIZE = 3;

    std::uint16_t m_reservation_id{};

    void unpack(const IpmiInterface::ByteBuffer& data) override;
};
}

}
}
}

