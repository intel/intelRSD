/*
 * Copyright (C) 2014 MediaSift Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
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
 * */

#pragma once

#include "psme/rest/server/methods.hpp"
#include "psme/rest/server/parameters.hpp"

#include <unordered_map>

namespace psme {
namespace rest {
namespace server {

/*!
 * @brief Represents a HTTP request.
 *
 * This class represents a parsed HTTP request, composed of the method (eg. GET,
 * PUT, POST, etc..), the version (eg. 1.1), the uri (eg. /endpoint/get?start=0&len=5#item1),
 * the headers (eg. 'Content-Type: application/json'), the body, and the source.
 *
 * This class is typically created by the request parser when a request is
 * received. The new object is then passed to method handlers that were registered
 * by the user to the multiplexer.
 * */
class Request {
public:
    /*!
     * @brief Set the HTTP method of this request.
     * @param method the HTTP method
     * */
    void set_method(const Method method);

    /*!
     * @brief Set the destination of this request.
     *
     * The destination is the URL path of the request, used to determine which resource is being
     * requested.
     *
     * @param destination the URI of the request.
     * */
    void set_destination(const std::string& destination);

    /*!
     * @brief Set the HTTP version of this request.
     *
     * Sets the HTTP protocol version of this request.
     *
     * @param http_version the HTTP protocol version.
     * */
    void set_HTTP_version(const std::string& http_version);

    /*!
     * @brief Set the source of this request.
     *
     * Sets the address to be associated with the client of this request.
     *
     * @param source the source address.
     * */
    void set_source(const std::string& source);

    /*!
     * @brief Set a header value of this request.
     * @param header the key of the header to be set.
     * @param value the value of the header.
     * */
    void set_header(const std::string& header, const std::string& value);

    /*!
     * @brief Set the body of the request.
     * @param body the body of the request.
     * */
    void set_body(const std::string& body);

    /*!
     * @brief Appends body data.
     * @param[in] body data to be appended.
     * */
    void append_body(const std::string& body);

    /*!
     * @brief Set flag indicating if request is over ssl.
     * @param is_request_secure request is executed over ssl flag.
     * */
    void set_secure(bool is_request_secure);

    /*!
     * @brief Get the HTTP method of the request.
     * @return HTTP method of the request.
     * */
    Method get_method() const;

    /*!
     * @brief Get the HTTP version of the request.
     * @return HTTP version of the request.
     * */
    const std::string& get_HTTP_version() const;

    /*!
     * @brief Get the source of this request.
     * @return the address of the source.
     * */
    const std::string& get_source() const;

    /*!
     * @brief Get a header value from this request.
     * @param header the key of the header to obtain.
     * @return either the header value, or an empty string if the header does not exist.
     * */
    std::string get_header(const std::string& header) const;

    /*!
     * @brief Get the body of the request.
     * @return the body of the request.
     * */
    const std::string& get_body() const;

    /*!
     * @brief Get the URL of the request.
     * @return The URL of the request.
     * */
    const std::string& get_url() const;

    /*!
     * @brief Request is executed over ssl.
     * @return True if request is executed over ssl, false otherwise.
     * */
    bool is_secure() const;

public:
    //  -----  public members  -----
    Parameters params{};
    Parameters query{};

private:
    // Appropriate map type for request may differ from response
    using HeaderList = std::unordered_map<std::string, std::string>;

    Method m_method{Method::UNKNOWN};
    std::string m_destination{};
    std::string m_HTTP_version{};
    std::string m_source{};
    HeaderList m_headers{};
    std::string m_body{};
    bool m_is_secure{false};
};

}
}
}

