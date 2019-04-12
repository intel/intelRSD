/*!
 * @brief Implementation of FPGA secure erase.
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
 * @file fpga_secure_erase.cpp
 */


#include "fpga/fpga_secure_erase.hpp"
#include "opaepp_error.hpp"
#include <fstream>



using namespace agent::fpgaof::fpga;
using namespace agent::fpgaof;
using namespace opaepp;


void agent::fpgaof::fpga::secure_erase(const std::shared_ptr<AgentContext> agent_context, const Uuid& processor_uuid) {

    auto host_uuid = OpaeProxyHostApi::get_device_owner_host_id(*agent_context->opae_proxy_context, processor_uuid);

    if (host_uuid.has_value()) {
        OpaeProxyHostApi::remove_device_ownership(*agent_context->opae_proxy_context, processor_uuid);
    }

    std::vector<uint8_t> gbs;

    std::ifstream gbs_file(agent_context->configuration->get_secure_erase_gbs_file_path(), std::ios::binary);

    if (gbs_file) {
        gbs_file.unsetf(std::ios::skipws);
        gbs_file.seekg(0, std::ios::end);
        std::streampos file_size = gbs_file.tellg();
        gbs_file.seekg(0, std::ios::beg);

        gbs.reserve(size_t(file_size));

        gbs.insert(gbs.begin(),
                   std::istream_iterator<std::uint8_t>(gbs_file),
                   std::istream_iterator<std::uint8_t>());
        gbs_file.close();
    }

    OpaeProxyHostApi::reconfigure_slot(*agent_context->opae_proxy_context, processor_uuid, gbs);
}
