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
 * @file content_types.hpp
 *
 * @brief Declaration of content type constants.
 * */

#pragma once
namespace psme {
namespace rest {
namespace server {

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

}
}
}

