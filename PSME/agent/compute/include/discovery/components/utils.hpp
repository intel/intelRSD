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
 *
 *
 * @file discovery/components/utils.hpp
 *
 * @brief Components free function declaration
 * */

#ifndef AGENT_COMPUTE_DISCOVERY_COMPONENTS_UTILS_HPP
#define	AGENT_COMPUTE_DISCOVERY_COMPONENTS_UTILS_HPP

#include "ipmi/response.hpp"
#include "ipmi/management_controller.hpp"

namespace agent {
namespace compute {
namespace discovery {

bool check_ipmi_response(const ipmi::Response& response,
                         const ipmi::Response::COMPLETION_CODE code);

template <typename T>
void set_connection_port(ipmi::ManagementController& mc,
                         const T& manager,
                         const std::string& entry) {
    const auto con_data = manager.get_connection_data(entry);
    mc.set_port(con_data.get_port());
}

}}}


#endif	/* AGENT_COMPUTE_DISCOVERY_COMPONENTS_UTILS_HPP */

