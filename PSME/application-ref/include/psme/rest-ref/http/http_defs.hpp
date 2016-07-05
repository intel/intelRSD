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
 *
 *
 * @file http_defs.hpp
 *
 * @brief Declaration of mime types, HTTP methods
 * */

#ifndef PSME_REST_HTTP_DEFS_HPP
#define PSME_REST_HTTP_DEFS_HPP

namespace psme {
namespace rest {
namespace http {

struct MimeType {
    /*! @brief JSON content type */
    static constexpr const char JSON[] = "application/json";
    /*! @brief XML content type */
    static constexpr const char XML[] = "application/xml";
    /*! @brief text content type */
    static constexpr const char TXT[] = "text/plain";
};

} /* namespace http */
} /* namespace rest */
} /* namespace psme */

#endif /* PSME_REST_HTTP_DEFS_HPP */
