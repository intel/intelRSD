/*!
 * @brief Processor secure erase endpoint.
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 *
 * @file processor_secure_erase.hpp
 */

#pragma once



#include "psme/rest/endpoints/endpoint_base.hpp"



namespace psme {
namespace rest {
namespace endpoint {

/*!
 * A class representing the REST API Processor Secure Erase endpoint
 */
class ProcessorSecureErase : public EndpointBase {
public:

    /*!
     * @brief The constructor for ProcessorSecureErase class
     */
    explicit ProcessorSecureErase(const std::string& path);


    /*!
     * @brief ProcessorSecureErase class destructor
     */
    virtual ~ProcessorSecureErase();


    void post(const server::Request& request, server::Response& response) override;
};

}
}
}
