/*!
 * @brief iSCSI MDR initiator structure builder.
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
 * @file iscsi_mdr_initiator_builder.hpp
 */

#pragma once

#include "ipmi/ipmi_interface.hpp"
#include "iscsi/iscsi_mdr_builder_base.hpp"
#include "iscsi/structs/iscsi_mdr_initiator.hpp"

#include <cstddef>

namespace iscsi {
namespace builder {

/*!
 * @brief The iSCSI MDR initiator structure builder.
 */
class IscsiInitiatorBuilder final : public IscsiMdrBuilderBase {
public:

    /*!
     * @brief Deleted constructor.
     */
    IscsiInitiatorBuilder() = delete;

    /*!
     * @brief Append an initiator structure to the IPMI ByteBuffer.
     *
     * @param buffer the buffer to append the structure to.
     * @param device_functions the network device functions context to build from.
     */
    template<typename DevFuncCtx>
    static void append_structure(ipmi::IpmiInterface::ByteBuffer& buffer, const DevFuncCtx& ctx,
        std::function<uint16_t()> handle) {
        const auto& boot = ctx.get_network_device_function().get_iscsi_boot();
        std::vector<std::string> strings;
        uint8_t string_index = {1};
        auto inc_idx = [&string_index]() -> uint8_t {return string_index++;};

        push_uint8(buffer, static_cast<uint8_t>(iscsi::structs::MdrStructType::INITIATOR));
        push_uint16(buffer, static_cast<uint16_t>(sizeof(iscsi::structs::ISCSI_MDR_INITIATOR)));
        push_uint16(buffer, handle());
        push_uint8(buffer, static_cast<uint8_t>(ctx.get_dhcp_enabled(boot.get_ip_mask_dns_via_dhcp())));
        push_string(buffer, strings, boot.get_initiator_name(), inc_idx);
        push_uint8(buffer, static_cast<uint8_t>(ctx.get_ip_type(boot.get_ip_address_type())));
        push_ip(buffer, boot.get_initiator_address(), iscsi::structs::IP_ADDRESS_SIZE);
        push_ip(buffer, boot.get_initiator_netmask(), iscsi::structs::IP_ADDRESS_SIZE);
        push_ip(buffer, boot.get_initiator_default_gateway(), iscsi::structs::IP_ADDRESS_SIZE);
        push_ip(buffer, boot.get_primary_dns(), iscsi::structs::IP_ADDRESS_SIZE);
        push_ip(buffer, boot.get_secondary_dns(), iscsi::structs::IP_ADDRESS_SIZE);
        push_mac(buffer, ctx.get_network_device_function().get_mac_address(), iscsi::structs::MAC_ADDRESS_SIZE);

        append_strings(buffer, strings);
    }
};

}  // namespace builder
}  // namespace iscsi
