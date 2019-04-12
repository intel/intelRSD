/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 * @file requests/common/set_component_attributes.hpp
 * @brief Definition of common SetComponentAttributes request
 * */

#pragma once



#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/module/model/attributes/attributes.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

/*! SetComponentAttributes request */
class SetComponentAttributes {
public:
    explicit SetComponentAttributes(const std::string& component,
                                    attribute::Attributes attributes);


    static std::string get_command() {
        return literals::Command::SET_COMPONENT_ATTRIBUTES;
    }


    /*!
     * @brief Get component UUID
     * @return Component UUID
     * */
    const std::string& get_component() const {
        return m_component;
    }


    /*!
     * @brief Get attributes
     * @return Attributes
     * */
    const agent_framework::model::attribute::Attributes& get_attributes() const {
        return m_attributes;
    }


    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    json::Json to_json() const;


    /*!
     * @brief create SetComponentAttributes from Json
     *
     * @param[in] json the input argument
     *
     * @return new SetComponentAttributes
     */
    static SetComponentAttributes from_json(const json::Json& json);


    /*!
     * @brief Returns procedure scheme
     * @return Procedure scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_STRING,
            literals::Component::COMPONENT, jsonrpc::JSON_STRING,
            literals::Component::ATTRIBUTES, jsonrpc::JSON_OBJECT,
            nullptr
        };
        return procedure;
    }


private:
    std::string m_component{};
    agent_framework::model::attribute::Attributes m_attributes{};
};

}
}
}
