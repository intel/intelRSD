/*!
 * @brief FPGA processor secure erase declaration.
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 *
 * @file fpga_secure_erase.hpp
 */

#pragma once



#include "opaepp/opae-proxy/opae_proxy_host_api.hpp"
#include "fpgaof_agent_context.hpp"
#include <string>


/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace fpgaof {
/*! NVMe namespace */
namespace fpga {

/*!
 * @brief Secure erase of FPGA processor
 *
 * @param agent_context pointer to AgentContext object for FPGAoF agent
 * @param processor_uuid UUID of the FPGA processor to be erased securely
 * */
void secure_erase(const std::shared_ptr<agent::fpgaof::AgentContext> agent_context, const std::string& processor_uuid);


/*!
 * @brief Update reconfigured slot details after secure erase of FPGA processor
 *
 * @param agent_context pointer to AgentContext object for FPGAoF agent
 * @param processor_uuid UUID of the FPGA processor to be erased securely
 * */
void update_reconfigured_slot_details(const std::shared_ptr<agent::fpgaof::AgentContext> agent_context,
                                      const Uuid& processor_uuid);

}
}
}
