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
 * @file lvm_attribute.hpp
 * @brief All Lvm attributes
 * */

#pragma once
#include <cstdint>

namespace storage {
namespace lvm {

/*! Namespace contains all Lvm attributes */
namespace attribute {

/*! @brief Status of lvm */
static constexpr const char* HEALTH_OK_STATUS = "OK";
/*! @brief Status of lvm */
static constexpr const char* HEALTH_CRITICAL_STATUS = "Critical";
/*! @brief Lvm state */
static constexpr const char* STATE_ENABLED = "Enabled";
/*! @brief Lvm state */
static constexpr const char* STATE_DISABLED = "Disabled";
/*! @brief Lvm state */
static constexpr const char* STATE_OFFLINE = "Offline";

/*! @brief Default flag for lvm handle */
static constexpr const std::uint32_t FLAGS = 0;

/*! @brief Lvm attribute property for LV */
static constexpr const char* LV_ATTR_PROPERTY = "lv_attr";
/*! @brief Lvm attribute property for VG */
static constexpr const char* VG_ATTR_PROPERTY = "vg_attr";
/*! @brief Lvm attribute property for PV */
static constexpr const char* PV_ATTR_PROPERTY = "pv_attr";

/*! @brief Lvm attribute value for read-only permission */
static constexpr const char* READ_MODE = "r";
/*! @brief Lvm attribute value for write permission */
static constexpr const char* WRITE_MODE = "w";

/*! @brief Lvm attribute for checking read-write permission */
static constexpr const char WRITE_MODE_ATTR = 'w';
/*! @brief lv_attr bit for checking read-write permission */
static constexpr const short LV_RW_ATTR = 1;
/*! @brief vg_attr bit for checking read-write permission */
static constexpr const short VG_RW_ATTR = 0;

/*! @brief LV attribute value for active state */
static constexpr const char STATE_ACTIVE = 'a';
/*! @brief LV, VG attribute value which means they are missing PV */
static constexpr const char HEALTH_CRITICAL = 'p';
/*! @brief PV attribute value which means it misses PV segments */
static constexpr const char PV_STATUS_HEALTH_CRITICAL = 'm';
/*! @brief lv_attr bit for checking status */
static constexpr const short LV_STATUS_ATTR = 4;
/*! @brief lv_attr bit for checking health */
static constexpr const short LV_HEALTH_ATTR = 8;
/*! @brief vg_attr bit for checking health and status */
static constexpr const short VG_STATUS_HEALTH_ATTR = 3;
/*! @brief pv_attr bit for checking health and status */
static constexpr const short PV_STATUS_HEALTH_ATTR = 2;

/*! @brief LV attribute value for snapshot */
static constexpr const char LV_SNAPSHOT_TYPE = 's';
/*! @brief LV attribute value for virtual type */
static constexpr const char LV_VIRTUAL_TYPE = 'v';
/*! @brief lv_attr bit for checking type */
static constexpr const short LV_TYPE_ATTR = 0;

/*! @brief LV attribute value for bootable */
static constexpr const char* LV_BOOTABLE_ATTR = "bootable";

}
}
}
