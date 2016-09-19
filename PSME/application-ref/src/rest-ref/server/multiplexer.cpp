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
#include "psme/rest-ref/server/status.hpp"
#include "psme/rest-ref/server/mux/matchers.hpp"
#include "psme/rest-ref/server/multiplexer.hpp"
#include "psme/rest-ref/server/utils.hpp"
#include "psme/rest-ref/server/error/error_factory.hpp"
#include "psme/rest-ref/server/error/server_exception.hpp"
//#include "psme/rest-ref/endpoints/root.hpp"

using namespace psme::rest::server;

namespace {
    bool segments_match(const mux::SegmentsVec& segments,
                        const std::vector<std::string>& req_segments) {
        const auto s_size = segments.size();
        if (s_size != req_segments.size()) {
            return false;
        }
        for (auto i = mux::SegmentsVec::size_type(0); i < s_size; ++i) {
            if (!segments[i]->check_match(req_segments[i])) {
                return false;
            }
        }
        return true;
    }

    void collect_request_params(Request& req, const mux::SegmentsVec& segments,
            const std::vector<std::string>& req_segments) {
        auto s_size = segments.size();
        for (size_t seg_index(0); seg_index < s_size; ++seg_index) {
            segments[seg_index]->
                    get_param(req.params, req_segments[seg_index]);
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
            return;
        }
    }

    m_handler_candidates.emplace_back(PathHandlerCandidate(mux::path_to_segments(path),
                                   std::move(handler), path, access_type));
}

const Multiplexer::PathHandlerCandidate& Multiplexer::select_handler(
                            const std::vector<std::string>& segments) const {
    auto it = std::find_if(std::begin(m_handler_candidates),
                           std::end(m_handler_candidates),
        [&segments](const PathHandlerCandidate& candidate) {
            return segments_match(std::get<0>(candidate), segments);
        });
    if (it != std::end(m_handler_candidates)) {
        return *it;
    }
    // If no handler was matched throw a 404
    auto error = error::ErrorFactory::create_not_found_error();
    throw error::ServerException(error);
}

bool Multiplexer::is_access_allowed(Response& /*res*/, Request& req,
                const Multiplexer::PathHandlerCandidate& candidate) const {
    auto const access_type = std::get<3>(candidate);
    if (access_type == AccessType::ALL) {
        return true;
    }
    return req.is_secure();
}

void Multiplexer::forward_to_handler(Response& res, Request& req) {

    for (const auto& hhandler : m_plugin_pre_handlers) {
        hhandler(req, res);
    }

    // Split request path into segments
    auto request_segments = mux::split_path(req.get_url());

    const auto& candidate = select_handler(request_segments);

    if (!is_access_allowed(res, req, candidate)) {
        auto error = error::ErrorFactory::create_unauthorized_error();
        throw error::ServerException(error);
    }

    auto& method_handler = *(std::get<1>(candidate));

    // Collect parameters from REST path segments
    collect_request_params(req, std::get<0>(candidate), request_segments);

    execute_handler(method_handler, req, res);
}

EndpointList Multiplexer::get_endpoint_list() {
	EndpointList list;
//	for (const auto & hhandler : m_handler_candidates) {
//		std::get<1>(hhandler).propagate_endpoint(list);
//	}
	return list;
}

