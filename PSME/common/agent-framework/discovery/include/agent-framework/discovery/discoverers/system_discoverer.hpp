/*!
 * @brief System discoverer interface.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file system_discoverer.hpp
 */

#pragma once



#include "agent-framework/module/model/system.hpp"
#include "sysfs/abstract_sysfs_interface.hpp"



namespace agent_framework {
namespace discovery {

class SystemDiscoverer {
public:

    /*!
     * @brief Constructor
     * @param sysfs Pointer to abstract sysfs interface required to read system GUID
     */
    explicit SystemDiscoverer(std::shared_ptr<sysfs::AbstractSysfsInterface> sysfs) : m_sysfs(sysfs) {}


    /*! Virtual destructor */
    virtual ~SystemDiscoverer() {}


    /*!
     * @brief Perform discovery of system object.
     *
     * @param context Storage agent context pointer.
     * @param parent_uuid Parent UUID.
     * @param chassis_uuid UUID of chassis where system is located.
     *
     * @return System object filled with discovered data.
     * */
    virtual model::System discover(const Uuid& parent_uuid, const Uuid& chassis_uuid);


protected:
    std::shared_ptr<sysfs::AbstractSysfsInterface> m_sysfs{};

};

}
}
