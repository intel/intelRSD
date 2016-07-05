
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

#ifndef APP_REST_ENDPOINT_SYSTEM_RESET_HPP
#define APP_REST_ENDPOINT_SYSTEM_RESET_HPP

#include "endpoint.hpp"


namespace psme {
namespace rest {
namespace endpoint {




/*!
 * A class representing the REST API ComputerSystem Reest endpoint
 */
class SystemReset : public Endpoint {
public:
    

    /*!
     * @brief The constructor for SystemReset class
     */
    explicit SystemReset(const std::string& path);

    /*!
     * @brief SystemReset class destructor
     */
    virtual ~SystemReset();

    void post(const server::Request& request, server::Response& response) override;
};

}
}
}

#endif
