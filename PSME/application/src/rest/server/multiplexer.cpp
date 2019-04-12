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
#include "psme/rest/constants/routes.hpp"
#include "psme/rest/server/status.hpp"
#include "psme/rest/server/mux/matchers.hpp"
#include "psme/rest/server/multiplexer.hpp"
#include "psme/rest/server/utils.hpp"
#include "psme/rest/server/error/error_factory.hpp"



using namespace psme::rest::server;

namespace {

void collect_request_params(Request& req, const mux::SegmentsVec& segments,
                            const std::vector<std::string>& req_segments) {
    auto segments_size = segments.size();
    for (size_t seg_index(0); seg_index < segments_size; ++seg_index) {
        segments[seg_index]->get_param(req.params, req_segments[seg_index]);
    }
}


void execute_handler(MethodsHandler& h, Request& req, Response& res) {
    switch (req.get_method()) {
        case Method::GET:
            h.get(req, res);
            break;
        case Method::POST:
            h.post(req, res);
            break;
        case Method::PATCH:
            h.patch(req, res);
            break;
        case Method::DELETE:
            h.del(req, res);
            break;
        case Method::PUT:
            h.put(req, res);
            break;
        case Method::HEAD:
        case Method::OPTIONS:
        case Method::UNKNOWN:
        default:
            http_method_not_allowed(req, res);
    }
}

}


void Multiplexer::use_before(RequestHandler plugin) {
    m_plugin_pre_handlers.emplace_back(plugin);
}


void Multiplexer::use_after(RequestHandler plugin) {
    m_plugin_post_handlers.emplace_back(plugin);
}


void Multiplexer::register_handler(MethodsHandler::UPtr handler, AccessType access_type) {
    const auto& path = handler->get_path();
    // Find existing candidate
    for (auto& candidate : m_handler_candidates) {
        if (std::get<2>(candidate) == path) {
            log_error("rest", "Attempted to register a duplicate handler for " + handler->get_path() + ".");
            return;
        }
    }

    m_handler_candidates.emplace_back(PathHandlerCandidate(mux::path_to_segments(path),
                                                           std::move(handler), path, access_type));
}


const Multiplexer::PathHandlerCandidate& Multiplexer::select_handler(const std::vector<std::string>& segments,
                                                                     const std::string& uri) const {
    auto it = std::find_if(std::begin(m_handler_candidates),
                           std::end(m_handler_candidates),
                           [&segments](const PathHandlerCandidate& candidate) {
                               return mux::segments_match(std::get<0>(candidate), segments);
                           });

    if (it != std::end(m_handler_candidates)) {
        return *it;
    }

    // If no handler was matched throw a 404
    log_warning("rest", "No handler was matched!");
    auto message = "Invalid endpoint in /redfish/v1 namespace.";
    throw error::ServerException(error::ErrorFactory::create_resource_missing_error(uri, message));
}


bool Multiplexer::is_access_allowed(Response&, Request& request,
                                    const Multiplexer::PathHandlerCandidate& candidate) const {
    auto const access_type = std::get<3>(candidate);
    if (access_type == AccessType::ALL) {
        return true;
    }
    return request.is_secure();
}


void Multiplexer::forward_to_handler(Response& response, Request& request) {

    for (const auto& handler : m_plugin_pre_handlers) {
        handler(request, response);
    }

    // Split request path into segments
    const auto& url = request.get_url();
    auto request_segments = mux::split_path(url);

    const auto& candidate = select_handler(request_segments, url);

    if (!is_access_allowed(response, request, candidate)) {
        auto error = error::ErrorFactory::create_unauthorized_error(url, "Secure connection is required.");
        throw error::ServerException(error);
    }

    auto& method_handler = *(std::get<1>(candidate));

    // Collect parameters from REST path segments
    collect_request_params(request, std::get<0>(candidate), request_segments);

    execute_handler(method_handler, request, response);
}


bool Multiplexer::is_correct_endpoint_url(const std::string& url) const {
    auto request_segments = mux::split_path(url);
    auto it = std::find_if(std::begin(m_handler_candidates),
                           std::end(m_handler_candidates),
                           [&request_segments](const PathHandlerCandidate& candidate) {
                               return mux::segments_match(std::get<0>(candidate), request_segments);
                           });

    return it != std::end(m_handler_candidates);
}


Parameters Multiplexer::get_params(const std::string& path, const std::string& path_template) const {
    auto it = std::find_if(m_handler_candidates.begin(), m_handler_candidates.end(),
                           [&path_template](const PathHandlerCandidate& candidate) {
                               return std::get<2>(candidate) == path_template;
                           }
    );
    if (it == m_handler_candidates.end()) {
        // path_template must be an existing endpoint path
        throw std::logic_error("Unrecognized path template supplied to multiplexer.");
    }

    const auto& handler = *it;
    const auto path_segments = mux::split_path(path);

    if (!mux::segments_match(std::get<0>(handler), path_segments)) {
        std::string message = "'" + path + "' is not a correct URL in /redfish/v1 namespace.";
        throw agent_framework::exceptions::InvalidValue(message);
    }

    Parameters params{};
    auto& handler_segments = std::get<0>(handler);
    auto segments_size = handler_segments.size();
    for (size_t seg_index(0); seg_index < segments_size; ++seg_index) {
        handler_segments[seg_index]->get_param(params, path_segments[seg_index]);
    }
    return params;
}


Parameters Multiplexer::try_get_params(const std::string& path, const std::string& path_template) const {
    auto it = std::find_if(m_handler_candidates.begin(), m_handler_candidates.end(),
                           [&path_template](const PathHandlerCandidate& candidate) {
                               return std::get<2>(candidate) == path_template;
                           }
    );
    if (it == m_handler_candidates.end()) {
        // path_template must be an existing endpoint path
        throw std::logic_error("Unrecognized path template supplied to multiplexer.");
    }

    const auto& handler = *it;
    const auto path_segments = mux::split_path(path);
    Parameters params{};

    if (mux::segments_match(std::get<0>(handler), path_segments)) {
        auto& handler_segments = std::get<0>(handler);
        auto segments_size = handler_segments.size();
        for (size_t seg_index(0); seg_index < segments_size; ++seg_index) {
            handler_segments[seg_index]->get_param(params, path_segments[seg_index]);
        }
    }

    return params;
}


Multiplexer::~Multiplexer() {}


bool Multiplexer::check_public_access(const std::string& http_method, const std::string& url,
                                      bool connection_secure) const {

    // Split request path into segments
    auto request_segments = mux::split_path(url);

    const auto& candidate = select_handler(request_segments, url);

    const auto endpoint_path = std::get<2>(candidate);
    if (endpoint_path == constants::Routes::SESSION_COLLECTION_PATH && http_method == "POST" && connection_secure) {
        return true;
    }
    auto const access_type = std::get<3>(candidate);
    if (access_type == AccessType::ALL) {
        return true;
    }
    return false;
    //TODO implement public resources access based on configuration.json. The solution above is temporary.
}
