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
 * @file session_collection.hpp
 *
 * @brief SessionCollection endpoint declaration.
 *
 * */

#pragma once



#include "endpoint_base.hpp"



namespace psme {
namespace rest {
namespace endpoint {

/*!
 * A class representing the rest api SessionCollection endpoint
 */
class SessionCollection : public EndpointBase {
public:

    /*!
     * @brief The constructor for SessionCollection class
     */
    explicit SessionCollection(const std::string& path);


    /*!
     * @brief SessionCollection class destructor
     */
    virtual ~SessionCollection();


    void get(const server::Request& request, server::Response& response) override;


    void post(const server::Request& request, server::Response& response) override;

};

}
}
}

