/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file command/psme/heart_beat.hpp
 * @brief Generic PSME HeartBeat command
 * */

#pragma once

#include "agent-framework/module/constants/psme.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

namespace agent_framework {
namespace model {
namespace requests {

/*! HeartBeat request */
class HeartBeat {
public:
    explicit HeartBeat(const std::string& gami_id);

    /*!
     * @brief Get command name
     * @return Command name
     * */
    static std::string get_command() {
        return literals::Command::HEART_BEAT;
    }

    /*!
     * @brief Get GAMI id
     * @return gami id
     * */
    const std::string& get_gami_id() const {
        return m_gami_id;
    }

    /*!
     * @brief Transform request to Json
     * @return created Json value
     */
    json::Json to_json() const;

    /*!
     * @brief create HeartBeat form Json
     * @param[in] json the input argument
     * @return new HeartBeat
     */
    static HeartBeat from_json(const json::Json& json);

    /*!
     * @brief Returns procedure scheme
     * @return Procedure scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_OBJECT,
            literals::HeartBeat::GAMI_ID, jsonrpc::JSON_STRING,
            nullptr};
        return procedure;
    }

private:
    std::string m_gami_id{};
};

}
}
}
