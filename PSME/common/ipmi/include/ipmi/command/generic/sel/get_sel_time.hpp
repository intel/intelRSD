/*!
 * @brief Get current SEL time
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
 * @file get_sel_time.hpp
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

/*! Request message for GetSelTime command */
class GetSelTime : public Request {
public:
    /*! Default constructor */
    GetSelTime();

    /*! Copy constructor */
    GetSelTime(const GetSelTime&) = default;

    /*! Assignment operator */
    GetSelTime& operator=(const GetSelTime&) = default;

    /*! @brief Destructor */
    virtual ~GetSelTime();

    virtual const char* get_command_name() const override {
        return "GetSelTime";
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

/*! Response message for GetSelTime command */
class GetSelTime : public Response {
public:
    /*! Default constructor */
    GetSelTime();

    /*! Copy constructor */
    GetSelTime(const GetSelTime&) = default;

    /*! Assignment operator */
    GetSelTime& operator=(const GetSelTime&) = default;

    /*! Destructor */
    virtual ~GetSelTime();

    /*!
     * @brief Get current SEL time
     * @return number of seconds since SEL service started
     */
    SelTimestamp get_sel_time() const {
        return m_sel_time;
    }

    virtual const char* get_command_name() const override {
        return "GetSelTime";
    }

private:
    /*!
     * @brief Unpacks data from vector to object.
     * @param data bytes to be parsed and stored in the response,
     */
    virtual void unpack(const IpmiInterface::ByteBuffer& data) override;

    SelTimestamp m_sel_time{};
};

}

}
}
}
