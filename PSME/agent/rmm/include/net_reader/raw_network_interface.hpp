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
 * @file rmm/net_reader/raw_network_interface.hpp
 */

#pragma once

#include "agent-framework/module/utils/utils.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace agent {
namespace rmm {
namespace net_reader {

/*!
 * Class representing ip address
 */
struct RawIpAddress final {
    OptionalField<std::string>  ip{};
    OptionalField<std::string>  netmask{};
};

/*!
 * Class representing raw network interface data
 */
struct RawNetworkInterface final {
    std::string                 name{};
    OptionalField<uint32_t>     mtu{};
    OptionalField<uint32_t>     speed{};
    OptionalField<std::string>  mac_address{};
    OptionalField<std::string>  factory_mac_address{};
    OptionalField<bool>         is_full_duplex{};
    std::vector<RawIpAddress>   addresses{};
    std::vector<uint32_t>       vlans{};
};

}
}
}
