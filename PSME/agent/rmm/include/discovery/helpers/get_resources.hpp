/*!
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
 * @file rmm/discovery/helpers/get_resources.hpp
 */

#pragma once

#include "discovery/helpers/common.hpp"

namespace agent {
namespace rmm {
namespace discovery {
namespace helpers {

using KeysVec = std::vector<std::string>;

/*!
 * This method is responsible for returning list of keys that are already stored in the model that match specific
 * requirements. Depending on the TYPE, different conditions are used. It requires two argumnents: discovery
 * params that provide additional data and parent uuid. By defult it returns all entries under a specific parent.
 * */
template <RmmType TYPE>
KeysVec get_resources(const DiscoveryParams<TYPE>&, const std::string& parent) {
    return module::get_manager<ModelType<TYPE>>().get_keys(parent);
}

template <>
KeysVec get_resources<RmmType::Rack>(const DiscoveryParams<RmmType::Rack>&, const std::string&) {
    using MODEL_CLASS = ModelType<RmmType::Rack>;
    auto filter = [] (const MODEL_CLASS& res) -> bool {
        return (enums::ChassisType::Rack == res.get_type()); };
    return module::get_manager<MODEL_CLASS>().get_keys(filter);
}

template <>
KeysVec get_resources<RmmType::Psu>(const DiscoveryParams<RmmType::Psu>& dp, const std::string& parent) {
    using MODEL_CLASS = ModelType<RmmType::Psu>;
    auto filter = [&dp] (const MODEL_CLASS& res) -> bool {
        return (res.get_slot_id() == dp.slot_id); };
    return module::get_manager<MODEL_CLASS>().get_keys(parent, filter);
}

template <>
KeysVec get_resources<RmmType::Fan>(const DiscoveryParams<RmmType::Fan>& dp, const std::string& parent) {
    using MODEL_CLASS = ModelType<RmmType::Fan>;
    auto filter = [&dp] (const MODEL_CLASS& res) -> bool {
        return (res.get_slot_id() == dp.slot_id); };
    return module::get_manager<MODEL_CLASS>().get_keys(parent, filter);
}

template <>
KeysVec get_resources<RmmType::Drawer>(const DiscoveryParams<RmmType::Drawer>& dp, const std::string&) {
    using MODEL_CLASS = ModelType<RmmType::Drawer>;
    auto filter = [&dp] (const MODEL_CLASS& res) -> bool {
        return (res.get_parent_chassis() == dp.zone_uuid && res.get_slot_id() == dp.slot_id); };
    return module::get_manager<MODEL_CLASS>().get_keys(filter);
}

template <>
KeysVec get_resources<RmmType::Zone>(const DiscoveryParams<RmmType::Zone>& dp, const std::string&) {
    using MODEL_CLASS = ModelType<RmmType::Zone>;
    auto filter = [&dp] (const MODEL_CLASS& chassis) -> bool {
        return (chassis.get_parent_chassis() == dp.rack_uuid && chassis.get_slot_id() == dp.location_offset); };
    return module::get_manager<MODEL_CLASS>().get_keys(filter);
}

template <>
KeysVec get_resources<RmmType::RackVlan>(const DiscoveryParams<RmmType::RackVlan>& dp, const std::string& parent) {
    using MODEL_CLASS = ModelType<RmmType::RackVlan>;
    auto filter = [&dp] (const MODEL_CLASS& vlan) -> bool {
        return (vlan.get_vlan_id() == dp.vlan_id); };
    return module::get_manager<MODEL_CLASS>().get_keys(parent, filter);
}

}
}
}
}
