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
 * @file requests/common/get_collection.hpp
 * @brief common GetCollection request
 * */

#pragma once
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

/*! GetCollection request*/
class GetCollection {
public:
    explicit GetCollection(const std::string& component,
            const std::string& name);

    static std::string get_command() {
        return literals::Command::GET_COLLECTION;
    }
    /*!
     * @brief Get component UUID
     * @return Component UUID
     * */
    const std::string& get_uuid() const {
        return m_component;
    }

    /*!
     * @brief Get collection name
     * @return Collection name
     * */
    const std::string& get_name() const {
        return m_name;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    json::Json to_json() const;

    /*!
     * @brief create GetCollection form Json
     *
     * @param[in] json the input argument
     *
     * @return new GetCollection
     */
    static GetCollection from_json(const json::Json& json);

    /*!
     * @brief Returns procedure scheme
     * @return Procedure scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
                get_command(),
                jsonrpc::PARAMS_BY_NAME,
                jsonrpc::JSON_STRING,
                literals::Collection::COMPONENT, jsonrpc::JSON_STRING,
                literals::Collection::NAME, jsonrpc::JSON_STRING,
                nullptr
        };
        return procedure;
    }

private:
        std::string m_component{};
        std::string m_name{};
};

}
}
}
