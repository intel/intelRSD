/*!
 * @brief Storage AddStoragePool request
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file add_storage pool.hpp
 */

#pragma once



#include "agent-framework/module/model/model_storage.hpp"
#include "agent-framework/module/constants/command.hpp"
#include "agent-framework/module/constants/storage.hpp"
#include "agent-framework/module/utils/optional_field.hpp"
#include "agent-framework/validators/procedure_validator.hpp"
#include "agent-framework/module/constants/regular_expressions.hpp"

#include <vector>
#include <string>



namespace agent_framework {
namespace model {
namespace requests {

/*! AddStoragePool request */
class AddStoragePool {
public:

    static std::string get_command() {
        return literals::Command::ADD_STORAGE_POOL;
    }


    /*! @brief Constructor */
    explicit AddStoragePool(
        uint64_t block_size_bytes,
        const attribute::Array<attribute::CapacitySource>& capacity_sources,
        const attribute::Oem& oem = attribute::Oem{}
    );


    /*!
     * @brief Transform request to JSON
     * @return Created JSON value
     */
    json::Json to_json() const;


    /*!
     * @brief Create AddStoragePool from JSON
     * @param[in] json The input argument
     * @return New AddStoragePool
     */
    static AddStoragePool from_json(const json::Json& json);


    /*!
     * @brief Get block size of the storage pool.
     * @return Block size in bytes.
     */
    uint64_t get_block_size_bytes() const {
        return m_block_size_bytes;
    }


    /*!
     * @brief Get capacity sources collection of the storage pool.
     * @return Array of capacity sources.
     */
    const attribute::Array<attribute::CapacitySource>& get_capacity_sources() const {
        return m_capacity_sources;
    }


    /*!
     * @brief Get OEM specific data
     * @return StoragePool's OEM specific data
     * */
    const attribute::Oem& get_oem() const {
        return m_oem;
    }


    /*!
     * @brief Returns procedure scheme
     * @return ProcedureValidator scheme
     */
    static const jsonrpc::ProcedureValidator& get_procedure() {
        static const jsonrpc::ProcedureValidator procedure{
            get_command(),
            jsonrpc::PARAMS_BY_NAME,
            jsonrpc::JSON_STRING,
            literals::StoragePool::BLOCK_SIZE_BYTES, VALID_NUMERIC_TYPED(UINT64),
            literals::StoragePool::CAPACITY_SOURCES, VALID_ARRAY_OF(VALID_ATTRIBUTE(CapacitySourceProcedure)),
            literals::StoragePool::OEM, VALID_JSON_OBJECT,
            nullptr
        };
        return procedure;
    }


private:
    std::int64_t m_block_size_bytes{};
    attribute::Array<attribute::CapacitySource> m_capacity_sources{};
    attribute::Oem m_oem{};

    class CapacitySourceProcedure {
    public:
        static const jsonrpc::ProcedureValidator& get_procedure() {
            static jsonrpc::ProcedureValidator procedure{
                jsonrpc::PARAMS_BY_NAME,
                literals::CapacitySource::PROVIDING_DRIVES, VALID_ARRAY_OF(VALID_UUID),
                literals::Capacity::CONSUMED_BYTES, VALID_NULLABLE(VALID_NUMERIC_TYPED(UINT64)),
                literals::Capacity::ALLOCATED_BYTES, VALID_NULLABLE(VALID_NEVER), // Should throw when value is provided
                literals::Capacity::GUARANTEED_BYTES, VALID_NULLABLE(VALID_NEVER), // Should throw when value is provided
                literals::Capacity::PROVISIONED_BYTES, VALID_NULLABLE(VALID_NEVER), // Should throw when value is provided
                literals::CapacitySource::PROVIDING_POOLS, VALID_NULLABLE(VALID_ARRAY_SIZE_OF(VALID_UUID, 0, 0)),
                literals::CapacitySource::PROVIDING_VOLUMES, VALID_NULLABLE(VALID_ARRAY_SIZE_OF(VALID_UUID, 0, 0)),
                nullptr
            };
            return procedure;
        }
    };
};

}
}
}
