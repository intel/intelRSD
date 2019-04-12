/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file regular_expressions.cpp
 * @brief Contains constants used by regex validator
 * */

#include "agent-framework/module/constants/regular_expressions.hpp"


namespace agent_framework {
namespace model {
namespace literals {
namespace regex {

constexpr const char RemoteTarget::TARGET_IQN[];
constexpr const char RemoteTarget::INITIATOR_IQN[];

constexpr const char EthernetInterface::MAC_ADDRESS[];

constexpr const char Common::DEVICE_ID[];
constexpr const char Common::VENDOR_ID[];
constexpr const char Common::SUBSYSTEM_ID[];
constexpr const char Common::SUBSYSTEM_VENDOR_ID[];
constexpr const char Common::DATE_TIME_LOCAL_OFFSET[];
constexpr const char Common::NO_WHITESPACE_STRING[];
constexpr const char Common::EMPTY_OR_NO_WHITESPACE_STRING[];

constexpr const char PCIeFunction::CLASS_CODE[];
constexpr const char PCIeFunction::REVISION_ID[];

constexpr const char Event::MESSAGE_ID[];

constexpr const char ServiceRoot::REDFISH_VERSION[];

constexpr const char Endpoint::PCI_CLASS_CODE[];

constexpr const char IPAddresses::ADDRESS[];
constexpr const char IPAddresses::GATEWAY[];
constexpr const char IPAddresses::SUBNET_MASK[];

constexpr const char IPv6Addresses::ADDRESS[];

constexpr const char Chassis::LOCATION_ID[];
}
}
}
}
