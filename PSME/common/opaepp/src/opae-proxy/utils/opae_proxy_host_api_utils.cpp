/*!
 * @brief OPAE proxy host API utils implementation
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
 * @file opae_proxy_host_api_utils.cpp
 */


#include "opaepp/opae-proxy/utils/opae_proxy_host_api_utils.hpp"



using namespace opaepp;


PcieDeviceAddress
OpaeProxyHostApiUtils::get_pcie_fpga_address(std::shared_ptr<OpaeProxyContext> opae_proxy_context,
                                             const Uuid& processor_uuid) {

    auto token = OpaeProxyHostApi::get_token_from_uuid(*opae_proxy_context, processor_uuid,
                                                       "get_token_from_uuid");

    OpaeProxyDeviceReader reader(opae_proxy_context);

    Device device = reader.discover(token);

    PcieDeviceAddress pcie_address_device(device.get_bus(), device.get_device(), device.get_function());

    return pcie_address_device;
}