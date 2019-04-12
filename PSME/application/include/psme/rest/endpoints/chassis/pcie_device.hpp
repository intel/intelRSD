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
 * @file pcie_device.hpp
 * @brief Device endpoint
 * */

#pragma once



#include "psme/rest/endpoints/endpoint_base.hpp"



namespace psme {
namespace rest {
namespace endpoint {

/*!
 * A class representing the rest api PcieDevice endpoint
 */
class PcieDevice : public EndpointBase {
public:

    /*!
     * @brief Constructor
     * @param path Path to be matched against request URL to select this endpoint.
     */
    explicit PcieDevice(const std::string& path);


    /*!
     * @brief PcieDevice class destructor
     */
    virtual ~PcieDevice();


    /*!
     * @brief the implementation of GET method on this endpoint
     *
     * @param request the client's request
     * @param response the GET response
     */
    void get(const server::Request& request, server::Response& response) override;


    /*!
     * @brief the implementation of PATCH method on this endpoint
     *
     * @param request the client's request
     * @param response the PATCH response
     */
    void patch(const server::Request& request, server::Response& response) override;

};

}
}
}

