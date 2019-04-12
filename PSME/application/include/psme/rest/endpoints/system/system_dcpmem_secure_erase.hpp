/*!
 * @brief SystemDcpmemSecureErase endpoint declaration
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file system_dcpmem_secure_erase.hpp
 */

#pragma once
#include "psme/rest/endpoints/endpoint_base.hpp"


namespace psme {
namespace rest {
namespace endpoint {


/*!
 * A class representing the REST API Persistent Memory SecureErase endpoint
 */
class SystemDcpmemSecureErase : public EndpointBase {
public:


    /*!
     * @brief The constructor for SystemDcpmemSecureErase
     */
    explicit SystemDcpmemSecureErase(const std::string& path);

    /*!
     * @brief SystemDcpmemSecureErase class destructor
     */
    virtual ~SystemDcpmemSecureErase();

    void post(const server::Request& request, server::Response& response) override;
};

}
}
}