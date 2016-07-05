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
 * @file chassis_manager.hpp
 * @brief Chassis Manager Interface
 * */

#ifndef AGENT_FRAMEWORK_MODULE_CHASSIS_MANAGER_HPP
#define AGENT_FRAMEWORK_MODULE_CHASSIS_MANAGER_HPP

#include "agent-framework/generic/singleton.hpp"
#include "agent-framework/module-ref/managers/generic_manager.hpp"
#include "agent-framework/module-ref/managers/utils/manager_utils.hpp"
#include "agent-framework/module-ref/model/manager.hpp"
#include "agent-framework/module-ref/model/chassis.hpp"
#include "agent-framework/module-ref/model/psu.hpp"
#include "agent-framework/module-ref/model/fan.hpp"
#include "agent-framework/module-ref/model/thermal_zone.hpp"
#include "agent-framework/module-ref/model/power_zone.hpp"
#include "agent-framework/module-ref/model/authorization_certificate.hpp"

/*! Psme namespace */
namespace agent_framework {
namespace module {

using namespace agent_framework::model;

/*!
 * @brief Class for managing chassis components and subcomponents
 * */
class ChassisManager : public generic::Singleton<ChassisManager> {
public:
    using CertificateManager = GenericManager<AuthorizationCertificate>;
    using ModuleManager = GenericManager<model::Manager>;
    using ChassisModuleManager = GenericManager<model::Chassis>;
    using PsuManager = GenericManager<model::Psu>;
    using FanManager = GenericManager<model::Fan>;
    using PowerZoneManager = GenericManager<model::PowerZone>;
    using ThermalZoneManager = GenericManager<model::ThermalZone>;

    virtual ~ChassisManager();

    /*!
     * @brief Get certificate manager
     * @return certificate manager
     * */
    CertificateManager& certificates() {
        return m_certificate_manager;
    }

    /*!
    * @brief Get chassis manager
    * @return Instance of chassis manager
    * */
    ChassisModuleManager& get_chassis_manager() {
        return m_chassis_manager;
    }

    /*!
     * @brief Get manager for managers
     * @return manager for managers
     * */
    ModuleManager& get_module_manager() {
        return m_module_manager;
    }

    /*!
     * @brief Get manager for PSU
     * @return manager for PSU
     * */
    PsuManager& get_psu_manager() {
        return m_psu_manager;
    }

    /*!
     * @brief Get manager for Fan
     * @return manager for Fan
     * */
    FanManager& get_fan_manager() {
        return m_fan_manager;
    }

    /*!
     * @brief Get manager for PowerZone
     * @return manager for PowerZone
     * */
    PowerZoneManager& get_power_zone_manager() {
        return m_power_zone_manager;
    }

    /*!
     * @brief Get manager for ThermalZone
     * @return manager for ThermalZone
     * */
    ThermalZoneManager& get_thermal_zone_manager() {
        return m_thermal_zone_manager;
    }

private:
    ModuleManager m_module_manager{};
    CertificateManager m_certificate_manager{};
    ChassisModuleManager m_chassis_manager{};
    PsuManager m_psu_manager{};
    FanManager m_fan_manager{};
    PowerZoneManager m_power_zone_manager{};
    ThermalZoneManager m_thermal_zone_manager{};
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_CHASSIS_MANAGER_HPP */
