/*!
 * @brief OPAE proxy host API implementation
 *
 * @copyright Copyright (c) 2019 Intel Corporation
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
 * @file opae_proxy_host_api.cpp
 */


#include "opaepp/opae-proxy/opae_proxy_host_api.hpp"
#include "opaepp_error.hpp"
#include <uuid/uuid.h>
#include "uuid/uuid.hpp"



using namespace opaepp;

namespace {

std::vector<uint8_t> parse_uuid(const std::string& _32_bytes_uuid) {
    std::vector<std::uint8_t> hex_uuid(sizeof(uuid_t));
    auto result_code = uuid_parse(_32_bytes_uuid.data(), hex_uuid.data());
    if (result_code != 0) {
        hex_uuid.resize(0);
    }
    return hex_uuid;
}


std::vector<std::uint8_t> convert_to_fpga_guid(const Uuid& model_uuid, const std::string& command_name) {
    auto parsed_uuid = parse_uuid(model_uuid.data());
    if (parsed_uuid.empty()) {
        OpaeApiError error(OpaeApiFpgaResultCode::FPGA_INVALID_PARAM, command_name, "Model UUID format is invalid");
        throw error;
    }
    return parsed_uuid;
}

}


void OpaeProxyHostApi::add_initiator_host(OpaeProxyContext& opae_proxy_context, const Uuid& host_uuid) {
    auto uuid_bytes = convert_to_fpga_guid(host_uuid, "add_initiator_host");
    auto result = fpgaofAddInitiatorHost(opae_proxy_context.get_backend(), uuid_bytes.data());
    OpaeApiError::throw_if_unexpected_result(result, "add_initiator_host");
}


void OpaeProxyHostApi::remove_initiator_host(OpaeProxyContext& opae_proxy_context, const Uuid& host_uuid) {
    auto uuid_bytes = convert_to_fpga_guid(host_uuid, "remove_initiator_host");
    auto result = fpgaofRemoveInitiatorHost(opae_proxy_context.get_backend(), uuid_bytes.data());
    OpaeApiError::throw_if_unexpected_result(result, "remove_initiator_host");
}


std::vector<Uuid>
OpaeProxyHostApi::get_initiator_hosts(OpaeProxyContext& opae_proxy_context, const uint32_t max_hosts) {
    std::vector<Uuid> host_uuids;
    uint32_t returned_host_count;
    std::unique_ptr<uint8_t[]> uuid_array(new uint8_t[max_hosts * sizeof(fpga_guid)]);

    uint8_t* raw_ptr = uuid_array.get();
    auto result = fpgaofGetInitiatorHosts(opae_proxy_context.get_backend(), reinterpret_cast<fpga_guid*>(&raw_ptr),
                                          max_hosts, &returned_host_count);
    OpaeApiError::throw_if_unexpected_result(result, "get_initiator_hosts");
    host_uuids.reserve(returned_host_count);
    for (size_t i = 0; i < returned_host_count; ++i) {
        host_uuids.push_back(unparse_uuid((const void*) (&uuid_array[i * sizeof(fpga_guid)])));
    }

    return host_uuids;
}


void OpaeProxyHostApi::set_device_ownership(OpaeProxyContext& opae_proxy_context, const Uuid& host_uuid,
                                            const Uuid& processor_uuid) {
    auto token = get_token_from_uuid(opae_proxy_context, processor_uuid, "set_device_ownership");
    auto uuid_bytes = convert_to_fpga_guid(host_uuid, "set_device_ownership");
    auto result = fpgaofSetDeviceOwnership(opae_proxy_context.get_backend(), uuid_bytes.data(), token);
    OpaeApiError::throw_if_unexpected_result(result, "set_device_ownership");
}


void OpaeProxyHostApi::remove_device_ownership(OpaeProxyContext& opae_proxy_context, const Uuid& processor_uuid) {
    auto token = get_token_from_uuid(opae_proxy_context, processor_uuid, "remove_device_ownership");
    auto result = fpgaofRemoveDeviceOwnership(opae_proxy_context.get_backend(), token);
    OpaeApiError::throw_if_unexpected_result(result, "remove_device_ownership");
}


OptionalField<Uuid>
OpaeProxyHostApi::get_device_owner_host_id(OpaeProxyContext& opae_proxy_context, const Uuid& processor_uuid) {
    auto token = get_token_from_uuid(opae_proxy_context, processor_uuid, "get_device_owner_host_id");
    fpga_guid host_uuid;
    bool is_owned;
    auto result = fpgaofGetDeviceOwnerHostId(opae_proxy_context.get_backend(), token, &host_uuid, &is_owned);
    OpaeApiError::throw_if_unexpected_result(result, "get_device_owner_host_id");
    return is_owned ? OptionalField<Uuid>(unparse_uuid(static_cast<const void*>(host_uuid))) : OptionalField<Uuid>();
}


void OpaeProxyHostApi::reconfigure_slot(OpaeProxyContext& opae_proxy_context, const Uuid& processor_uuid,
                                        const std::vector<uint8_t>& bitstream_bytes) {
    auto token = get_token_from_uuid(opae_proxy_context, processor_uuid, "remove_device_ownership");
    fpga_handle handle;
    auto result = fpgaofOpen(opae_proxy_context.get_backend(), token, &handle, 0);
    OpaeApiError::throw_if_unexpected_result(result, "reconfigure_slot::fpgaofOpen");
    result = fpgaofReconfigureSlot(opae_proxy_context.get_backend(), handle, 0, bitstream_bytes.data(),
                                   bitstream_bytes.size(), 0);
    OpaeApiError::throw_if_unexpected_result(result, "reconfigure_slot::fpgaofReconfigureSlot");
}


fpga_token& OpaeProxyHostApi::get_token_from_uuid(OpaeProxyContext& opae_proxy_context, const Uuid& processor_uuid,
                                                  const std::string& command_name) {
    auto token_it = opae_proxy_context.get_proxy_device_ids().find(processor_uuid);
    if (token_it == opae_proxy_context.get_proxy_device_ids().end()) {
        throw OpaeppError("OpaeProxyHostApi::" + command_name + " - cannot find token for processor with UUID: " +
                          processor_uuid);
    }
    return token_it->second;
}