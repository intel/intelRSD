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
 *
 *
 * @file storage.cpp
 * @brief Contains constants used by storage agents
 * */

#include "agent-framework/module/constants/storage.hpp"

namespace agent_framework {
namespace model {
namespace literals {

constexpr const char StorageService::SERVICE[];
constexpr const char StorageService::STATUS[];
constexpr const char StorageService::COLLECTIONS[];
constexpr const char StorageService::OEM[];

constexpr const char StoragePool::STORAGE_POOL[];
constexpr const char StoragePool::NAME[];
constexpr const char StoragePool::DESCRIPTION[];
constexpr const char StoragePool::STATUS[];
constexpr const char StoragePool::COLLECTIONS[];
constexpr const char StoragePool::OEM[];
constexpr const char StoragePool::BLOCK_SIZE_BYTES[];
constexpr const char StoragePool::IDENTIFIERS[];
constexpr const char StoragePool::CAPACITY[];
constexpr const char StoragePool::CAPACITY_SOURCES[];

constexpr const char Volume::VOLUME[];
constexpr const char Volume::STATUS[];
constexpr const char Volume::NAME[];
constexpr const char Volume::DESCRIPTION[];
constexpr const char Volume::COLLECTIONS[];
constexpr const char Volume::OEM[];
constexpr const char Volume::BLOCK_SIZE_BYTES[];
constexpr const char Volume::CAPACITY[];
constexpr const char Volume::CAPACITY_BYTES[];
constexpr const char Volume::CAPACITY_SOURCES[];
constexpr const char Volume::VOLUME_TYPE[];
constexpr const char Volume::ENCRYPTED[];
constexpr const char Volume::ENCRYPTION_TYPES[];
constexpr const char Volume::IDENTIFIERS[];
constexpr const char Volume::OPTIMUM_IO_SIZE_BYTES[];
constexpr const char Volume::BOOTABLE[];
constexpr const char Volume::INITIALIZATION[];
constexpr const char Volume::REPLICA_INFOS[];
constexpr const char Volume::ACCESS_CAPABILITIES[];

constexpr const char ReplicaInfo::REPLICA_READ_ONLY_ACCESS[];
constexpr const char ReplicaInfo::REPLICA_TYPE[];
constexpr const char ReplicaInfo::REPLICA_ROLE[];
constexpr const char ReplicaInfo::REPLICA[];

}
}
}
