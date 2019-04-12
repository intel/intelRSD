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
 *
 *
 * @file utils.cpp
 *
 * */

#include "psme/rest/server/utils.hpp"
#include "psme/rest/server/request.hpp"



std::string psme::rest::server::build_location_header(const psme::rest::server::Request& request,
                                                      bool scheme_secure,
                                                      const std::string& resource_path,
                                                      const std::uint16_t port) {
    auto host_header = request.get_header("Host");

    return build_location_header(host_header, scheme_secure, resource_path, port);
}

std::string psme::rest::server::build_location_header(const std::string& host_header,
                                                      bool scheme_secure,
                                                      const std::string& resource_path,
                                                      const std::uint16_t port) {
    auto tmp_host_header = host_header;

    constexpr const char HTTPS[] = "https://";
    constexpr const char HTTP[] = "http://";

    const auto scheme = scheme_secure ? HTTPS : HTTP;
    if (port) {
        auto port_pos = host_header.find_first_of(':');
        auto port_string = std::to_string(static_cast<uint>(port));
        if (std::string::npos != port_pos) {
            tmp_host_header = host_header.substr(0, port_pos + 1) + port_string;
        }
        else {
            tmp_host_header = host_header + ':' + port_string;
        }
    }
    return scheme + tmp_host_header + resource_path;
}

