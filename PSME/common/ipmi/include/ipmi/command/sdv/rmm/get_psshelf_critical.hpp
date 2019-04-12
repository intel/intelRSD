/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file get_psshelf_critical.hpp
 *
 * @brief GetPsShelfCritical IPMI command request and response.
 * */

#pragma once



#include "ipmi/request.hpp"
#include "ipmi/response.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief GetPsShelfCritical request command.
 */
class GetPsShelfCritical : public Request {
public:
    /*!
     * @brief Constructor.
     */
    explicit GetPsShelfCritical();


    GetPsShelfCritical(const GetPsShelfCritical&) = default;


    GetPsShelfCritical(GetPsShelfCritical&&) = default;


    virtual ~GetPsShelfCritical();

    virtual const char* get_command_name() const override {
        return "GetPsShelfCritical";
    }

private:
    void pack(IpmiInterface::ByteBuffer&) const override {
        /* no data to send */
    }
};

}

namespace response {

/*!
 * @brief GetPsShelfCritical response command.
 */
class GetPsShelfCritical : public Response {
public:
    /*!
     * @brief Constructor.
     */
    explicit GetPsShelfCritical();


    GetPsShelfCritical(const GetPsShelfCritical&) = default;


    GetPsShelfCritical(GetPsShelfCritical&&) = default;


    virtual ~GetPsShelfCritical();


    /*!
     * @brief Checks PSShelf critical state.
     * @return True if critical state is set, false otherwise.
     */
    bool is_critical() const;

    virtual const char* get_command_name() const override {
        return "GetPsShelfCritical";
    }

private:
    static constexpr const std::size_t RESPONSE_SIZE = 2;
    bool m_is_critical{false};


    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

}

}
}
}
