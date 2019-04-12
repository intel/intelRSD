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
 *
 * @file certificate.cpp
 * @brief Certificate
 * */

#include "agent-framework/module/chassis_components.hpp"
#include "agent-framework/module/common_components.hpp"

#include <ipmb/watcher/thermal_sensor_task.hpp>
#include <ipmb/command/thermal_sensor_response.hpp>
#include <ipmb/gpio.hpp>
#include <ipmb/service.hpp>

#include <ipmi/command/generic/get_device_id.hpp>
#include <ipmi/command/generic/get_sensor_reading.hpp>
#include <ipmi/command/generic/get_sensor_reading_factors.hpp>
#include <ipmi/command/sdv/get_fan_pwm.hpp>
#include <ipmi/manager/ipmitool/management_controller.hpp>



using namespace std;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent::chassis;
using namespace agent::chassis::ipmb;
using namespace agent::chassis::ipmb::watcher;
using namespace ipmi;
using namespace ipmi::command;

using agent_framework::module::ChassisComponents;
using agent_framework::module::CommonComponents;


ThermalSensorTask::~ThermalSensorTask() {}


/*! Drawer thermal sensor processing*/
class ProcessThermalSensors {
public:
    /*!
     * Executes Drawer thermal sensor processing
     * @param[in] manager_keys Blades' manager keys
     */
    void execute(const std::vector<string>& manager_keys) {
        fill_thermal_sensor_data(manager_keys);
    }


private:
    static constexpr const uint8_t SLED_TYPE_HSW = 0x00;
    static constexpr const uint8_t SLED_TYPE_BDX_DE = 0x01;

    static constexpr const uint8_t SENSOR_INLET_TEMP = 0x07;

    ThermalSensorIpmbResponse m_response{};


    void fill_thermal_sensor_data(const std::vector<string>& manager_keys);


    uint8_t get_sled_type(ipmi::ManagementController& mc);


    uint8_t get_desired_pwm(ipmi::ManagementController& mc);


    uint8_t get_inlet_temp(ipmi::ManagementController& mc);


    uint8_t get_inlet_temp_sensor(ipmi::ManagementController& mc);


    uint16_t get_inlet_temp_sensor_multiplier(ipmi::ManagementController& mc,
                                              uint8_t reading_byte);
};


void ThermalSensorTask::execute() {
    try {
        auto drawer_manager_keys = CommonComponents::get_instance()->
            get_module_manager().get_keys("");
        auto blade_manager_keys = CommonComponents::get_instance()->
            get_module_manager().get_keys(drawer_manager_keys.front());

        ProcessThermalSensors ps{};
        ps.execute(blade_manager_keys);
    }
    catch (const std::exception& e) {
        log_debug(LOGUSR, "ProcessThermalSensors - exception : " << e.what());
    }
}


uint8_t ProcessThermalSensors::get_sled_type(ipmi::ManagementController& mc) {
    ipmi::command::generic::request::GetDeviceId ipmi_request;
    ipmi::command::generic::response::GetDeviceId ipmi_response;

    mc.send(ipmi_request, ipmi_response);

    auto product_id = ipmi_response.get_product_id();
    log_debug(LOGUSR, "ProcessThermalSensors - GetDeviceId: product_id = " << static_cast<int>(product_id));
    if (ipmi::command::generic::ProductId::PRODUCT_ID_INTEL_XEON_BDC_R == product_id ||
        ipmi::command::generic::ProductId::PRODUCT_ID_INTEL_XEON_BDC_A == product_id) {
        return SLED_TYPE_HSW;
    }
    else if (ipmi::command::generic::ProductId::PRODUCT_ID_INTEL_BDX_DE_BDC_R == product_id) {
        return SLED_TYPE_BDX_DE;
    }
    else {
        throw std::runtime_error("Unknown sled type");
    }
}


uint8_t ProcessThermalSensors::get_desired_pwm(ipmi::ManagementController& mc) {

    sdv::request::GetFanPwm ipmi_request;
    sdv::response::GetFanPwm ipmi_response;

    mc.send(ipmi_request, ipmi_response);

    return ipmi_response.get_maximum_pwm();
}


uint8_t ProcessThermalSensors::get_inlet_temp(ipmi::ManagementController& mc) {
    uint8_t reading_byte = get_inlet_temp_sensor(mc);
    uint16_t multiplier = get_inlet_temp_sensor_multiplier(mc, reading_byte);
    return static_cast<uint8_t>(reading_byte * multiplier);
}


uint8_t ProcessThermalSensors::get_inlet_temp_sensor(ipmi::ManagementController& mc) {

    ipmi::command::generic::request::GetSensorReading ipmi_request;
    ipmi::command::generic::response::GetSensorReading ipmi_response;

    ipmi_request.set_sensor_number(SENSOR_INLET_TEMP);

    mc.send(ipmi_request, ipmi_response);

    return ipmi_response.get_sensor_reading();
}


uint16_t ProcessThermalSensors::get_inlet_temp_sensor_multiplier(ipmi::ManagementController& mc,
                                                                 uint8_t reading_byte) {
    ipmi::command::generic::request::GetSensorReadingFactors ipmi_request;
    ipmi::command::generic::response::GetSensorReadingFactors ipmi_response;

    ipmi_request.set_sensor_number(SENSOR_INLET_TEMP);
    ipmi_request.set_reading_byte(reading_byte);

    mc.send(ipmi_request, ipmi_response);

    return ipmi_response.get_multiplier();
}


void ProcessThermalSensors::fill_thermal_sensor_data(const std::vector<string>& manager_keys) {

    for (const auto& key: manager_keys) {

        auto manager = CommonComponents::get_instance()->
            get_module_manager().get_entry(key);

        if (manager.get_presence()) {
            const auto& connection_data = manager.get_connection_data();

            ipmi::manager::ipmitool::ManagementController mc;

            mc.set_ip(connection_data.get_ip_address());
            mc.set_port(connection_data.get_port());
            mc.set_username(connection_data.get_username());
            mc.set_password(connection_data.get_password());

            auto chassis_keys = CommonComponents::get_instance()->
                get_chassis_manager().get_keys(manager.get_uuid());
            auto thermal_zone_keys = ChassisComponents::get_instance()->
                get_thermal_zone_manager().get_keys(chassis_keys.front());
            auto temperature_sensor_keys = ChassisComponents::get_instance()->
                get_chassis_sensor_manager().get_keys(chassis_keys.front(), [](const ChassisSensor& sensor) {
                return sensor.get_reading_units() == enums::ReadingUnits::Celsius;
            });

            try {
                auto type = get_sled_type(mc);
                auto pwm = get_desired_pwm(mc);
                auto tmp = get_inlet_temp(mc);

                auto chassis = CommonComponents::get_instance()->
                    get_chassis_manager().get_entry_reference(chassis_keys.front());
                auto thermal_zone = ChassisComponents::get_instance()->
                    get_thermal_zone_manager().get_entry_reference(thermal_zone_keys.front());
                auto temperature_sensor = ChassisComponents::get_instance()->get_chassis_sensor_manager().get_entry_reference(
                    temperature_sensor_keys.front());

                chassis->set_ipmb_type(type);
                thermal_zone->set_desired_speed_pwm(pwm);
                temperature_sensor->set_reading(tmp);

                log_debug(LOGUSR, "ProcessThermalSensors for " << connection_data.get_ip_address() << ": "
                                                               << " sled_type: " << static_cast<uint32_t>(type)
                                                               << " desired_pwm: " << static_cast<uint32_t>(pwm)
                                                               << " inlet_temp: " << static_cast<uint32_t>(tmp));
            }
            catch (const std::runtime_error& e) {
                log_error(LOGUSR, "Cannot execute IPMI command: " << e.what());
            }
        }
    }
}
