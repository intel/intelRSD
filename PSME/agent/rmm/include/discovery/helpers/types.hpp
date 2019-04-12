/*!
 * @brief Enum with types, and matching model types, for rmm discovery
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file base.hpp
 */

#pragma once

#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/chassis_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "telemetry/resource_key.hpp"

#include <memory>


namespace agent {
namespace rmm {
namespace discovery {
namespace helpers {

namespace enums = agent_framework::model::enums;
namespace module = agent_framework::module;
namespace model = agent_framework::model;

/*! Enum representing 'subtype' of the resource to be discovered e.g. there are different entities that have to be
 *  handled differently but use the same model class, therefore this enum is used in all templates instead of a model
 *  class. Note that not all RmmTypes represent something that has to be discovered/handled etc. Some values used
 *  for updates only are also present, e.g. PowerZoneConsumedPower, as some operations are divided into parts */
enum class RmmType : uint8_t {Psu, Fan, PowerZone, ThermalZone, Zone, Drawer, ZoneManager, Rack, RackManager,
    RackInterface, ZoneInterface, RackVlan};

/*! Needed for static_assert in templates for better error logging */
template <RmmType TYPE>
constexpr bool fail_assert() { return false; }

/*! This is required to get base model class for a given rmm subtype. Using a default template will generate
 *  compile-time errors as it is required to make specialization for each case.
 */
template<RmmType TYPE> struct ModelTypeHelper final {};
template<> struct ModelTypeHelper<RmmType::RackManager> final { using type = model::Manager; };
template<> struct ModelTypeHelper<RmmType::ZoneManager> final { using type = model::Manager; };
template<> struct ModelTypeHelper<RmmType::Rack> final { using type = model::Chassis; };
template<> struct ModelTypeHelper<RmmType::Zone> final { using type = model::Chassis; };
template<> struct ModelTypeHelper<RmmType::Drawer> final { using type = model::Chassis; };
template<> struct ModelTypeHelper<RmmType::RackInterface> final { using type = model::NetworkInterface; };
template<> struct ModelTypeHelper<RmmType::ZoneInterface> final { using type = model::NetworkInterface; };
template<> struct ModelTypeHelper<RmmType::RackVlan> final { using type = model::EthernetSwitchPortVlan; };
template<> struct ModelTypeHelper<RmmType::PowerZone> final { using type = model::PowerZone; };
template<> struct ModelTypeHelper<RmmType::ThermalZone> final { using type = model::ThermalZone; };
template<> struct ModelTypeHelper<RmmType::Psu> final { using type = model::Psu; };
template<> struct ModelTypeHelper<RmmType::Fan> final { using type = model::Fan; };

/*! Used to get underlying model class for each RmmType */
template<RmmType TYPE> using ModelType = typename ModelTypeHelper<TYPE>::type;

/*! Helper method to directly access Component of the Rmm type */
template<RmmType TYPE>
constexpr enums::Component get_model_component() {
    return ModelType<TYPE>::get_component();
}


/*!
 * @brief ResourceSensor - matches a resource with a MetricDefinition of one of its metrics
 */
class ResourceSensor {
public:
    ResourceSensor(telemetry::ResourceInstance _resource, agent_framework::model::MetricDefinition _definition)
        : m_resource{_resource}, m_definition{_definition} {}


    virtual ~ResourceSensor();

    const telemetry::ResourceInstance& get_resource() const {
        return m_resource;
    }

    agent_framework::model::MetricDefinition& get_definition() {
        return m_definition;
    }

private:
    telemetry::ResourceInstance m_resource;
    agent_framework::model::MetricDefinition m_definition;
};
using ResourceSensorPtr = std::shared_ptr<ResourceSensor>;


}
}
}
}
