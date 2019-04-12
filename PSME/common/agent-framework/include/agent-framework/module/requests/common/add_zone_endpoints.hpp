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
 * @file add_zone_endpoints.hpp
 */

#pragma once



#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/constants/pnc.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/module/model/attributes/array.hpp"
#include "agent-framework/module/model/attributes/oem.hpp"
#include "agent-framework/module/utils/uuid.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <string>



namespace agent_framework {
namespace model {
namespace requests {

/*! AddZoneEndpoints request */
class AddZoneEndpoints {
public:

    using UuidArray = agent_framework::model::attribute::Array<Uuid>;


    explicit AddZoneEndpoints(const Uuid& zone, const UuidArray& endpoints, const attribute::Oem& oem);


    static std::string get_command() {
        return literals::Command::ADD_ZONE_ENDPOINTS;
    }


    /*!
     * @brief Get zone uuid from request
     * @return uuid string
     * */
    const Uuid& get_zone() const {
        return m_zone;
    }


    /*!
     * @brief Get endpoints uuids from request
     * @return uuids collection
     * */
    const UuidArray& get_endpoints() const {
        return m_endpoints;
    }


    /*!
     * @brief Get oem from request
     * @return oem
     * */
    const attribute::Oem& get_oem() const {
        return m_oem;
    }


    /*!
     * @brief Transform request to Json
     * @return created Json value
     */
    json::Json to_json() const;


    /*!
     * @brief create AddZoneEndpoints from Json
     * @param[in] json the input argument
     * @return new AddZoneEndpoints
     */
    static AddZoneEndpoints from_json(const json::Json& json);


    /*!
     * @brief Returns procedure scheme
     * @return ProcedureValidator scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_OBJECT,
            literals::Zone::ZONE, jsonrpc::JSON_STRING,
            literals::Zone::ENDPOINTS, jsonrpc::JSON_ARRAY,
            literals::Zone::OEM, jsonrpc::JSON_OBJECT,
            nullptr
        };
        return procedure;
    }


private:
    Uuid m_zone{};
    UuidArray m_endpoints{};
    attribute::Oem m_oem{};
};

}
}
}
