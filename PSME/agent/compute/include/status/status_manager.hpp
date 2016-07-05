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
 * @file status_manager.hpp
 * @brief Module software status implementation
*/

#ifndef AGENT_COMPUTE_STATUS_STATUS_MANAGER_HPP
#define AGENT_COMPUTE_STATUS_STATUS_MANAGER_HPP

#include "agent-framework/status/status_manager.hpp"
#include "agent-framework/status/module_status.hpp"
#include "status/module_hardware_status.hpp"
#include "status/module_software_status.hpp"

/*! Agent namespace */
namespace agent {
/*! Compute namespace */
namespace compute {
/*! Status namespace */
namespace status {

using agent_framework::status::ModuleStatus;

/*! Compute Status Manager implementation */
class StatusManager : public agent_framework::status::StatusManager {
public:
    /*!
     * Default constructor
     * */
    explicit StatusManager(const std::uint32_t slot, const std::string& ip);

    /*! Copy constructor */
    StatusManager(const StatusManager&) = default;

    /*! Assignment operator */
    StatusManager& operator=(const StatusManager&) = default;

    /*!
     * @brief Gets hardware status basing on GPIO presence.
     *
     * @return Module hardware status
     * */
    ModuleStatus::Status get_hw_status();

    /*!
     * @brief Gets software status basing on PING.
     *
     * @return Module software status
     * */
    ModuleStatus::Status get_sw_status();

    ~StatusManager();

private:
    std::uint32_t m_slot{};
    std::string m_ip_address{};
    ModuleSoftwareStatus m_sw_status{};
    ModuleHardwareStatus m_hw_status{};
};

}
}
}

#endif /* AGENT_COMPUTE_STATUS_STATUS_MANAGER_HPP */

