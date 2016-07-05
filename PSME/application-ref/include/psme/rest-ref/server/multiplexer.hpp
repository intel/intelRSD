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
 * */

#ifndef PSME_REST_SERVER_MULTIPLEXER_HPP
#define PSME_REST_SERVER_MULTIPLEXER_HPP

#include "psme/rest-ref/server/methods.hpp"
#include "psme/rest-ref/server/request.hpp"
#include "psme/rest-ref/server/response.hpp"
#include "psme/rest-ref/server/methods_handler.hpp"
#include "psme/rest-ref/server/mux/matchers.hpp"

#include <tuple>
#include <vector>

namespace psme {
namespace rest {
namespace server {

/*
 * Used to register endpoint handlers.
 * The multiplexer is used by a served HTTP
 * server to determine which handler should be used to
 * resolve an HTTP request based on the target endpoint and the HTTP
 * method of the request.
 * A multiplexer is required by the served HTTP server
 * and should be fully configured before
 * the server is run with all endpoints registered.
 * The multiplexer must not be modified after running the HTTP server,
 * as its internal components
 * will be accessible to all HTTP threads.
 */
class Multiplexer {

    using PathHandlerCandidate = std::tuple<mux::SegmentsVec,
                                            MethodsHandler::UPtr,
                                            std::string,
                                            AccessType>;

    using PathHandlerCandidates = std::vector<PathHandlerCandidate>;

    using PluginHandler = std::vector<RequestHandler>;

public:
    //  -----  http request handlers  -----
    /*
     * Register a plugin that should be called before each request.
     *
     * A plugin is a handler called regardless of the request route,
     * this registers a handler
     * to be called for every request before the actual request
     * handler is called.
     *
     * @param plugin the plugin request handler
     **/
    void use_before(RequestHandler plugin);

    /*
     * Register a plugin that should be called after each request.
     *
     * A plugin is a handler called regardless of the
     * request route, this registers a handler
     * to be called for every request after the actual
     * request handler is called.
     *
     * @param plugin the plugin request handler
     * */
    void use_after(RequestHandler plugin);

    /*!
     * @brief Registers a handler for a specific HTTP endpoint.
     *
     *
     * @param handler Handler for endpoint.
     *
     */
    void register_handler(MethodsHandler::UPtr handler,
                          AccessType access_type = AccessType::SSL_REQUIRED);

    //  ----- request forwarding  -----

    /*
     * Forwards a response and request object to a registered handler.
     *
     * Based on the URI target of the request object, forwards the request and response objects to
     * an appropriate handler for producing a response. Always chooses the first registered match.
     *
     * @param res object used to generate an HTTP response
     * @param req object containing information about the HTTP request
     */
    void forward_to_handler(Response& res, Request& req);

    //  -----  accessors  -----

    /*
     * Constructs a full list of all endpoints registered.
     *
     * Registered endpoints contain a list of HTTP methods supported and an optional summary of the
     * resource each endpoint represents.
     *
     * @return a list of all endpoints registered
     */
    EndpointList get_endpoint_list();

private:
    const PathHandlerCandidate& select_handler(const std::vector<std::string>& segments) const;

    bool is_access_allowed(Response& res,
                           Request& req,
                           const PathHandlerCandidate& candidate) const;

    PathHandlerCandidates m_handler_candidates{};

    PluginHandler m_plugin_pre_handlers{};
    PluginHandler m_plugin_post_handlers{};
};

}
}
}

#endif // PSME_REST_SERVER_MULTIPLEXER_HPP
