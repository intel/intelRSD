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

#include "psme/rest/endpoints/endpoint_base.hpp"
#include "psme/rest/server/error/error_factory.hpp"
#include "psme/rest/server/mux/matchers.hpp"

#include <locale>
#include <chrono>

using namespace psme::rest::server;
using namespace psme::rest::endpoint;

namespace {
constexpr std::size_t TIME_BUFFER_SIZE = 26;

std::string get_current_time() {
    char time_buffer[TIME_BUFFER_SIZE];
    auto now = std::chrono::system_clock::now();
    auto time_now = std::chrono::system_clock::to_time_t(now);

    tzset();

    struct tm local_tm;
    localtime_r(&time_now, &local_tm);
    std::strftime(time_buffer, TIME_BUFFER_SIZE, "%FT%H:%M", &local_tm);

    return time_buffer;
}

}

void psme::rest::server::http_method_not_allowed(const Request&, Response& response) {
    auto message = "Requested operation is not allowed on this resource.";
    auto error = error::ErrorFactory::create_method_not_allowed_error(message);

    response.set_status(error.get_http_status_code());
    response.set_body(error.as_string());
}

EndpointBase::EndpointBase(const std::string& path)
    : MethodsHandler(path), m_modified_time{::get_current_time()} {}

EndpointBase::~EndpointBase() {}

void EndpointBase::update_modified_time() {
    m_modified_time = ::get_current_time();
}

void EndpointBase::get(const Request& request, Response& response) {
    http_method_not_allowed(request, response);
}

void EndpointBase::del(const Request& request, Response& response) {
    http_method_not_allowed(request, response);
}

void EndpointBase::post(const Request& request, Response& response) {
    http_method_not_allowed(request, response);
}

void EndpointBase::patch(const Request& request, Response& response) {
    http_method_not_allowed(request, response);
}

void EndpointBase::put(const Request& request, Response& response) {
    http_method_not_allowed(request, response);
}
