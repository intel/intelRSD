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
 * @file rmm/discovery/helpers/common.cpp
 */

#include "discovery/helpers/common.hpp"

namespace agent {
namespace rmm {
namespace discovery {
namespace helpers {

DiscoveryAction get_action_type(bool was_present, bool is_present) {
    if (is_present) {
        return was_present ? DiscoveryAction::Update : DiscoveryAction::Add;
    }
    else {
        return was_present ? DiscoveryAction::Remove : DiscoveryAction::None;
    }
}

OptionalField<enums::Ipv4AddressOrigin> convert_ipmi_address_source_to_model_address_origin(
        ipmi::command::generic::LanConfigIpAddressSource source) {
    OptionalField<enums::Ipv4AddressOrigin> origin{};
    switch(source) {
    case ipmi::command::generic::LanConfigIpAddressSource::DHCP:
        origin = enums::Ipv4AddressOrigin::DHCP;
        break;
    case ipmi::command::generic::LanConfigIpAddressSource::STATIC:
        origin = enums::Ipv4AddressOrigin::Static;
        break;
    case ipmi::command::generic::LanConfigIpAddressSource::UNSPECIFIED:
    case ipmi::command::generic::LanConfigIpAddressSource::BIOS:
    case ipmi::command::generic::LanConfigIpAddressSource::OTHER:
    default:
        /* Unrecognized value - do nothing */
        break;
    }
    return origin;
}

OptionalField<std::string> to_optional_string(const std::string& str) {
    return str.empty() ? OptionalField<std::string>{} : OptionalField<std::string>{str};
}

std::string generic_stabilizer(const enums::Component& component, const std::string& uuid, StabilizeFnPtr fn) {
    try {
        RmmTreeStabilizer stabilizer{};
        return (stabilizer.*fn)(uuid);
    }
    catch (const std::exception& e) {
        log_error("rmm-discovery", "Unable to stabilize " << component.to_string()
            << " " << uuid << " : " << e.what());
        return uuid;
    }
}

}
}
}
}
