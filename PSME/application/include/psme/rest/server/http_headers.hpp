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
 * @file http_headers.hpp
 *
 * @brief Declaration of http headers constants.
 * */

#pragma once
namespace psme {
namespace rest {
namespace server {
namespace http_headers {

namespace ContentType {
/*! @brief Content-Type header constant */
extern const char CONTENT_TYPE[];
/*! @brief Content-Type header value of "application/json" */
extern const char JSON[];
/*! @brief Content-Type header value of "application/xml" */
extern const char XML[];
/*! @brief Content-Type header value of "plain/text" */
extern const char TXT[];
}

namespace WWWAuthenticate {
/*! @brief WWW-Authenticate header constant */
extern const char WWW_AUTHENTICATE[];
/*! @brief WWW-Authenticate header value of "Basic" */
extern const char BASIC[];
/*! @brief WWW-Authenticate header value of "realm" */
extern const char REALM[];
}

namespace Authorization {
/*! @brief Authorization header constant */
extern const char AUTHORIZATION[];
/*! @brief Authorization header value of "Basic" */
extern const char BASIC[];
}

namespace XAuthToken {
/*! @brief X-Auth-Token header constant */
extern const char X_AUTH_TOKEN[];
}

namespace Origin {
/*! @brief Origin header constant */
extern const char ORIGIN[];
}

namespace Location {
/*! @brief Location header constant */
extern const char LOCATION[];
}

}
}
}
}
