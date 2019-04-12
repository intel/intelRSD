/*!
 * @brief Factory base class for nvme databases.
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
 * @file base_database_factory.hpp
 */

#pragma once


#include "tools/database.hpp"

namespace agent {
namespace nvme {
namespace tools {

/*!
 * @brief An interface for an NVME database factory.
 */
class BaseDatabaseFactory {
public:

    /*!
     * @brief Defaulted destructor.
     */
    virtual ~BaseDatabaseFactory() = default;

    /*!
     * @brief Creates a database of a given type with a given uuid.
     *
     * @param db_type the type of the database to be created.
     * @param uuid the uuid associated with this database
     *
     * @return A handle to the requested database.
     */
    virtual Database::Ptr create_database(Database::Types db_type, const Uuid& uuid) = 0;
};


}  // namespace tools
}  // namespace nvme
}  // namespace agent

