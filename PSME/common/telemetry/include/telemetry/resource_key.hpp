/*!
 * @brief ResourceInstance
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file resource_instance.hpp
 */

#pragma once

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/model/memory.hpp"

namespace telemetry {

/*!
 * @brief Identifies Resource instance within platform
 */
class ResourceInstance {
public:
    using Component = agent_framework::model::enums::Component;
    static constexpr int ANY_INSTANCE = -1;

    /*!
     * @brief Constructor
     * @param component Resource type
     * @param instance_number Resource instance number within platform
     */
    explicit constexpr ResourceInstance(const Component& component = Component::None,
                                        const int instance_number = ANY_INSTANCE)
        : m_component(component), m_component_instance(instance_number) {}

    /*!
     * @brief Equal operator
     */
    constexpr bool operator==(const ResourceInstance& other) const {
        return m_component == other.m_component
               && m_component_instance == other.m_component_instance;
    }

    /*!
     * @brief Unequal operator
     */
    constexpr bool operator!=(const ResourceInstance& other) const {
        return !operator==(other);
    }

    /*!
     * @brief Less operator
     */
    constexpr bool operator<(const ResourceInstance& other) const {
        return m_component == other.m_component
               ? m_component_instance < other.m_component_instance
               : m_component < other.m_component;
    }

    /*!
     * @return resource component type
     */
    constexpr const Component& get_component() const {
        return m_component;
    }

    /*!
     * @return resource instance number within platform
     */
    constexpr int get_instance_number() const {
        return m_component_instance;
    }

private:
    Component m_component;
    int m_component_instance;
};


std::ostream& operator<<(std::ostream& os, const ResourceInstance& resource_key);


/*
 * @return ResourceInstance uniquely identifying given Resource type within platform.
 */
template<typename R>
ResourceInstance get_resource_key(const R&) {
    return ResourceInstance{R::get_component()};
}

template<>
ResourceInstance get_resource_key<agent_framework::model::Processor>(const agent_framework::model::Processor& processor);

template<>
ResourceInstance get_resource_key<agent_framework::model::Memory>(const agent_framework::model::Memory& processor);

}
