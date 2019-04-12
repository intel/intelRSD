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
#include "psme/rest/server/request.hpp"
#include "psme/rest/server/response.hpp"

#include <map>
#include <memory>
#include <vector>
#include <functional>

namespace psme {
namespace rest {
namespace server {

using RequestHandler = std::function<void(const Request&, Response&)>;
using MethodList = std::tuple<std::string, std::vector<std::string>>;
using EndpointList = std::map<std::string, MethodList>;

/*!
 * @brief Default http not allowed handler
 *
 * @param[in] request HTTP request object
 * @param[out] response HTTP response object
 * */
void http_method_not_allowed(const Request& request, Response& response);

/*!
 * @brief Enumeration for MethodsHandler transport AccessType
 *
 * ALL allows for access via HTTP and HTTPS
 * while SSL_REQUIRED requires HTTPS.
 */
enum class AccessType {
    ALL,
    SSL_REQUIRED
};

/*!
 * @brief Represents a single endpoint with various HTTP method handlers.
 *
 * A method handler is returned when handle has been called on a multiplexer,
 * and should then be
 * used for daisy chaining calls until all method handlers are specified.
 */
class MethodsHandler {
    std::string m_path{};

public:
    using UPtr = std::unique_ptr<MethodsHandler>;

    /*!
     * @brief Create a MethodsHandler from an endpoint path and summary.
     *
     * The endpoint path should be the raw path used when registering a handler.
     * The info param is optional, but should be a short summary of the resource
     * this endpoint points to and is
     * exposed when obtaining a list of registered endpoints from the multiplexer.
     *
     * @param path the endpoint path of this handler
     */
    explicit MethodsHandler(const std::string& path);

    MethodsHandler(const MethodsHandler&) = default;
    MethodsHandler& operator=(const MethodsHandler&) = default;
    virtual ~MethodsHandler();

    const std::string& get_path() const;

    /*!
     * @brief GET HTTP method handler
     * @param[in] request HTTP request object
     * @param[out] response HTTP response object
     */
    virtual void get(const Request& request, Response& response) = 0;

    /*!
     * @brief DELETE HTTP method handler
     * @param[in] request HTTP request object
     * @param[out] response HTTP response object
     */
    virtual void del(const Request& request, Response& response) = 0;

    /*!
     * @brief POST HTTP method handler
     * @param[in] request HTTP request object
     * @param[out] response HTTP response object
     */
    virtual void post(const Request& request, Response& response) = 0;

    /*!
     * @brief PATCH HTTP method handler
     * @param[in] request HTTP request object
     * @param[out] response HTTP response object
     */
    virtual void patch(const Request& request, Response& response) = 0;

    /*!
     * @brief PUT HTTP method handler
     * @param[in] request HTTP request object
     * @param[out] response HTTP response object
     */
    virtual void put(const Request& request, Response& response) = 0;
};

}
}
}


