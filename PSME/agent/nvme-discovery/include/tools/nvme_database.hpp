/*!
 * @brief Definition of NvmeDatabase class.
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
 * @file nvme_database.hpp
 */

#pragma once

#include "tools/database.hpp"
#include "database/persistent_attributes.hpp"
#include "database/aggregate.hpp"
#include "agent-framework/module/utils/uuid.hpp"

#include <string>

namespace agent {
namespace nvme {
namespace tools {

class NvmeDatabase : public Database {
public:
    static const std::string ZONES;
    static const std::string ENDPOINTS;
    static const std::string VOLUME_ERASED;
    static const std::string CHASSIS_ASSET_TAG;
    static const std::string ROLE;
    static const std::string IPV4;
    static const std::string RDMA_PORT;
    static const std::string NQN;

    /*!
     * @brief Constructor
     *
     * @param uuid Uuid of a resource
     * @param name Name of the database
     */
    NvmeDatabase(const Uuid& uuid, const std::string& name);

    /*!
     * @brief Destructor
     */
    virtual ~NvmeDatabase();

    void put(const std::string& key, const std::string& value) override;

    std::string get(const std::string& key) const override;

    void del(const std::string& key) override;

    void append(const std::string& bucket, const std::string& value) override;

    std::vector<std::string> get_multiple_values(const std::string& bucket) const override;

    void remove() override;

private:
    database::Aggregate<database::PersistentAttributes> m_db;
};


}
}
}
