/*!
 * @brief Implementation of ostream operator overloads.
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
 * @file iscsi_structs_ostream.cpp
 */

#include "iscsi/structs/iscsi_mdr_header.hpp"
#include "iscsi/structs/iscsi_mdr_initiator.hpp"
#include "iscsi/structs/iscsi_mdr_target.hpp"

#include <ostream>

namespace iscsi {
namespace structs {

std::ostream& operator<<(std::ostream& os, const IpAddressType& iat) {
    switch (iat) {
        case IpAddressType::AUTO:
            os << "Auto";
            break;
        case IpAddressType::IPv4:
            os << "IPv4";
            break;
        case IpAddressType::IPv6:
            os << "IPv6";
            break;
        default:
            os << "Unknown IP Address Type";
            break;
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const AuthenticationMethod& ids) {
    switch (ids) {
        case AuthenticationMethod::NO_CHAP:
            os << "NO_CHAP";
            break;
        case AuthenticationMethod::CHAP:
            os << "CHAP";
            break;
        case AuthenticationMethod::MUTUAL_CHAP:
            os << "MUTUAL_CHAP";
            break;
        default:
            os << "Unknown Identification Method";
            break;
    }

    return os;
}

std::ostream& operator<<(std::ostream& os, const TargetDhcpStatus& tds) {
    os << static_cast<uint8_t>(tds);
    return os;
}

std::ostream& operator<<(std::ostream& os, const VlanStatus& vs) {
    os << static_cast<uint8_t>(vs);
    return os;
}

std::ostream& operator<<(std::ostream& os, const RouterAdvertisement& rs) {
    os << static_cast<uint8_t>(rs);
    return os;
}

std::ostream& operator<<(std::ostream& os, const InitiatiorDhcpStatus& ids) {
    os << static_cast<uint8_t>(ids);
    return os;
}

}  // namespace structs
}  // namespace iscsi
