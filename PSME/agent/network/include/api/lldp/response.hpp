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
 * @file response.hpp
 *
 * @brief LLDP response
 * */

#pragma once

#include <string>

namespace agent {
namespace network {
namespace api {
namespace lldp {

/* forward declaration */
class MessageHeader;

/*!
 * @brief LLDP Response
 * */
class Response final {
public:
    /*!
     * @enum Status
     * @brief Response status.
     *
     * @var Status::SUCCESS
     * LLDP response is ok
     *
     * @var Status::FAILURE
     * Error message is returned
     *
     * @var Status::NO_AGENT
     * The LLDP agent not found for given interface
     * It happens if operation status of interface is down
     * or the LLDP admin status is down
     *
     * */
    enum class Status {
        SUCCESS,
        FAILURE,
        NO_AGENT
    };

    /*!
     * @brief Default constructor
     *
     * @param[in] raw Raw message
     *
     * */
    explicit Response(const std::string& raw);

    /*!
     * @brief Pop message header form the head of the response
     *
     * @return Message header
     *
     * */
    template <typename T>
    auto pop() -> T {
        T header{};
        pop_header(header);
        return header;
    }

    /*!
     * @brief Get response message
     *
     * @return Response message
     *
     * */
    const std::string& get_status_message() const;

    /*!
     * @brief Get response status
     *
     * @return Response status
     *
     * */
    Status get_status() const;

    /*! Return status of the response */
    explicit operator bool() const;

private:
    void pop_header(MessageHeader&);

    std::string m_raw_message{};
    std::uint32_t m_status{};
};

}
}
}
}
