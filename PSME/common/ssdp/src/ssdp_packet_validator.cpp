/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * */

#include "ssdp/ssdp_packet_validator.hpp"
#include "ssdp/ssdp_packet.hpp"

#include <unordered_map>

using namespace ssdp;

SsdpPacketValidator::~SsdpPacketValidator() {}

bool SsdpNotifyPacketValidator::is_valid(const SsdpPacket& packet) const {
    if (packet.get_type() != SsdpPacket::Type::NOTIFY) {
        return false;
    }

    if (!packet.has_header(SsdpPacket::HOST)
            || !packet.has_header(SsdpPacket::NT)
            || !packet.has_header(SsdpPacket::NTS)
            || !packet.has_header(SsdpPacket::USN)) {
        return false;
    }

    return validate_subtype(packet);
}

bool SsdpAlivePacketValidator::validate_subtype(const SsdpPacket& packet) const {
    return  SsdpPacket::SSDP_ALIVE_STR == packet.get_header(SsdpPacket::NTS)
            && packet.has_header(SsdpPacket::CACHE_CONTROL)
            && (packet.has_header(SsdpPacket::AL)
                || packet.has_header(SsdpPacket::LOCATION));
}

bool SsdpByeByePacketValidator::validate_subtype(const SsdpPacket& packet) const {
    return  SsdpPacket::SSDP_BYEBYE_STR == packet.get_header(SsdpPacket::NTS);
}

bool SsdpSearchRequestPacketValidator::is_valid(const SsdpPacket& packet) const {
    if (SsdpPacket::Type::SEARCH_REQUEST == packet.get_type()) {
        if (!packet.has_header(SsdpPacket::HOST) || !packet.has_header(SsdpPacket::ST)
            || !packet.has_header(SsdpPacket::MAN) || !packet.has_header(SsdpPacket::MX)) {
            return false;
        }
        if (packet.get_header(SsdpPacket::MAN) != SsdpPacket::SSDP_DISCOVER) {
            return false;
        }
        return true;
    }
    return false;
}

bool SsdpSearchResponsePacketValidator::is_valid(const SsdpPacket& packet) const {
    if (SsdpPacket::Type::SEARCH_RESPONSE == packet.get_type()) {
        if (!packet.has_header(SsdpPacket::CACHE_CONTROL)
             || !packet.has_header(SsdpPacket::ST)
             || !(packet.has_header(SsdpPacket::AL) || packet.has_header(SsdpPacket::LOCATION))
             || !packet.has_header(SsdpPacket::USN)) {
            return false;
        }
        return true;
    }
    return false;
}

using SsdpPacketValidatorsMap = std::unordered_map<SsdpPacketKey, SsdpPacketValidator::Ptr>;

static SsdpPacketValidatorsMap create_validators_map() {
    SsdpPacketValidatorsMap validators;
    validators.emplace(SsdpPacketKey{SsdpPacket::Type::NOTIFY, SsdpPacket::SSDP_ALIVE_STR},
                       SsdpPacketValidator::Ptr(new SsdpAlivePacketValidator));
    validators.emplace(SsdpPacketKey{SsdpPacket::Type::NOTIFY, SsdpPacket::SSDP_BYEBYE_STR},
                       SsdpPacketValidator::Ptr(new SsdpByeByePacketValidator));
    validators.emplace(SsdpPacket::Type::SEARCH_REQUEST,
                       SsdpPacketValidator::Ptr(new SsdpSearchRequestPacketValidator));
    validators.emplace(SsdpPacket::Type::SEARCH_RESPONSE,
                       SsdpPacketValidator::Ptr(new SsdpSearchResponsePacketValidator));
    return validators;
}

bool ssdp::validate_ssdp_packet(const SsdpPacket& packet) {
    static const auto validators = create_validators_map();
    const auto it = validators.find(SsdpPacketKey(packet));
    if (it != validators.cend()) {
        return it->second->is_valid(packet);
    }
    return false;
}
