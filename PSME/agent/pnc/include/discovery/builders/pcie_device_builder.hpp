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
 * @file pcie_device_builder.hpp
 *
 * @brief PcieDeviceBuilder interface
 * */

#pragma once

#include "discovery/builders/abstract_builder.hpp"
#include "agent-framework/module/common_components.hpp"
#include "sysfs/sysfs_dto.hpp"

namespace agent {
namespace pnc {
namespace discovery {
namespace builders {

/*!
 * @brief Builder used to create pcie device objects
 */
class PcieDeviceBuilder : public AbstractBuilder<agent_framework::model::PcieDevice> {

public:

    using ReturnType = PcieDeviceBuilder&;

    PcieDeviceBuilder() = default;
    ~PcieDeviceBuilder() = default;

    /*!
     * @brief Updates sysfs data of the device
     * @param[in] sysfs_device Device data read from sysfs
     * @return Pointer to the builder
     * */
    virtual ReturnType update_sysfs(const sysfs::SysfsDevice& sysfs_device);

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
