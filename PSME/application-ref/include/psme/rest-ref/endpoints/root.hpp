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

#ifndef APP_REST_ENDPOINT_ROOT_HPP
#define APP_REST_ENDPOINT_ROOT_HPP

#include "endpoint.hpp"


namespace psme {
namespace rest {
namespace endpoint {



/*!
 * A class representing the rest api Root endpoint
 */
class Root : public Endpoint {
public:
    

    /*!
     * @brief Constructor
     * @param path Path to be matched against request URL to select this endpoint.
     */
    explicit Root(const std::string& path);

    /*!
     * @brief Root class destructor
     */
    virtual ~Root();

    void get(const server::Request& request, server::Response& response) override;
};

}
}
}

#endif
