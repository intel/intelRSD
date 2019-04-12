/*!
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
 * @brief Declaration of function processing Set Component Attributes command on Processor
 * @file set_processor_attributes.hpp
 */

#pragma once



#include "fpgaof_agent_commands.hpp"



namespace agent {
namespace fpgaof {
namespace command {

/*!
 * @brief Implementation of SetProcessorAttributes
 * @param context Pointer to agent context
 * @param[in] uuid The UUID of the processor
 * @param[in] attributes The attributes to be set
 * @param[out] response The response to SetComponentAttributes request
 * */
void set_processor_attributes(AgentContext::SPtr context,
                              const Uuid& uuid,
                              const agent_framework::model::attribute::Attributes& attributes,
                              SetComponentAttributes::Response& response);

}
}
}
