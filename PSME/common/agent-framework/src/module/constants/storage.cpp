/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

constexpr const char PhysicalDrive::DRIVE[];
constexpr const char PhysicalDrive::STATUS[];
constexpr const char PhysicalDrive::INTERFACE[];
constexpr const char PhysicalDrive::TYPE[];
constexpr const char PhysicalDrive::CAPACITY[];
constexpr const char PhysicalDrive::RPM[];
constexpr const char PhysicalDrive::FRU_INFO[];
constexpr const char PhysicalDrive::OEM[];

constexpr const char LogicalDrive::DRIVE[];
constexpr const char LogicalDrive::STATUS[];
constexpr const char LogicalDrive::TYPE[];
constexpr const char LogicalDrive::CAPACITY[];
constexpr const char LogicalDrive::SNAPSHOT[];
constexpr const char LogicalDrive::BOOTABLE[];
constexpr const char LogicalDrive::IMAGE[];
constexpr const char LogicalDrive::MASTER[];
constexpr const char LogicalDrive::MODE[];
constexpr const char LogicalDrive::PROTECTED[];
constexpr const char LogicalDrive::COLLECTIONS[];
constexpr const char LogicalDrive::DRIVES[];
constexpr const char LogicalDrive::OEM[];

constexpr const char IscsiTarget::TARGET[];
constexpr const char IscsiTarget::STATUS[];
constexpr const char IscsiTarget::INITIATOR_IQN[];
constexpr const char IscsiTarget::TARGET_ADDRESS[];
constexpr const char IscsiTarget::TARGET_PORT[];
constexpr const char IscsiTarget::TARGET_IQN[];
constexpr const char IscsiTarget::TARGET_LUNS[];
constexpr const char IscsiTarget::AUTHENTICATION_METHOD[];
constexpr const char IscsiTarget::CHAP_USERNAME[];
constexpr const char IscsiTarget::CHAP_SECRET[];
constexpr const char IscsiTarget::MUTUAL_CHAP_USERNAME[];
constexpr const char IscsiTarget::MUTUAL_CHAP_SECRET[];
constexpr const char IscsiTarget::OEM[];

constexpr const char TargetLun::LUN[];
constexpr const char TargetLun::LOGICAL_DRIVE[];

constexpr const char StorageService::SERVICE[];
constexpr const char StorageService::STATUS[];
constexpr const char StorageService::COLLECTIONS[];
constexpr const char StorageService::OEM[];

}
}
}
