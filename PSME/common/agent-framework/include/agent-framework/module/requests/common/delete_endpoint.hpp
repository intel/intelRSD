/*!
 * @brief Definition of DeleteEndpoint class.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file delete_endpoint.hpp
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

class DeleteEndpoint {
public:
    explicit DeleteEndpoint(const Uuid& endpoint);

    static std::string get_command() {
        return literals::Command::DELETE_ENDPOINT;
    }

    const Uuid& get_endpoint() const {
        return m_endpoint;
    }

    void set_endpoint(const Uuid& endpoint) {
        m_endpoint = endpoint;
    }

    json::Json to_json() const;

    static DeleteEndpoint from_json(const json::Json& json);

    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_OBJECT,
            literals::Endpoint::ENDPOINT, VALID_UUID,
            nullptr
        };
        return procedure;
    }

private:
    Uuid m_endpoint{};
};

}
}
}
