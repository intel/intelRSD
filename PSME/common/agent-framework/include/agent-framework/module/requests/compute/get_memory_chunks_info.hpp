/*!
 * @brief Compute GetMemoryChunksInfo request
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
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
 * @file get_memory_chunks_info.hpp
 */

#pragma once
#include "agent-framework/module/constants/compute.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/validators/procedure_validator.hpp"
#include "agent-framework/module/utils/uuid.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

/*! GetMemoryChunksInfo request */
class GetMemoryChunksInfo {
public:
    explicit GetMemoryChunksInfo(const Uuid& memory_chunks);

    static std::string get_command() {
        return literals::Command::GET_MEMORY_CHUNKS_INFO;
    }

    /*!
     * @brief Get memory chunks from request
     * @return uuid string
     * */
    const Uuid& get_uuid() const {
        return m_memory_chunks;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    json::Json to_json() const;

    /*!
     * @brief create GetMemoryChunksInfo from Json
     *
     * @param[in] json the input argument
     *
     * @return new GetMemoryChunksInfo
     */
    static GetMemoryChunksInfo from_json(const json::Json& json);

    /*!
     * @brief Returns procedure scheme
     * @return Procedure scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_OBJECT,
            literals::MemoryChunks::MEMORY_CHUNKS, VALID_UUID,
            nullptr
        };
        return procedure;
    }
private:
    Uuid m_memory_chunks{};
};

}
}
}