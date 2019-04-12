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
#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace responses {

class DeleteEndpoint {
public:
    static std::string get_command() {
        return literals::Command::DELETE_ENDPOINT;
    }

    json::Json to_json() const;

    static DeleteEndpoint from_json(const json::Json& json);
};

}
}
}
