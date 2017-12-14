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
 * @file storage.hpp
 * @brief Contains constants used by storage agents
 * */

#pragma once
namespace agent_framework {
namespace model {
namespace literals {

/*!
 * @brief Class consisting of literals for PhysicalDrive model objects
 */
class PhysicalDrive {
public:
    static constexpr const char DRIVE[] = "drive";
    static constexpr const char STATUS[] = "status";
    static constexpr const char INTERFACE[] = "interface";
    static constexpr const char TYPE[] = "type";
    static constexpr const char CAPACITY[] = "capacityGB";
    static constexpr const char RPM[] = "rpm";
    static constexpr const char FRU_INFO[] = "fruInfo";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for LogicalDrive model objects
 */
class LogicalDrive {
public:
    static constexpr const char DRIVE[] = "drive";
    static constexpr const char STATUS[] = "status";
    static constexpr const char TYPE[] = "type";
    static constexpr const char CAPACITY[] = "capacityGB";
    static constexpr const char MODE[] = "mode";
    static constexpr const char MASTER[] = "master";
    static constexpr const char SNAPSHOT[] = "snapshot";
    static constexpr const char BOOTABLE[] = "bootable";
    static constexpr const char IMAGE[] = "image";
    static constexpr const char PROTECTED[] = "protected";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char DRIVES[] = "drives";
    static constexpr const char OEM[] = "oem";
};

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

}
}
}

