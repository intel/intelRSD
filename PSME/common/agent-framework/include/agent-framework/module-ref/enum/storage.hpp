/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @brief string enums for model classes, see enum_builder.hpp for more info
 * */

#ifndef AGENT_FRAMEWORK_MODEL_ENUM_STORAGE_HPP
#define AGENT_FRAMEWORK_MODEL_ENUM_STORAGE_HPP

#include "enum_builder.hpp"

namespace agent_framework {
namespace model {
namespace enums {

/*!
 * @brief ENUM LogicalDriveType for Logical Drive class member
 */
ENUM(LogicalDriveType, uint32_t, LVM, CEPH, RSTe, MDRAID);

/*!
 * @brief ENUM LogicalDriveMode for Logical Drive class member
 */
ENUM(LogicalDriveMode, uint32_t, JBOD, BlockDevice, LogicalDrive, PV, LV, LVG,
        OSD, Pool, RAID0Array, RAID1Array, RAID10Array, RAID5Array,
        RAID6Array, RAID50Array, RAID60Array);

/*!
 * @brief ENUM PhysicalDriveInterface for Physical Drive class member
 */
ENUM(PhysicalDriveInterface, uint32_t, PCIe, SAS, SATA);

/*!
* @brief ENUM PhysicalDriveInterface for Physical Drive class member
*/
ENUM(PhysicalDriveType, uint32_t, HDD, SSD, NVMe);

/*!
* @brief ENUM PhysicalDriveMode for Physical Drive class member
 */
ENUM(PhysicalDriveMode, uint32_t, JBOD, BlockDevice, LogicalDrive, PV, LV, LVG,
        OSD, Pool, RAID0Array, RAID1Array, RAID10Array, RAID5Array,
        RAID6Array, RAID50Array, RAID60Array);

}
}
}

#endif //AGENT_FRAMEWORK_MODEL_ENUM_STORAGE_HPP
