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



#include "agent-framework/generic/singleton.hpp"
#include "psme/rest/server/methods.hpp"
#include "psme/rest/server/request.hpp"
#include "psme/rest/server/response.hpp"
#include "psme/rest/server/methods_handler.hpp"
#include "psme/rest/server/mux/matchers.hpp"

#include <tuple>
#include <vector>



namespace psme {
namespace rest {
namespace server {

/*!
 * @brief Used to register endpoint handlers.
 *
 * The multiplexer is used by a served HTTP server to determine which handler
 * should be used to resolve an HTTP request based on the target endpoint and
 * the HTTP method of the request.
 *
 * A multiplexer is required by the served HTTP server and should be fully
 * configured before the server is run with all endpoints registered.
 * The multiplexer must not be modified after running the HTTP server,
 * as its internal components will be accessible to all HTTP threads.
 * */
class Multiplexer : public agent_framework::generic::Singleton<Multiplexer> {

    using PathHandlerCandidate = std::tuple<mux::SegmentsVec,
        MethodsHandler::UPtr,
        std::string,
        AccessType>;

    using PathHandlerCandidates = std::vector<PathHandlerCandidate>;
    using PluginHandler = std::vector<RequestHandler>;

public:
    virtual ~Multiplexer();


    /*!
     * @brief Register a plugin that should be called before each request.
     *
     * A plugin is a handler called regardless of the request route,
     * this registers a handler to be called for every request before
     * the actual request handler is called.
     *
     * @param plugin the plugin request handler
     * */
    void use_before(RequestHandler plugin);


    /*!
     * @brief Register a plugin that should be called after each request.
     *
     * A plugin is a handler called regardless of the request route, this
     * registers a handler to be called for every request after the actual
     * request handler is called.
     *
     * @param plugin the plugin request handler
     * */
    void use_after(RequestHandler plugin);


    /*!
     * @brief Registers a handler for a specific HTTP endpoint.
     *
     * @param handler Handler for endpoint.
     * @param access_type Used protocol
     */
    void register_handler(MethodsHandler::UPtr handler,
                          AccessType access_type = AccessType::SSL_REQUIRED);


    /*!
     * @brief Forwards a response and request object to a registered handler.
     *
     * Based on the URI target of the request object, forwards the request and response objects to
     * an appropriate handler for producing a response. Always chooses the first registered match.
     *
     * @param response object used to generate an HTTP response
     * @param request object containing information about the HTTP request
     */
    void forward_to_handler(Response& response, Request& request);


    /*!
     * @brief Check whether a given string is a correct endpoint URL from this REST API
     *
     * @param url the string to be checked
     */
    bool is_correct_endpoint_url(const std::string& url) const;


    /*!
     * @brief Verify resource path according to endpoint path template and return the path ids
     * in the form of Parameters - a map of pairs <id name, id as string>
     *
     * @param path the URL to be parsed
     * @param path_template the template to match, must be one of our endpoint Route constants
     * @return the map containing URL ids
     */
    Parameters get_params(const std::string& path, const std::string& path_template) const;


    /*!
     * @brief Verifies whether given url and http method are allowed for all users to access with
     *
     * @param http_method provided with the request
     * @param url provided with the request
     * @param connection_secure connection security flag. True if connection is secure (https), false if not secure (http)
     * @return true if resource is publicly available for given url, method and security flag, false otherwise
     */
    bool check_public_access(const std::string& http_method, const std::string& url, bool connection_secure) const;


    /*!
     * @brief Verify resource path according to endpoint path template and return the path ids
     * in the form of Parameters - a map of pairs <id name, id as string>. No throw GAMI exception after path did not
     * match path template but return empty Parameters instead.
     *
     * @param path the URL to be parsed
     * @param path_template the template to match, must be one of our endpoint Route constants
     * @return the map containing URL ids. Empty map after path did not match path_template
     */
    Parameters try_get_params(const std::string& path, const std::string& path_template) const;


private:
    const PathHandlerCandidate& select_handler(const std::vector<std::string>& segments, const std::string& uri) const;


    bool is_access_allowed(Response& response, Request& request,
                           const PathHandlerCandidate& candidate) const;


    PathHandlerCandidates m_handler_candidates{};

    PluginHandler m_plugin_pre_handlers{};
    PluginHandler m_plugin_post_handlers{};
};

}
}
}

