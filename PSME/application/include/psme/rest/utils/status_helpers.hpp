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
#include <algorithm>



namespace psme {
namespace rest {
namespace endpoint {

/*!
 * @brief Derives rollup health from subtree of resources
 *
 */
template<typename T>
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
     * @return Health Rollup optional
     */
    agent_framework::module::utils::OptionalField<agent_framework::model::enums::Health> get(
        const std::string& uuid,
        const Component filter = Component::None) {

        my_uuid = uuid;
        component_filter = filter;
        auto handler = psme::rest::model::handler::HandlerManager::get_instance()->get_handler(T::get_component());
        handler->accept(*this, uuid);

        if (my_health.has_value() && worst_children_health.has_value()) {
            return std::max(worst_children_health, my_health);
        }
        else if (my_health.has_value()){
            return my_health;
        }
        return worst_children_health;
    }


    /*!
     * @brief Derives rollup health from own health and health of child nodes
     *
     * @param[in] resource Resource object (one of descendants of nodes passed to get() method)
     * @param[in] component Component type of Resource
     * @return returns false if execution of visitor should be stopped
     */
    bool visit(const agent_framework::model::Resource& resource, const Component component) override {
        if (Component::None == component_filter || component == component_filter) {
            if (my_uuid == resource.get_uuid()) {
                my_health = resource.get_status().get_health();
                if (!my_health.has_value()) {
                    return false; // we are done, skip children
                }
            }
            else { // descendant

                const auto& health = resource.get_status().get_health();
                if (health.has_value()) {
                    worst_children_health = std::max(worst_children_health, health); // update health
                }
                else {
                    // ignore null health of
                }
            }
        }
        return true;
    }


protected:
    std::string my_uuid{};
    agent_framework::module::utils::OptionalField<agent_framework::model::enums::Health> my_health{};
    agent_framework::module::utils::OptionalField<agent_framework::model::enums::Health> worst_children_health{};
    Component component_filter{Component::None};
};


/*!
 * @brief Get resource status (state and health) and add it to GET reply JSON
 *
 * @param resource a Resource
 * @param v the json value sent as a GET response
 */
template <typename T>
void status_to_json(const T& resource, json::Json& v) {
    auto& status = resource.get_status();
    v[constants::Common::STATUS][constants::Common::STATE] = status.get_state().to_string();
    v[constants::Common::STATUS][constants::Common::HEALTH] = status.get_health();
    v[constants::Common::STATUS][constants::Common::HEALTH_ROLLUP] = HealthRollup<T>().get(resource.get_uuid());
}

}
}
}
