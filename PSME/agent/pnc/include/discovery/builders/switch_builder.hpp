/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file switch_builder.hpp
 *
 * @brief SwitchBuilder interface
 * */

#pragma once

#include "discovery/builders/abstract_builder.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "sysfs/sysfs_dto.hpp"
#include "seeprom.hpp"

namespace agent {
namespace pnc {
namespace discovery {
namespace builders {

/*!
 * @brief Builder used to create switch objects
 */
class SwitchBuilder : public AbstractBuilder<agent_framework::model::Switch> {

public:

    using ReturnType = SwitchBuilder&;

    SwitchBuilder() = default;
    ~SwitchBuilder() = default;

    /*!
     * @brief Updates sysfs data of the switch
     * @param[in] sysfs_switch Switch data read from sysfs
     * @return Pointer to the builder
     * */
    virtual ReturnType update_sysfs(const sysfs::SysfsSwitch& sysfs_switch);

    /*!
     * @brief Updates seeprom data of the switch
     * @param[in] seeprom Read seeprom data
     * @return Pointer to the builder
     * */
    virtual ReturnType update_seeprom(const Seeprom& seeprom);

    /*!
     * @brief Updates links to other resources
     * @param[in] chassis_uuid Chassis uuid
     * @return Pointer to the builder
     * */
    virtual ReturnType update_links(const std::string& chassis_uuid);

protected:
    /*!
     * @brief Performs construction of the default object
     * */
    virtual void build_default() override;
};

}
}
}
}
