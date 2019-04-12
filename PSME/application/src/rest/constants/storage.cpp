
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

#include "psme/rest/constants/storage.hpp"

namespace psme {
namespace rest {
namespace constants {

namespace StorageService {
const char DRIVES[] = "Drives";
const char STORAGE_POOLS[] = "StoragePools";
const char VOLUMES[] = "Volumes";
const char ENDPOINTS[] = "Endpoints";
const char HOSTING_SYSTEM[] = "HostingSystem";
}

namespace Swordfish {
const char PROVIDED_CAPACITY[] = "ProvidedCapacity";
const char CAPACITY[] = "Capacity";
const char CAPACITY_SOURCES[] = "CapacitySources";
const char CAPACITY_BYTES[] = "CapacityBytes";
const char PROVIDING_DRIVES[] = "ProvidingDrives";
const char PROVIDING_POOLS[] = "ProvidingPools";
const char PROVIDING_VOLUMES[] = "ProvidingVolumes";
const char ALLOCATED_VOLUMES[] = "AllocatedVolumes";
const char ALLOCATED_POOLS[] = "AllocatedPools";
const char ACCESS_CAPABILITIES[] = "AccessCapabilities";
const char REPLICA_INFOS[] = "ReplicaInfos";
const char BLOCK_SIZE_BYTES[] = "BlockSizeBytes";
const char OPTIMUM_IO_SIZE_BYTES[] = "OptimumIOSizeBytes";
const char BOOTABLE[] = "Bootable";
const char ASSIGNED[] = "Assigned";
const char VOLUME_INITIALIZE[] = "#Volume.Initialize";
const char VOLUME_INITIALIZE_ENDPOINT[] = "Volume.Initialize";
const char INITIALIZE_TYPE[] = "InitializeType";
const char CAPACITY_USED_BYTES[] = "CapacityUsedBytes";
const char IDENTIFIER[] = "Identifier";
const char IS_THIN_PROVISIONED[] = "IsThinProvisioned";
}

namespace Data {
const char DATA[] = "Data";
const char ALLOCATED_BYTES[] = "AllocatedBytes";
const char CONSUMED_BYTES[] = "ConsumedBytes";
const char GUARANTEED_BYTES[] = "GuaranteedBytes";
const char PROVISIONED_BYTES[] = "ProvisionedBytes";
}

namespace ReplicaInfo {
const char REPLICA[] = "Replica";
const char REPLICA_TYPE[] = "ReplicaType";
const char REPLICA_ROLE[] = "ReplicaRole";
const char REPLICA_READ_ONLY_ACCESS[] = "ReplicaReadOnlyAccess";
}

}
}
}
