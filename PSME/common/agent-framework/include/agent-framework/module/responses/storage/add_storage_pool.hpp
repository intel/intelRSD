/*!
 * @brief Definition of AddStoragePool class.
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
 * @file add_storage_pool.hpp
 */

#pragma once



#include "agent-framework/module/model/attributes/oem.hpp"
#include "json-wrapper/json-wrapper.hpp"
#include "agent-framework/module/utils/uuid.hpp"



namespace agent_framework {
namespace model {
namespace responses {

/*! AddStoragePool response */
class AddStoragePool {
public:
    /*!
     * @brief explicit AddStoragePool response constructor
     */
    explicit AddStoragePool(const Uuid& storage_pool = Uuid{},
                            const attribute::Oem& oem = attribute::Oem{});


    /*!
     * @brief Get storage pool UUID.
     * @return storage pool UUID.
     * */
    const Uuid& get_storage_pool() const {
        return m_storage_pool;
    }


    /*!
     * @brief Set storage pool
     * @param[in] storage_pool UUID created storage pool UUID
     * */
    void set_storage_pool(const Uuid& storage_pool) {
        m_storage_pool = storage_pool;
    }


    /*!
     * @brief Transform response to Json
     * @return created Json value
     */
    json::Json to_json() const;


    /*!
     * @brief create AddStoragePool from Json
     * @param[in] json the input argument
     * @return new AddStoragePool
     */
    static AddStoragePool from_json(const json::Json& json);


private:
    Uuid m_storage_pool{};
    attribute::Oem m_oem{};
};

}
}
}
