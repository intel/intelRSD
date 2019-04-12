/*!
 * @brief IPMI service utilities implementation.
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
 * @file utils.cpp
 */

#include "ipmb/utils.hpp"
#include <sstream>



using namespace agent::compute::ipmb;
using agent_framework::generic::Version;


uint8_t agent::compute::ipmb::IPMI_DEVICE_REV_MAJOR() {
    return uint8_t(agent_framework::generic::get_version_component(Version::VERSION_STRING, Version::MAJOR_INDEX));
}


uint8_t agent::compute::ipmb::IPMI_DEVICE_REV_MINOR() {
    return uint8_t(agent_framework::generic::get_version_component(Version::VERSION_STRING, Version::MINOR_INDEX));
}


uint32_t agent::compute::ipmb::IPMI_AUX_FW_REV() {
    return uint32_t(agent_framework::generic::get_version_component(Version::VERSION_STRING, Version::BUILD_INDEX));
}


uint8_t agent::compute::ipmb::IPMI_IANA_INTEL_0() { return (IPMI_IANA_INTEL & 0xff); }


uint8_t agent::compute::ipmb::IPMI_IANA_INTEL_1() { return (IPMI_IANA_INTEL >> 8) & 0xff; }


uint8_t agent::compute::ipmb::IPMI_IANA_INTEL_2() { return (IPMI_IANA_INTEL >> 16) & 0xff; }


/* IPMI Message */
uint8_t agent::compute::ipmb::IPMI_MSG_SEQ(uint8_t byte) { return uint8_t(byte >> 2); }


uint8_t agent::compute::ipmb::IPMI_MSG_NETFN(uint8_t byte) { return uint8_t((byte >> 2) & 0x3f); }


uint8_t agent::compute::ipmb::IPMI_MSG_SET_REQ_NETFN_DLUN(const uint8_t netfn, const uint8_t dlun) {
    return uint8_t((netfn << 2) | dlun);
}


uint8_t agent::compute::ipmb::IPMI_MSG_SET_RSP_NETFN_DLUN(const uint8_t netfn, const uint8_t dlun) {
    return uint8_t(((netfn + 1) << 2) | dlun);
}


uint8_t agent::compute::ipmb::IPMI_MSG_SET_SEQ_SLUN(const uint8_t seq, const uint8_t slun) {
    return uint8_t((seq << 2) | slun);
}


std::string utils::print_frame(const uint8_t* buffer, const uint16_t len) {
    std::string msg_body_string{};

    if (buffer && len > 0) {
        for (uint16_t i = 0; i < len; i++) {
            std::stringstream ss{};
            ss.width(2);
            ss.fill('0');
            ss << std::hex << static_cast<unsigned>(buffer[i]);
            msg_body_string += " " + ss.str();
        }
    }

    return msg_body_string;
}


uint8_t agent::compute::ipmb::GET_BYTE(const uint32_t value, const uint8_t byte) {
    if (byte > 3) {
        return 0;
    }
    return uint8_t((value >> (byte * 8)) & 0x000000ff);
}


uint8_t utils::ipmi_checksum(const uint8_t* buffer, uint16_t len) {
    uint8_t checksum = 0;

    for (; len > 0; len--, buffer++) {
        checksum = uint8_t(checksum + *buffer);
    }
    return uint8_t(-checksum);

}



