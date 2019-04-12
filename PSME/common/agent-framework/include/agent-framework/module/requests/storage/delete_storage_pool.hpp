/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file delete_storage_pool.hpp
 */

#include "agent-framework/module/utils/uuid.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/validators/procedure_validator.hpp"
#include "agent-framework/module/constants/storage.hpp"



namespace agent_framework {
namespace model {
namespace requests {

/*! DeleteStoragePool request */
class DeleteStoragePool {
public:
    explicit DeleteStoragePool(const Uuid& storage_pool);


    /*!
      * brief Get command name
      * @return Command name
      */
    static std::string get_command() {
        return literals::Command::DELETE_STORAGE_POOL;
    }


    /*!
     * @brief Get the UUID of the Storage Pool to be deleted
     * @return Storage Pool UUID
     * */
    const Uuid& get_storage_pool() const {
        return m_storage_pool;
    }


    /*!
     * @brief Transform request to JSON
     * @return Created JSON value
     */
    json::Json to_json() const;


    /*!
     * @brief Create DeleteStoragePool from JSON
     * @param[in] json The input argument
     * @return DeleteStoragePool
     */
    static DeleteStoragePool from_json(const json::Json& json);


    /*!
     * @brief Returns procedure scheme
     * @return ProcedureValidator scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_STRING,
            literals::StoragePool::STORAGE_POOL, VALID_UUID,
            nullptr
        };
        return procedure;
    }


private:
    Uuid m_storage_pool{};
};
}
}
}
