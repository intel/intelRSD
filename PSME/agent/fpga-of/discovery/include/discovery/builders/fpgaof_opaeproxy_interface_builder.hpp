/*!
 * @brief OPAE Proxy interface discoverer builder.
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
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
 * @file fpgaof_opaeproxy_interface_builder.hpp
 */

#pragma once


namespace agent {
namespace fpgaof {
namespace discovery {

class FpgaofOpaeproxyInterfaceBuilder {
public:

    /*!
     * @brief Build default system object.
     * @param[in] parent_uuid UUID of the parent System.
     * @param[in] ipv4 IP address of the interface.
     * @param[in] opaeproxy_transport_protocols List of OPAE Proxy transport protocols of the interface
     * @return Built NetworkInterface object.
     */
    static agent_framework::model::NetworkInterface build(const Uuid& parent_uuid,
                                                          const std::string& ipv4,
                                                          const std::vector<std::string>& opaeproxy_transport_protocols);
};

}
}
}

