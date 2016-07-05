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
 * @file generate_data.cpp
 * @brief Chassis generate stubbed data implementation
 * */

#include "generate_data.hpp"
#include "agent-framework/module-ref/chassis_manager.hpp"

using namespace agent_framework::module;
using namespace agent_framework::model;

using ChassisComponents = agent_framework::module::ChassisManager;

namespace {

Chassis make_chassis(const std::string& parent,
                            agent_framework::model::enums::ChassisType type) {
    Chassis chassis{parent};
    chassis.set_size(type);
    chassis.set_location_offset(1);
    chassis.set_parent_id(std::to_string(1));
    chassis.set_platform(agent_framework::model::enums::PlatformType::UNKNOWN);

    agent_framework::model::attribute::FruInfo fru_info{};
    fru_info.set_manufacturer("Company ltd.");
    fru_info.set_model_number("1.2.3.4");
    fru_info.set_part_number("12");
    fru_info.set_serial_number("1234567ABCDEF");
    chassis.set_fru_info(std::move(fru_info));

    chassis.set_sku("SKU");
    chassis.set_indicator_led(
        agent_framework::model::enums::IndicatorLed::Lit);
    chassis.set_asset_tag("Asset Tag");

    return chassis;
}

void make_manager_internals(Manager& manager) {

    manager.set_firmware_version("1.0.2.312");
    manager.set_ipv4_address("127.0.0.1");
    manager.set_status({
                               agent_framework::model::enums::State::Enabled,
                               agent_framework::model::enums::Health::OK
                       });
    manager.set_oem(agent_framework::model::attribute::Oem{});
    manager.add_network_service(attribute::NetworkService(enums::NetworkServiceName::IPMI, 0, false));
    manager.add_network_service(attribute::NetworkService(enums::NetworkServiceName::SSH, 0, false));
    manager.add_network_service(attribute::NetworkService(enums::NetworkServiceName::Telnet, 0, false));

    agent_framework::model::attribute::SerialConsole serial{};
    serial.set_bitrate(115200);
    serial.set_data_bits(8);
    serial.set_enabled(true);
    serial.set_flow_control(enums::SerialConsoleFlowControl::None);
    serial.set_parity(enums::SerialConsoleParity::None);
    serial.set_pin_out(enums::SerialConsolePinOut::Cisco);
    serial.set_stop_bits(1);
    serial.set_signal_type(enums::SerialConsoleSignalType::Rs232);
    manager.set_serial_console(std::move(serial));

    manager.add_collection(attribute::Collection(
            enums::CollectionName::Chassis,
            enums::CollectionType::Chassis,
            "slotMask"
    ));
    manager.add_collection(attribute::Collection(
            enums::CollectionName::Managers,
            enums::CollectionType::Managers,
            "slotMask"
    ));
}

Manager make_manager() {
    Manager manager{};
    make_manager_internals(manager);
    return manager;
}

Manager make_manager(const string& parent) {
    Manager manager{parent};
    make_manager_internals(manager);
    return manager;
}

void do_chassis(const std::string& parent,
                    agent_framework::model::enums::ChassisType type) {
    auto chassis = ::make_chassis(parent, type);
    log_debug(GET_LOGGER("rpc"), "Adding chassis:" << chassis.get_uuid());
    ChassisComponents::get_instance()->get_chassis_manager().add_entry(chassis);
}

void do_manager() {
    auto rack_manager = ::make_manager();
    log_debug(GET_LOGGER("rpc"), "Adding Rack manager:" << rack_manager.get_uuid());
    ChassisComponents::get_instance()->get_module_manager().add_entry(rack_manager);

    auto drawer_manager = ::make_manager(rack_manager.get_uuid());
    log_debug(GET_LOGGER("rpc"), "Adding Drawer manager:" << drawer_manager.get_uuid());
    ChassisComponents::get_instance()->get_module_manager().add_entry(drawer_manager);

    do_chassis(rack_manager.get_uuid(),
                        agent_framework::model::enums::ChassisType::Rack);
    do_chassis(drawer_manager.get_uuid(),
                        agent_framework::model::enums::ChassisType::Drawer);
}

}

void agent::chassis::generate_data() {
    do_manager();
}
