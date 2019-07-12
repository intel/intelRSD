/*!
 * @brief GetLogEntryInfo request
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
 * @file get_log_entry_info.hpp
 */

#pragma once
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/validators/procedure_validator.hpp"
#include "agent-framework/module/utils/uuid.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

/*! GetLogEntryInfo request */
class GetLogEntryInfo {
public:
    explicit GetLogEntryInfo(const Uuid& log_entry);

    static std::string get_command() {
        return literals::Command::GET_LOG_ENTRY_INFO;
    }

    /*!
     * @brief Get log entry UUID from request
     * @return uuid string
     * */
    const Uuid& get_uuid() const {
        return m_log_entry;
    }

    /*!
     * @brief Transform request to JSON
     *
     * @return created JSON value
     */
    json::Json to_json() const;

    /*!
     * @brief create GetLogEntry form JSON
     *
     * @param[in] json the input argument
     *
     * @return new GetLogEntry
     */
    static GetLogEntryInfo from_json(const json::Json& json);

    /*!
     * @brief Returns procedure scheme
     * @return Procedure scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
                get_command(),
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_OBJECT,
                literals::LogEntry::LOG_ENTRY, VALID_UUID,
                nullptr
        };
        return procedure;
    }
private:
    Uuid m_log_entry{};
};

}
}
}
