/*!
 * @brief FPGA processor secure erase task declaration.
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
 * @file fpga_secure_erase_task.hpp
 */

#pragma once



#include <string>
#include "fpgaof_agent_context.hpp"



namespace agent {
namespace fpgaof {
namespace fpga {

/*! @brief  FPGA processor secure erase task class */
class FpgaSecureEraseTask {
public:
    /*!
     * @brief Constructor
     * @param agent_context pointer to AgentContext object for FPGAoF agent
     * @param[in] processor_uuid FPGA processor to be erased
     * */
    explicit FpgaSecureEraseTask(const std::shared_ptr<agent::fpgaof::AgentContext> agent_context,
                                 const std::string& processor_uuid);


    /*! @brief Starts erasing */
    void operator()();


private:
    std::shared_ptr<AgentContext> m_agent_context;
    Uuid m_processor_uuid{};


    std::string get_processor_uuid() const;
};

}
}
}

