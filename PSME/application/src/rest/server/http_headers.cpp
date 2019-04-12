/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file http_headers.cpp
 * */

#include "psme/rest/server/http_headers.hpp"



namespace psme {
namespace rest {
namespace server {
namespace http_headers {

namespace ContentType {
/*! @brief Content-Type header constant */
const char CONTENT_TYPE[] = "Content-Type";
/*! @brief Content-Type header value of "application/json" */
const char JSON[] = "application/json";
/*! @brief Content-Type header value of "application/xml" */
const char XML[] = "application/xml";
/*! @brief Content-Type header value of "plain/text" */
const char TXT[] = "text/plain";
}

namespace WWWAuthenticate {
/*! @brief WWW-Authenticate header constant */
const char WWW_AUTHENTICATE[] = "WWW-Authenticate";
/*! @brief WWW-Authenticate header value of "Basic" */
const char BASIC[] = "Basic";
/*! @brief WWW-Authenticate header value of "realm" */
const char REALM[] = "realm";
}

namespace Authorization {
/*! @brief Authorization header constant */
const char AUTHORIZATION[] = "Authorization";
/*! @brief Authorization header value of "Basic" */
const char BASIC[] = "Basic";
}

namespace XAuthToken {
/*! @brief X-Auth-Token header constant */
const char X_AUTH_TOKEN[] = "X-Auth-Token";
}

namespace Origin {
/*! @brief Origin header constant */
const char ORIGIN[] = "Origin";
}

namespace Location {
/*! @brief Location header constant */
const char LOCATION[] = "Location";
}

}
}
}
}
