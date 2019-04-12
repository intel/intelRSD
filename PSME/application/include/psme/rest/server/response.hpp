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

#include "psme/rest/server/status.hpp"
#include "psme/rest/server/content_types.hpp"

#include <map>
#include <sstream>

namespace psme {
namespace rest {
namespace server {

/*!
 * @brief Represents a HTTP response.
 *
 * This class represents a HTTP response, and provides an appropriate interface
 * for setting properties, and then generating a response string. The class is
 * composed of a status code, headers, and the body of the response.
 *
 * The response to a given HTTP request is built by method handlers in response
 * to a request. This class is used to simplify the method of composing an
 * appropriate response to a request.
 */
class Response {
public:
    /*! @brief HTTP Headers container */
    using HeaderList = std::map<std::string, std::string>;

    /*! @brief Constructor */
    explicit Response();

    /*!
     * @brief Sets a header field.
     *
     * Setting the same header twice will overwrite the previous.
     *
     * @param header the header key
     * @param value the header value
     */
    void set_header(const std::string& header, const std::string& value);

    /*!
     * @brief HTTP Headers getter.
     * @return  HTTP Headers of response object.
     */
    const HeaderList& get_headers() const;

    /*!
     * @brief Set the HTTP status of the response.
     * @param status_code the status code
     */
    void set_status(const std::uint32_t status_code);

    /*!
     * @brief Set the entire body of the response.
     * Sets the body of the response, overwriting
     * any previous data stored in the body.
     * @param body the response body
     */
    void set_body(const std::string& body);

    /*!
     * @brief Pipe data to the body of the response.
     * Appends data onto the body of the response.
     * @param rhs data to be appended
     */
    Response& operator<<(const std::string& rhs);

    /*!
     * @brief Get the status of the response.
     * @return the status of the response
     */
    std::uint32_t get_status();

    /*!
     * @brief Get the byte count of the response body.
     * @return the size of the response body
     */
    std::size_t get_body_size();

    /*!
     * @brief Get HTTP response body.
     *
     * @return the HTTP response body
     */
    const std::string& get_body() const;

private:
    std::uint32_t m_status{};
    HeaderList m_headers{};
    std::string m_body{};
};

}
}
}

