/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 * @file storage.hpp
 * @brief Contains constants used by storage agents
 * */

#pragma once
namespace agent_framework {
namespace model {
namespace literals {

/*!
 * @brief Class consisting of literals for IscsiTarget model objects
 */
class IscsiTarget {
public:
    static constexpr const char TARGET[] = "target";
    static constexpr const char STATUS[] = "status";
    static constexpr const char INITIATOR_IQN[] = "initiatorIQN";
    static constexpr const char TARGET_ADDRESS[] = "targetAddress";
    static constexpr const char TARGET_PORT[] = "targetPort";
    static constexpr const char TARGET_IQN[] = "targetIQN";
    static constexpr const char TARGET_LUNS[] = "targetLUNs";
    static constexpr const char AUTHENTICATION_METHOD[] = "authenticationMethod";
    static constexpr const char CHAP_USERNAME[] = "chapUsername";
    static constexpr const char CHAP_SECRET[] = "chapSecret";
    static constexpr const char MUTUAL_CHAP_USERNAME[] = "mutualChapUsername";
    static constexpr const char MUTUAL_CHAP_SECRET[] = "mutualChapSecret";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for TargetLun model objects
 */
class TargetLun {
public:
    static constexpr const char LUN[] = "LUN";
    static constexpr const char LOGICAL_DRIVE[] = "logicalDrive";
};

/*!
 * @brief Class consisting of literals for StorageServices model objects
 */
class StorageService {
public:
    static constexpr const char SERVICE[] = "service";
    static constexpr const char STATUS[] = "status";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for StoragePool model objects
 */
class StoragePool {
public:
    static constexpr const char STORAGE_POOL[] = "storagePool";
    static constexpr const char STATUS[] = "status";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char OEM[] = "oem";
    static constexpr const char BLOCK_SIZE_BYTES[] = "blockSizeBytes";
    static constexpr const char IDENTIFIERS[] = "identifiers";
    static constexpr const char CAPACITY[] = "capacity";
    static constexpr const char CAPACITY_SOURCES[] = "capacitySources";
};

/*!
 * @brief Class consisting of literals for Volume model objects
 */
class Volume {
public:
    static constexpr const char VOLUME[] = "volume";
    static constexpr const char STATUS[] = "status";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char OEM[] = "oem";
    static constexpr const char VOLUME_TYPE[] = "volumeType";
    static constexpr const char ENCRYPTED[] = "encrypted";
    static constexpr const char ENCRYPTION_TYPES[] = "encryptionTypes";
    static constexpr const char IDENTIFIERS[] = "identifiers";
    static constexpr const char BLOCK_SIZE_BYTES[] = "blockSizeBytes";
    static constexpr const char OPTIMUM_IO_SIZE_BYTES[] = "optimumIoSizeBytes";
    static constexpr const char CAPACITY[] = "capacity";
    static constexpr const char CAPACITY_BYTES[] = "capacityBytes";
    static constexpr const char CAPACITY_SOURCES[] = "capacitySources";
    static constexpr const char BOOTABLE[] = "bootable";
    static constexpr const char INITIALIZATION[] = "initialization";
    static constexpr const char ERASED[] = "erased";
    static constexpr const char ERASE_ON_DETACH[] = "eraseOnDetach";
    static constexpr const char REPLICA_INFOS[] = "replicaInfos";
    static constexpr const char ACCESS_CAPABILITIES[] = "accessCapabilities";
};

/*!
 * @brief ReplicaInfo consisting of literals for replica information
 */
class ReplicaInfo {
public:
    static constexpr const char REPLICA_READ_ONLY_ACCESS[] = "replicaReadOnlyAccess";
    static constexpr const char REPLICA_TYPE[] = "replicaType";
    static constexpr const char REPLICA_ROLE[] = "replicaRole";
    static constexpr const char REPLICA[] = "replica";
};

}
}
}
