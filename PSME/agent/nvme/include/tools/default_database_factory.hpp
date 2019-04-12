/*!
 * @brief The default database factory implementation.
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
 * @file default_database_factory.hpp
 */

#pragma once

#include "tools/base_database_factory.hpp"

namespace agent {
namespace nvme {
namespace tools {

/*!
 * The default implementation of an NVME database factory.
 */
class DefaultDatabaseFactory final : public BaseDatabaseFactory {
public:

    /*!
     * @ brief Destructor.
     */
    virtual ~DefaultDatabaseFactory();

    /*!
     * @brief Creates an NVME database of a given type with a given uuid.
     *
     * @param db_type the type of the database to be created.
     * @param uuid the uuid associated with this database
     *
     * @return A handle to the requested database.
     */
    virtual Database::Ptr create_database(Database::Types db_type, const Uuid& uuid) override;
};

}  // namespace tools
}  // namespace nvme
}  // namespace agent
