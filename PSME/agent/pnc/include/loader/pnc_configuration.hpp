/*!
 * @brief Configuration for PNC agent
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file pnc_configuration.hpp
 */

#pragma once
#include "agent-framework/generic/singleton.hpp"
#include "agent-framework/module/enum/enum_builder.hpp"



namespace agent {
namespace pnc {
namespace loader {

ENUM(FabricDiscoveryMode, unsigned,
     DATABASE,
     AUTOMATIC
);

class PncConfiguration final : public agent_framework::generic::Singleton<PncConfiguration> {
public:
    void set_fabric_discovery_mode(const std::string& fabric_discovery_mode) {
        m_fabric_discovery_mode = FabricDiscoveryMode::from_string(fabric_discovery_mode);
    }

    const FabricDiscoveryMode get_fabric_discovery_mode() const {
        return m_fabric_discovery_mode;
    }

    const std::string& get_secure_erase_gbs() const {
        return m_secure_erase_gbs;
    }

    void set_secure_erase_gbs(const std::string& path_to_gbs) {
        m_secure_erase_gbs = path_to_gbs;
    }

private:
    FabricDiscoveryMode m_fabric_discovery_mode{FabricDiscoveryMode::DATABASE};
    std::string m_secure_erase_gbs{};
};
}
}
}