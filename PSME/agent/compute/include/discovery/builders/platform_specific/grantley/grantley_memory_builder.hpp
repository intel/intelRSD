/*!
 * @brief Grantley memory builder class interface.
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
 * @file grantley_memory_builder.hpp
 */

#pragma once



#include "discovery/builders/memory_builder.hpp"
#include "ipmi/command/sdv/get_dimm_info.hpp"



namespace agent {
namespace compute {
namespace discovery {

class GrantleyMemoryBuilder : public MemoryBuilder {
public:
    /*!
     * @brief Update memory object with DIMM memory index.
     * @param memory Memory object to be updated.
     * @param dimm_index DIMM index.
     */
    static void update_memory_index(agent_framework::model::Memory& memory, std::uint8_t dimm_index);


    /*!
     * @brief Update memory object with DIMM info read via IPMI.
     * @param memory Memory object to be updated with discovered data.
     * @param get_dimm_info_response GetDimmInfo IPMI command response object.
     */
    static void update_dimm_info(agent_framework::model::Memory& memory,
                                 const ipmi::command::sdv::response::GetDimmInfo& get_dimm_info_response);
};

}
}
}
