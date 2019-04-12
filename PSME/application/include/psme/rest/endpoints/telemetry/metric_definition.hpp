/*!
 * @brief Metric definition endpoint
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file metric_definition.hpp
 */

#pragma once



#include "psme/rest/endpoints/endpoint_base.hpp"



namespace psme {
namespace rest {
namespace endpoint {

/*!
 * A class representing the rest api MetricDefinition endpoint
 */
class MetricDefinition : public EndpointBase {
public:

    /*!
     * @brief The constructor for MetricDefinition class
     */
    explicit MetricDefinition(const std::string& path);


    /*!
     * @brief MetricDefinition class destructor
     */
    virtual ~MetricDefinition();


    void get(const server::Request& request, server::Response& response) override;
};

}
}
}

