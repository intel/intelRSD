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
 * @file discoverer_mf3.hpp
 *
 * @brief DiscovererMf3 - MF3 discoverer implementation
 * */

#pragma once

#include "tools/toolset.hpp"
#include "sysfs/sysfs_dto.hpp"
#include "gas/global_address_space_registers.hpp"
#include "discovery/discoverer.hpp"

#include "discovery/builders/builder_factory.hpp"

namespace agent {
namespace pnc {
namespace discovery {

/*!
 * @brief Discoverer class
 * */
class DiscovererMf3 : public Discoverer {

public:

    /*!
     * @brief Default constructor.
     * @param[in] factory Builder factory to be used
     * */
    DiscovererMf3(builders::BuilderFactoryPtr factory): Discoverer(
        agent_framework::model::enums::PlatformType::MF3, factory) {}

    /* Enable copy*/
    DiscovererMf3(const DiscovererMf3&) = default;
    DiscovererMf3& operator=(const DiscovererMf3&) = default;

    /*!
     * @brief Default destructor.
     * */
    virtual ~DiscovererMf3();

    /*!
     * @brief Mf3 implementation of the full switch discovery
     * @param[in] fabric_uuid Uuid of the parent fabric
     * @param[in] t tools::Toolset used to perform discovery
     * @param[in] chassis_uuid Uuid of the chassis containing the switch
     * @param[in] sysfs_switch Data read from the sysfs
     * @return Discovered entity
     * */
    virtual agent_framework::model::Switch discover_switch(const std::string& fabric_uuid, const tools::Toolset& t,
        const std::string& chassis_uuid, const sysfs::SysfsSwitch& sysfs_switch) const override;

    /*!
     * Implementation of Chassis discovery
     * @param t tools::Toolset used to perform discovery
     * @param chassis Chassis instance to be updated
     * @return Discovered entity
     */
    virtual agent_framework::model::Chassis discover_chassis(
            const tools::Toolset& t, const agent_framework::model::Chassis& chassis) const override;
};

}
}
}
