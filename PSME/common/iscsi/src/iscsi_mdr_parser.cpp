/*!
 * @brief Implementation of the iSCSI MDR region parser.
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
 * @file iscsi_mdr_parser.cpp
 */

#include "iscsi/iscsi_mdr_parser.hpp"
#include "iscsi/structs/iscsi_mdr_attempt.hpp"
#include "iscsi/structs/iscsi_mdr_header.hpp"
#include "iscsi/structs/iscsi_mdr_initiator.hpp"
#include "iscsi/structs/iscsi_mdr_target.hpp"
#include "iscsi/structs/iscsi_mdr_version.hpp"
#include "mdr/printers.hpp"

#include <memory>
#include <arpa/inet.h>
#include <iomanip>
#include <ostream>
#include <sstream>

namespace iscsi {
namespace parser {

IscsiMdrTraits::Exception::~Exception() {}

}  // namespace parser
}  // namespace iscsi

namespace mdr {

template<>
std::shared_ptr<const uint8_t> iscsi::parser::IscsiMdrParser::init_buffer(const uint8_t* _buf, size_t _buf_size) {
    if (nullptr == _buf) {
        throw Exception("Invalid iSCSI MDR blob pointer");
    }
    auto buffer = std::shared_ptr<const uint8_t>(new uint8_t[_buf_size], std::default_delete<uint8_t[]>());
    std::copy(_buf, _buf + _buf_size, const_cast<uint8_t*>(buffer.get()));
    return buffer;
}

template<>
std::ostream& operator<<(std::ostream& os, const iscsi::parser::IscsiMdrParser& parser) {
    os << "iSCSI MDR:\n";

    std::uint64_t offset = 0;
    while (offset + sizeof(iscsi::structs::ISCSI_MDR_HEADER) < parser.buf_size) {
        os << "offset : " << std::dec << offset << std::endl;
        const iscsi::structs::ISCSI_MDR_HEADER& header =
            *reinterpret_cast<const iscsi::structs::ISCSI_MDR_HEADER*>(parser.buf.get() + offset);

        if (offset + header.length > parser.buf_size) {
            throw iscsi::parser::IscsiMdrParser::Exception("Unexpectedly reached end of iSCSI MDR blob");
        }

        switch (header.parameter_id) {
            case iscsi::structs::ISCSI_MDR_ATTEMPT::ID: {
                const auto data = parser.read_struct<iscsi::structs::ISCSI_MDR_ATTEMPT>(offset);
                os << data;
                break;
            }
            case iscsi::structs::ISCSI_MDR_INITIATOR::ID: {
                const auto data = parser.read_struct<iscsi::structs::ISCSI_MDR_INITIATOR>(offset);
                os << data;
                break;
            }
            case iscsi::structs::ISCSI_MDR_TARGET::ID: {
                const auto data = parser.read_struct<iscsi::structs::ISCSI_MDR_TARGET>(offset);
                os << data;
                break;
            }
            case iscsi::structs::ISCSI_MDR_VERSION::ID: {
                const auto data = parser.read_struct<iscsi::structs::ISCSI_MDR_VERSION>(offset);
                os << data;
                break;
            }
            default:
                offset += header.length;
                /* Get past trailing string-list - double-null */
                while (offset + 1 < parser.buf_size
                       && uint8_t(*(parser.buf.get() + offset) | *(parser.buf.get() + offset + 1)) != 0) {
                    offset++;
                }

                offset += 2;
                break;
        }
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const StructEnhanced<iscsi::structs::ISCSI_MDR_ATTEMPT>& se) {
    os << "ISCSI_MDR_ATTEMPT\t@" << se.header.handle
       << "\n\tInitiator Handle         : " << std::hex << se.data.initiator_handle
       << "\n\tTarget Handle            : " << std::hex << se.data.target_handle
       << "\n\tConnection Wait Time     : " << std::hex << se.data.connection_wait_time
       << "\n\tConnection Retry Count   : " << std::hex << uint32_t(se.data.connection_retry_count)
       << std::endl;

    return os;
}

std::ostream& operator<<(std::ostream& os, const StructEnhanced<iscsi::structs::ISCSI_MDR_INITIATOR>& se) {
    os << "ISCSI_MDR_INITIATOR\t@" << se.header.handle
       << "\n\tDHCP enabled     : " << std::boolalpha << bool(se.data.initiator_dhcp_enabled)
       << "\n\tName             : " << se.get_string(se.data.initiator_name)
       << "\n\tIP Address Type  : " << se.data.ip_address_type
       << "\n\tIP Address       : " << mdr::print_ip(se.data.initiator_ip_address)
       << "\n\tSubnet Mask      : " << mdr::print_ip(se.data.initiator_subnet_mask)
       << "\n\tGateway          : " << mdr::print_ip(se.data.initiator_gateway)
       << "\n\tPrimary DNS      : " << mdr::print_ip(se.data.initiator_primary_dns)
       << "\n\tSecondary DNS    : " << mdr::print_ip(se.data.initiator_secondary_dns)
       << "\n\tNIC MAC Address  : " << mdr::print_mac(se.data.nic_mac_address)
       << std::endl;

    return os;
}

std::ostream& operator<<(std::ostream& os, const StructEnhanced<iscsi::structs::ISCSI_MDR_TARGET>& se) {
    os << "ISCSI_MDR_TARGET\t@" << se.header.handle
       << "\n\tDHCP enabled             : " << std::boolalpha << bool(se.data.target_dhcp_enabled)
       << "\n\tName                     : " << se.get_string(se.data.target_name)
       << "\n\tIP Address Type          : " << se.data.ip_address_type
       << "\n\tIP Address               : " << mdr::print_ip(se.data.target_ip_address)
       << "\n\tPort                     : " << std::hex << se.data.target_port
       << "\n\tLUN                      : " << std::hex << se.data.target_lun
       << "\n\tVLAN Enabled             : " << std::boolalpha << bool(se.data.vlan_enabled)
       << "\n\tVLAN ID                  : " << std::hex << se.data.vlan_id
       << "\n\tRouter Advertisement     : " << std::boolalpha << bool(se.data.router_advertisement)
       << "\n\tAuthentication Method    : " << se.data.authentication_method
       << "\n\tCHAP User Name           : " << se.get_string(se.data.chap_user_name)
       << "\n\tCHAP Secret              : " << se.get_string(se.data.chap_secret)
       << "\n\tMutual CHAP Name         : " << se.get_string(se.data.mutual_chap_name)
       << "\n\tMutual CHAP Secret       : " << se.get_string(se.data.mutual_chap_secret)
       << std::endl;

    return os;
}

std::ostream& operator<<(std::ostream& os, const StructEnhanced<iscsi::structs::ISCSI_MDR_VERSION>& se) {
    os << "ISCSI_MDR_VERSION\t@" << se.header.handle
       << "\n\tMajor    : " << std::hex << uint32_t(se.data.major_version)
       << "\n\tMinor    : " << std::hex << uint32_t(se.data.minor_version)
       << std::endl;

    return os;
}

}  // namespace mdr
