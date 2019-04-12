/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 * */

#include "agent-framework/module/chassis_components.hpp"
#include "agent-framework/module/common_components.hpp"

#include <ipmb/watcher/drawer_power_task.hpp>
#include <ipmb/command/drawer_power_response.hpp>
#include <ipmb/service.hpp>
#include <ipmb/gpio.hpp>

#include <ipmi/command/generic/get_sensor_reading.hpp>
#include <ipmi/command/generic/get_sensor_reading_factors.hpp>
#include <ipmi/manager/ipmitool/management_controller.hpp>

using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent::chassis;
using namespace agent::chassis::ipmb;
using namespace agent::chassis::ipmb::watcher;
using namespace ipmi;
using namespace ipmi::manager;

using agent_framework::module::ChassisComponents;
using agent_framework::module::ChassisComponents;

DrawerPowerTask::~DrawerPowerTask() {}

/*! Drawer power processing*/
class ProcessDrawerPower {
public:
    /*!
     * Executes Drawer power processing
     * @param[in] manager_keys Baldes' manager key
     */
    void execute(const std::vector<std::string>& manager_keys) {
        fill_sled_power(manager_keys);
    }

private:
    static constexpr const uint8_t SENSOR_HSC_INPUT_POWER = 0x29;

    void fill_sled_power(const std::vector<std::string>& manager_keys);

    uint16_t get_sled_power(ipmi::ManagementController& mc);

    uint8_t get_sled_power_sensor(ipmi::ManagementController& mc);
    uint16_t get_sled_power_sensor_multiplier(ipmi::ManagementController& mc,
                                              uint8_t reading_byte);
};

void DrawerPowerTask::execute() {
    try {
        auto drawer_manager_keys = CommonComponents::get_instance()->
            get_module_manager().get_keys("");
        auto blade_manager_keys = CommonComponents::get_instance()->
            get_module_manager().get_keys(drawer_manager_keys.front());

        ProcessDrawerPower ps{};
        ps.execute(blade_manager_keys);
    }
    catch (const std::exception& e) {
        log_debug(LOGUSR, "ProcessDrawerPowers - exception : " << e.what());
    }
}

uint16_t ProcessDrawerPower::get_sled_power(ipmi::ManagementController& mc) {
    uint8_t reading_byte = get_sled_power_sensor(mc);
    uint16_t multiplier = get_sled_power_sensor_multiplier(mc, reading_byte);
    log_debug(LOGUSR, "Reading byte  " << static_cast<std::uint32_t>(reading_byte) <<
                      " : "  << "multi " << static_cast<std::uint32_t>(multiplier));
    return static_cast<uint16_t>(reading_byte * multiplier);
}

uint8_t ProcessDrawerPower::get_sled_power_sensor(ipmi::ManagementController& mc) {

    command::generic::request::GetSensorReading ipmi_request;
    command::generic::response::GetSensorReading ipmi_response;

    ipmi_request.set_sensor_number(SENSOR_HSC_INPUT_POWER);
    mc.send(ipmi_request, ipmi_response);

    return ipmi_response.get_sensor_reading();
}

uint16_t ProcessDrawerPower::get_sled_power_sensor_multiplier(ipmi::ManagementController& mc,
                                                                     uint8_t reading_byte) {

    command::generic::request::GetSensorReadingFactors ipmi_request;
    command::generic::response::GetSensorReadingFactors ipmi_response;

    ipmi_request.set_sensor_number(SENSOR_HSC_INPUT_POWER);
    ipmi_request.set_reading_byte(reading_byte);
    mc.send(ipmi_request, ipmi_response);

    return ipmi_response.get_multiplier();
}

void ProcessDrawerPower::fill_sled_power(const std::vector<std::string>& manager_keys) {
    for (const auto& key: manager_keys) {
        auto manager = CommonComponents::get_instance()->
                get_module_manager().get_entry(key);

        if (manager.get_presence()) {
            const auto &connection_data = manager.get_connection_data();

            ipmitool::ManagementController mc;

            mc.set_ip(connection_data.get_ip_address());
            mc.set_port(connection_data.get_port());
            mc.set_username(connection_data.get_username());
            mc.set_password(connection_data.get_password());


            auto chassis_keys = CommonComponents::get_instance()->
                    get_chassis_manager().get_keys(manager.get_uuid());
            auto power_zone_keys = ChassisComponents::get_instance()->
                    get_power_zone_manager().get_keys(chassis_keys.front());

            try {
                auto power = get_sled_power(mc);
                auto power_zone = ChassisComponents::get_instance()->
                        get_power_zone_manager().get_entry_reference(power_zone_keys.front());

                power_zone->set_power_consumed_watts(power);

                log_debug(LOGUSR, "ProcessDrawerPower for " << connection_data.get_ip_address() <<
                                  ": "  << " sled_power: " << static_cast<uint32_t>(power));
            }
            catch (const std::runtime_error& e) {
                log_error(LOGUSR, "Cannot execute IPMI command: " << e.what());
            }
        }
    }
}
