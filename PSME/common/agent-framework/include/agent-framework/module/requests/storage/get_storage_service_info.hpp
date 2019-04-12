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
 * @file requests/storage/get_storage_service_info.hpp
 * @brief Storage GetStorageServiceInfo request
 * */

#pragma once

#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/validators/procedure_validator.hpp"
#include "agent-framework/module/constants/storage.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace requests {

/*! GetStorageServiceInfo request */
class GetStorageServiceInfo {
public:
    explicit GetStorageServiceInfo(const std::string& service);

     static std::string get_command() {
        return literals::Command::GET_STORAGE_SERVICE_INFO;
    }

    /*!
     * @brief Get the UUID of the service
     * @return Service UUID
     * */
    std::string get_uuid() const {
        return m_service;
    }

    /*!
     * @brief Transform request to JSON
     *
     * @return Created JSON value
     */
    json::Json to_json() const;

    /*!
     * @brief Create GetStorageServiceInfo from JSON
     *
     * @param[in] json the input argument
     *
     * @return new GetStorageServiceInfo
     */
    static GetStorageServiceInfo from_json(const json::Json& json);

    /*!
     * @brief Returns procedure scheme
     * @return ProcedureValidator scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_STRING,
            literals::StorageService::SERVICE, jsonrpc::JSON_STRING,
            nullptr
        };
        return procedure;
    }

private:
    std::string m_service{};
};

}
}
}
