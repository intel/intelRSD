/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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



#include "psme/rest/endpoints/endpoint_base.hpp"



namespace psme {
namespace rest {
namespace endpoint {

/*!
 * @brief A class representing the REST API ManagerVlanNetworkInterfaceCollection endpoint
 */
class ManagerVlanNetworkInterfaceCollection : public EndpointBase {
public:

    /*!
     * @brief The constructor for ManagerVlanNetworkInterfaceCollection class.
     * @param[in] path URL path to the endpoint.
     * */
    explicit ManagerVlanNetworkInterfaceCollection(const std::string& path);


    /*!
     * @brief ManagerVlanNetworkInterfaceCollection class destructor.
     * */
    virtual ~ManagerVlanNetworkInterfaceCollection();


    void get(const server::Request& request, server::Response& response) override;
};

}
}
}

