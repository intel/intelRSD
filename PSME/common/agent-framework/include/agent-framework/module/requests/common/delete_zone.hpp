/*!
 * @copyright Copyright (c) 2016-2019 Intel Corporation.
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
 * @file delete_zone.hpp
 */

#pragma once
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/module/model/attributes/oem.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

/*! DeleteZone request */
class DeleteZone {
public:

    explicit DeleteZone(const std::string& zone);

    static std::string get_command() {
        return literals::Command::DELETE_ZONE;
    }

    /*!
     * @brief Get PCIeZone UUID from request
     * @return UUID string
     * */
    const std::string& get_zone() const {
        return m_zone;
    }

    /*!
     * @brief Transform request to Json
     * @return created Json value
     */
    json::Json to_json() const;

    /*!
     * @brief create DeleteZone form Json
     * @param[in] json the input argument
     * @return new DeleteZone
     */
    static DeleteZone from_json(const json::Json& json);

    /*!
     * @brief Returns procedure scheme
     * @return ProcedureValidator scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
                get_command(),
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_OBJECT,
                literals::Zone::ZONE, VALID_UUID,
                nullptr
        };
        return procedure;
    }
private:
    std::string m_zone{};
};

}
}
}
