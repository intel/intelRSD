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
 * */

#pragma once
namespace psme {
namespace rest {
namespace constants {

/*!
 * @brief Constant literals for StorageService endpoint.
 */
namespace StorageService {
extern const char DRIVES[];
extern const char STORAGE_POOLS[];
extern const char VOLUMES[];
extern const char ENDPOINTS[];
extern const char HOSTING_SYSTEM[];
}

/*!
 * @brief Constant literals for Swordfish objects.
 */
namespace Swordfish {
extern const char PROVIDED_CAPACITY[];
extern const char CAPACITY[];
extern const char CAPACITY_SOURCES[];
extern const char CAPACITY_BYTES[];
extern const char PROVIDING_DRIVES[];
extern const char PROVIDING_POOLS[];
extern const char PROVIDING_VOLUMES[];
extern const char ALLOCATED_VOLUMES[];
extern const char ALLOCATED_POOLS[];
extern const char ACCESS_CAPABILITIES[];
extern const char REPLICA_INFOS[];
extern const char BLOCK_SIZE_BYTES[];
extern const char OPTIMUM_IO_SIZE_BYTES[];
extern const char BOOTABLE[];
extern const char ASSIGNED[];
extern const char VOLUME_INITIALIZE[];
extern const char VOLUME_INITIALIZE_ENDPOINT[];
extern const char INITIALIZE_TYPE[];
extern const char CAPACITY_USED_BYTES[];
extern const char IDENTIFIER[];
extern const char IS_THIN_PROVISIONED[];
}

namespace Data {
extern const char DATA[];
extern const char ALLOCATED_BYTES[];
extern const char CONSUMED_BYTES[];
extern const char GUARANTEED_BYTES[];
extern const char PROVISIONED_BYTES[];
}

namespace ReplicaInfo {
extern const char REPLICA[];
extern const char REPLICA_TYPE[];
extern const char REPLICA_ROLE[];
extern const char REPLICA_READ_ONLY_ACCESS[];
}

}
}
}
