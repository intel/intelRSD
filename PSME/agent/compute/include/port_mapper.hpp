/*!
 * @copyright
 * Copyright (c) 2016 Intel Corporation
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
 * @file port_mapper.hpp
 *
 * @brief Map manager to a switch port
 * */

#ifndef AGENT_PORT_MAPPER_HPP
#define AGENT_PORT_MAPPER_HPP

#include "agent-framework/generic/singleton.hpp"

#include <map>
#include <string>

namespace agent {
namespace compute {

/*!
 * @brief PortMapper singleton
 *
 * Store information about switch port identifier per manager as read from
 * configuration file during application startup.
 */
class PortMapper : public agent_framework::generic::Singleton<PortMapper> {
public:
    virtual ~PortMapper();

    /*!
     * @brief Set switch port identifier for given manager UUID.
     * @param manager_uuid manager uuid
     * @param port_identifier port identifier.
     */
    void set_switch_port_identifier(const std::string& manager_uuid,
                                    const std::string& port_identifier);

    /*!
     * @brief Get switch port identifier for given manager uuid.
     * @param manager_uuid Manager uuid.
     */
    const std::string& get_switch_port_identifier(const std::string&
                                                  manager_uuid) const;

private:
    std::map<std::string, std::string> m_manager_to_port_map{};
};


}
}

#endif /* AGENT_PORT_MAPPER_HPP */
