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
 * @file delete_volume.hpp
 * @brief DeleteVolume request
 * */

#pragma once



#include "agent-framework/module/utils/uuid.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/validators/procedure_validator.hpp"
#include "agent-framework/module/constants/storage.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

/*! DeleteVolume request */
class DeleteVolume {
public:
    explicit DeleteVolume(const Uuid& volume);

    static std::string get_command() {
        return literals::Command::DELETE_VOLUME;
    }

    /*!
     * @brief Get the UUID of the volume to be deleted
     * @return Volume UUID
     * */
    const Uuid& get_volume() const {
        return m_volume;
    }

    /*!
     * @brief Transform request to JSON
     * @return Created JSON value
     */
    json::Json to_json() const;

    /*!
     * @brief Create DeleteVolume from JSON
     * @param[in] json The input argument
     * @return DeleteVolume
     */
    static DeleteVolume from_json(const json::Json& json);

    /*!
     * @brief Returns procedure scheme
     * @return ProcedureValidator scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_STRING,
            literals::Volume::VOLUME, VALID_UUID,
            nullptr
        };
        return procedure;
    }

private:
    Uuid m_volume{};
};

}
}
}
