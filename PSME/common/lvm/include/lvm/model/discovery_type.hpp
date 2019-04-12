/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file discovery_type.hpp
 * @brief Declaration of discovery type enum
 * */

#pragma once

namespace storage {
namespace lvm {

/*!
 * @enum Discovery type. Select components to discover in the system.
 *
 * @var VG - Volume Groups
 * @var VG_LV - Volume Groups and Logical Volumes
 * @var VG_PV - Volume Groups and Physical Volumes
 * @var VG_PV_LV - Volume Groups, Physical Volumes and Logical Volumes
 */
enum class DiscoveryType {
    VG,
    VG_LV,
    VG_PV,
    VG_PV_LV
};

}
}
