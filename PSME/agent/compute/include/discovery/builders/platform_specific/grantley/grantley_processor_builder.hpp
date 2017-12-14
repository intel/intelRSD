/*!
 * @brief Grantley processor builder class interface.
 *
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
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
 * @header{Filesystem}
 * @file grantley_processor_builder.hpp
 */

#pragma once



#include "discovery/builders/processor_builder.hpp"
#include "ipmi/command/sdv/get_processor_info.hpp"



namespace agent {
namespace compute {
namespace discovery {

class GrantleyProcessorBuilder : public ProcessorBuilder {
public:
    /*!
     * @brief Update processor object with data retrieved via IPMI.
     * @param processor Processor object to be updated with discovered data.
     * @param get_processor_info_response GetProcessorInfo IPMI command response object.
     */
    static void update_processor_info(agent_framework::model::Processor& processor,
                                      const ipmi::command::sdv::response::GetProcessorInfo& get_processor_info_response);


    /*!
     * @brief Manually update processor socket ID.
     * @param processor Processor object to be updated.
     * @param socket_id Socket ID.
     */
    static void update_socket_id(agent_framework::model::Processor& processor, std::uint8_t socket_id);
};

}
}
}
