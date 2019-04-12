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
#include <string>
#include <vector>

#include "response.hpp"

namespace psme {
namespace rest {
namespace server {

/*! Forward declaration */
class Request;

/*!
 * @brief Builds Location header string.
 *
 * @param request Http request.
 * @param scheme_secure Scheme selector: http or https.
 * @param resource_path Path to resource.
 * @param port Optional port value, if not provided will be the one of HOST header (if any)
 *
 * @return location header value
 **/
std::string build_location_header(const Request& request,
                        const bool scheme_secure,
                        const std::string& resource_path,
                        const std::uint16_t port = 0);

/*!
 * @brief Builds Location header string.
 *
 * @param host_header Http host header containing hostname or hostname and port number.
 * @param scheme_secure Scheme selector: http or https.
 * @param resource_path Path to resource.
 * @param port Optional port value, if not provided will be the one of HOST header (if any)
 *
 * @return location header value
 **/
std::string build_location_header(const std::string& host_header,
                        bool scheme_secure,
                        const std::string& resource_path,
                        const std::uint16_t port = 0);

}
}
}

