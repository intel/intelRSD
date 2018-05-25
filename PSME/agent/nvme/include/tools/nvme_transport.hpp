/*!
 * @brief Definition of NvmeTransport class.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Files}
 * @file nvme_transport.hpp
 */

#pragma once

#include "agent-framework/module/model/endpoint.hpp"

namespace agent {
namespace nvme {

class NvmeTransport {
public:
    /*!
     * @detail Connect to given target.
     *
     * @param target target to connect to.
     */
    static void connect(const agent_framework::model::Endpoint& target);

    /*!
     * @detail Disconnect from given target.
     *
     * @param target target to disconnect from
     */
    static void disconnect(const agent_framework::model::Endpoint& target);
};

}
}

