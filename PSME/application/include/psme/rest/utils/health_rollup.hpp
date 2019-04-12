/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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

#pragma once

#include "psme/rest/model/handlers/handler_manager.hpp"
#include "psme/rest/model/handlers/generic_handler_deps.hpp"
#include "psme/rest/model/handlers/handler_interface.hpp"

namespace psme {
namespace rest {
namespace endpoint {

/*!
 * @brief Derives rollup health from subtree of resources
 *
 */
template <typename T>
class HealthRollup : public psme::rest::model::handler::HandlerInterface::ResourceVisitor {
    using Component = agent_framework::model::enums::Component;
public:
    HealthRollup() {
        static_assert(agent_framework::model::enums::Health::OK < agent_framework::model::enums::Health::Warning,
                      "Unexpected Health values ordering");
        static_assert(agent_framework::model::enums::Health::Warning < agent_framework::model::enums::Health::Critical,
                      "Unexpected Health values ordering");
    }

    /*!
     * @brief Computes Health rollup starting from node identified by uuid
     *
     * Executes visitor pattern implemented by GenericHandler.
     *
     * @param[in] uuid Node for which health rollup is to be computed
     * @param[in] filter Optional parameter that limits rollup computation to given component type
     */
    agent_framework::model::enums::Health get(const std::string& uuid,
                                              const Component filter = Component::None) {
        component_filter = filter;
        auto handler = psme::rest::model::handler::HandlerManager::get_instance()->get_handler(T::get_component());
        handler->accept(*this, uuid);

        return health;
    }

    /*!
     * @brief Updates rollup health with health from subnode
     *
     * @param[in] resource Resource object (one of descendants of nodes passed to get() method)
     * @param[in] component Component type of Resource
     */
    void visit(const agent_framework::model::Resource& resource, const Component component) override {
        if (Component::None == component_filter || component == component_filter) {
            health = std::max(health, resource.get_status().get_health()); // update health
        }
    }
protected:
    agent_framework::model::enums::Health health{agent_framework::model::enums::Health::OK};
    Component component_filter { Component::None };
};

}
}
}
