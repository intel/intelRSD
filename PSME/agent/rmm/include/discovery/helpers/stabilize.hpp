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
 * @file rmm/discovery/helpers/stabilize.hpp
 */

#pragma once

namespace agent {
namespace rmm {
namespace discovery {
namespace helpers {

/*!
 * Helper method for stabilizing model classes uuids. It is a template that redirects calls to a proper tree
 * stabilizer member function. This method has to be specialized for each class used. Note that most of the
 * stabilize_* methods are const. In case of the 'stabilize' used to stabilize Managers which is a non-const
 * member function, it is required to cast it to a pointer to a const member function (which is always safe).
 * */
template<RmmType TYPE>
constexpr StabilizeFnPtr get_stabilize_fn() {
    static_assert(fail_assert<TYPE>(), "Stabilize specialization required");
    return reinterpret_cast<StabilizeFnPtr>(nullptr);
}

template<> constexpr StabilizeFnPtr get_stabilize_fn<RmmType::RackManager>() { return reinterpret_cast<StabilizeFnPtr>(&RmmTreeStabilizer::stabilize); }
template<> constexpr StabilizeFnPtr get_stabilize_fn<RmmType::ZoneManager>() { return reinterpret_cast<StabilizeFnPtr>(&RmmTreeStabilizer::stabilize); }
template<> constexpr StabilizeFnPtr get_stabilize_fn<RmmType::RackInterface>() { return &RmmTreeStabilizer::stabilize_network_interface; }
template<> constexpr StabilizeFnPtr get_stabilize_fn<RmmType::ZoneInterface>() { return &RmmTreeStabilizer::stabilize_network_interface; }
template<> constexpr StabilizeFnPtr get_stabilize_fn<RmmType::RackVlan>() { return &RmmTreeStabilizer::stabilize_port_vlan; }
template<> constexpr StabilizeFnPtr get_stabilize_fn<RmmType::Rack>() { return &RmmTreeStabilizer::stabilize_chassis; }
template<> constexpr StabilizeFnPtr get_stabilize_fn<RmmType::Zone>() { return &RmmTreeStabilizer::stabilize_chassis; }
template<> constexpr StabilizeFnPtr get_stabilize_fn<RmmType::Drawer>() { return &RmmTreeStabilizer::stabilize_chassis; }
template<> constexpr StabilizeFnPtr get_stabilize_fn<RmmType::PowerZone>() { return &RmmTreeStabilizer::stabilize_power_zone; }
template<> constexpr StabilizeFnPtr get_stabilize_fn<RmmType::ThermalZone>() { return &RmmTreeStabilizer::stabilize_thermal_zone; }
template<> constexpr StabilizeFnPtr get_stabilize_fn<RmmType::Psu>() { return &RmmTreeStabilizer::stabilize_psu; }
template<> constexpr StabilizeFnPtr get_stabilize_fn<RmmType::Fan>() { return &RmmTreeStabilizer::stabilize_fan; }

/*!
 * This method is responsible for generating persistent uuid for a resource of a specific type. It assumes the resource
 * is already in the model and it has all information required for successful stabilization. It returns new uuid of the
 * resource.
 * */
template<RmmType TYPE>
std::string stabilize(const std::string& uuid) {
    return generic_stabilizer(get_model_component<TYPE>(), uuid, get_stabilize_fn<TYPE>());
}

}
}
}
}
