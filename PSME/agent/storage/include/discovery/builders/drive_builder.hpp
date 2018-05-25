/*!
 * @brief Drive builder class interface.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Filesystem}
 * @file drive_builder.hpp
 */

#pragma once



#include "agent-framework/module/model/drive.hpp"
#include "sysfs/sysfs_api.hpp"


namespace agent {
namespace storage {
namespace discovery {

class DriveBuilder final {
public:

    /*!
     * @brief Build default drive object.
     * @param[in] parent_uuid UUID of the parent component.
     * @return Default drive object.
     */
    static agent_framework::model::Drive build_default(const Uuid& parent_uuid);

    /*!
     * @brief Updates drive model with sysfs data.
     * @param drive Drive to be updated.
     * @param block_device Sysfs block device data.
     */
    static void update_with_sysfs(agent_framework::model::Drive& drive,
                                  const agent::storage::SysfsApi::HardDrive& block_device);


    /*!
     * @brief Add drive's system path identifier from sysfs data.
     * @param[in,out] drive The drive to be updated.
     * @param[in] block_device Sysfs block device data.
     * */
    static void add_identifier_with_system_path(agent_framework::model::Drive& drive,
                                                const agent::storage::SysfsApi::HardDrive& block_device);


    /*!
     * @brief Add drive model with UUID identifier.
     * @param[in,out] drive The drive to be updated.
     * */
    static void add_identifier_with_uuid(agent_framework::model::Drive& drive);


    /*!
     * @brief Updates drive model with UUID identifier.
     *
     * @param[in,out] drive The drive to be updated.
     *
     * @warning Should be called after stabilization process.
     * */
    static void update_identifier_with_uuid(agent_framework::model::Drive& drive);
};

}
}
}

