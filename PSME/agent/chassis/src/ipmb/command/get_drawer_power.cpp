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
 * @file get_drawer_power.cpp
 * @brief GetDrawerPower command handler for RMM.
 * */
#include <ipmb/command/get_drawer_power.hpp>

#include "agent-framework/module/model/model_chassis.hpp"
#include "agent-framework/module/managers/utils/manager_utils.hpp"



using namespace agent::chassis::ipmb::command;

using agent_framework::module::get_manager;
using namespace agent_framework::model;

namespace {

/*!
 * Checks if the machine running the code is big endian without
 * resorting to compiler depended macros or architecture dependent code.
 *
 * @return true if the machine is big endian, false otherwise
 */
bool is_big_endian() {
    // LSB = 0x00, MSB = 0xff
    std::uint16_t test_value = 0xff << 8;
    // Points to LSB on little endian machines or MSB on big endian ones.
    // This command does not cause undefined behaviour as the dereferenced
    // value is located on a proper memory boundary.
    std::uint8_t* probe = reinterpret_cast<std::uint8_t*>(&test_value);
    // evaluates to false for little endian machines
    return *probe;
}


/*!
 * Return 2-byte drawer power in a proper IPMI defined order.
 *
 * @param power Drawer power
 * @return Drawer power in MSB order
 */
std::uint16_t drawer_power_ipmi_order(std::uint16_t power) {
    if (is_big_endian()) {
        return power;
    }
    else {
        return static_cast<uint16_t>(power << 8u | (power >> 8u));
    }
}

}


GetDrawerPower::~GetDrawerPower() {}


void GetDrawerPower::unpack(IpmiMessage& msg) {
    log_debug(LOGUSR, "Unpacking GetDrawerPower message.");
    msg.set_to_request();
}


void GetDrawerPower::pack(IpmiMessage& msg) {

    log_debug(LOGUSR, "Packing GetDrawerPower message.");

    DrawerPowerIpmbResponse response{};

    msg.set_to_response();

    make_response(response);

    // 1 byte bitmap present
    // + size of power data
    auto len = 1 + response.sled_count * sizeof(uint16_t);

    auto data = msg.get_data();
    data[OFFSET_CC] = uint8_t(CompletionCode::CC_OK);

    const auto src = reinterpret_cast<const uint8_t*>(&response);
    std::copy(src, src + len, data + OFFSET_DATA);

    // 1 byte completion code
    // + size of data
    msg.add_len(static_cast<std::uint16_t>(1 + len));

}


void GetDrawerPower::make_response(DrawerPowerIpmbResponse& response) {
    uint8_t sled_presence_bit_map = 0;
    uint8_t sled_presence_mask = 1;

    auto drawer_manager_keys = get_manager<Manager>().get_keys("");
    auto sled_manager_keys = get_manager<Manager>().get_keys(drawer_manager_keys.front());

    for (const auto& key: sled_manager_keys) {
        auto manager = get_manager<Manager>().get_entry(key);
        if (manager.get_presence()) {
            sled_presence_bit_map = uint8_t(sled_presence_bit_map | sled_presence_mask << (manager.get_slot() - 1));
            log_debug(LOGUSR, "Sled presence mask: " << std::to_string(static_cast<uint>(sled_presence_mask))
                                                     << " Sled presence bit map: "
                                                     << std::to_string(static_cast<uint>(sled_presence_bit_map)));
            auto sled_chassis_keys = get_manager<Chassis>().get_keys(manager.get_uuid());

            auto power_zone_keys = get_manager<PowerZone>().get_keys(sled_chassis_keys.front());

            auto power_zone = get_manager<PowerZone>().get_entry(power_zone_keys.front());

            auto power = power_zone.get_power_consumed_watts();
            response.sled_power[response.sled_count] = drawer_power_ipmi_order(
                uint16_t(power.has_value() ? power.value() : ~0x00));

            response.sled_count++;
            sled_presence_mask = 1;
        }
    }

    response.sled_presence_bitmap = sled_presence_bit_map;
}
