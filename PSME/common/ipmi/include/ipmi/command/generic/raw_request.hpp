/*!
 * @brief Declaration of raw ipmi Request/Response.
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
 * @file ipmi/command/generic/raw_request.hpp
 */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

namespace ipmi {
namespace command {
namespace generic {

namespace request {

/*!
 * @brief Represents request message containing raw bytes.
 */
class RawRequest : public Request {
public:

    /*!
     * @brief Default constructor.
     * @param name Command name
     * @param request Raw request data as byte array.
     */
    RawRequest(const std::string& name, IpmiInterface::ByteBuffer request);

    /*! Copy constructor */
    RawRequest(const RawRequest&) = default;

    /*! Assignment operator */
    RawRequest& operator=(const RawRequest&) = default;

    const char* get_command_name() const override {
        return m_name.c_str();
    }

    /*!
     * @brief Destructor.
     */
    virtual ~RawRequest();

private:
    const std::string m_name{};
    IpmiInterface::ByteBuffer raw;

    void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {

/*!
 * @brief Represents response message for RawRequest command.
 */
class RawResponse: public Response {
public:

    /*!
     * @brief Default constructor.
     * @param name Command name
     */
    RawResponse(const std::string& name);

    /*! Copy constructor */
    RawResponse(const RawResponse&) = default;

    /*! Assignment operator */
    RawResponse& operator=(const RawResponse&) = default;

    /*!
     * @brief Destructor.
     */
    virtual ~RawResponse();

    const char* get_command_name() const override {
        return m_name.c_str();
    }

    /* @return bytes of ipmi response */
    const IpmiInterface::ByteBuffer& get_data() const {
        return raw;
    }

    template<typename T>
    T convert_to_struct() {
        static_assert(std::is_pod<T>::value, "Invalid struct type");
        if (sizeof(T) > raw.size()) {
            throw std::runtime_error("conversion error, struct size exceeds data size");
        }
        return *reinterpret_cast<const T*>(raw.data());
    }

private:
    const std::string m_name{};
    IpmiInterface::ByteBuffer raw{};

    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

}

}
}
}
