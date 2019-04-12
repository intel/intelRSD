/*!
 * @briefThe default database factory implementation.
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
 * @file default_database_factory.cpp
 */

#include "tools/default_database_factory.hpp"
#include "tools/databases.hpp"

#include <exception>
#include <memory>

namespace agent {
namespace nvme {
namespace tools {

DefaultDatabaseFactory::~DefaultDatabaseFactory() {
}

Database::Ptr DefaultDatabaseFactory::create_database(Database::Types db_type, const Uuid& uuid) {
    Database::Ptr ret{};
    switch (db_type) {
        case Database::Types::CHASSIS:
            ret = std::make_shared<ChassisDatabase>(uuid);
            break;
        case Database::Types::ENDPOINT:
            ret = std::make_shared<EndpointDatabase>(uuid);
            break;
        case Database::Types::FABRIC:
            ret = std::make_shared<FabricDatabase>(uuid);
            break;
        case Database::Types::VOLUME:
            ret = std::make_shared<VolumeDatabase>(uuid);
            break;
        case Database::Types::ZONE:
            ret = std::make_shared<ZoneDatabase>(uuid);
            break;
        default:
            throw std::runtime_error("Unknown database type");
    }
    return ret;
}

}  // namespace tools
}  // namespace nvme
}  // namespace agent
