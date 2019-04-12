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
 * @file get_id_field.cpp
 * @brief Get ID Field for RMM.
 * */

#include "agent-framework/module/common_components.hpp"

#include <ipmb/utils.hpp>
#include <ipmb/ipmi_message.hpp>
#include <ipmb/command/get_id_field.hpp>

#include <logger/logger_factory.hpp>

using namespace agent::chassis::ipmb;
using namespace agent::chassis::ipmb::command;
using namespace agent_framework::module;

using agent_framework::module::CommonComponents;

namespace {

agent_framework::model::Chassis get_chassis() {
    auto drawer_manager_keys = get_manager<agent_framework::model::Manager>().get_keys("");
    if (drawer_manager_keys.empty()) {
        return agent_framework::model::Chassis{};
    }

    auto chassis_keys = get_manager<agent_framework::model::Chassis>().get_keys(drawer_manager_keys.front());
    return get_manager<agent_framework::model::Chassis>().get_entry(chassis_keys.front());
}

std::vector<uint8_t> uint32_as_msb_array(std::uint32_t number) {
    std::vector<uint8_t> array(4);
    array[0] = static_cast<uint8_t>(number >> 24);
    array[1] = static_cast<uint8_t>(number >> 16);
    array[2] = static_cast<uint8_t>(number >> 8);
    array[3] = static_cast<uint8_t>(number);
    return array;
}

}

void GetIdField::Response::add_data(IpmiMessage& msg) {
    auto data = msg.get_data();

    std::uint16_t offset = 0;
    data[offset++] = static_cast<uint8_t>(get_cc());
    data[offset++] = get_max_field_size();
    data[offset++] = get_curr_field_size();
    for (const auto& byte: m_field) {
        data[offset++] = byte;
    }
    msg.add_len(offset);
}

void  GetIdField::unpack(IpmiMessage& msg) {
    log_debug("ipmb", "Unpacking Get ID Field message.");

    msg.set_to_request();

    // Validate request length
    if (CMD_REQUEST_DATA_LENGTH > (msg.get_len() - IPMB_FRAME_HDR_WITH_DATA_CHCKSUM_LEN)) {
        log_error("ipmb", "Invalid request length.");
        m_response.set_cc(CompletionCode::CC_REQ_DATA_LENGTH_INVALID);
        return;
    }

    auto data = msg.get_data();

    if (data) {
        m_request.set_instance(*data);
    }
}

void GetIdField::pack(IpmiMessage& msg) {

    if (CompletionCode::CC_OK != m_response.get_cc()) {
        populate(msg);
        return;
    }

    switch (InstanceField(m_request.get_instance())) {
    case InstanceField::RESERVED:
        log_error("ipmb", "Can not read reserved value.");
        m_response.set_cc(CompletionCode::CC_PARAMETER_OUT_OF_RANGE);
        break;
    case InstanceField::RACKPUID:
        m_response.set_max_field_size(CMD_MAX_NUMERIC_FIELD_SIZE);
        m_response.set_field(get_rack_puid());
        break;
    case InstanceField::RACKBPID:
        m_response.set_max_field_size(CMD_MAX_NUMERIC_FIELD_SIZE);
        m_response.set_field({0,0,0,0});
        break;
    case InstanceField::TRAYRUID:
        m_response.set_max_field_size(CMD_MAX_NUMERIC_FIELD_SIZE);
        m_response.set_field(get_tray_ruid());
        break;
    case InstanceField::RACKID:
        m_response.set_max_field_size(CMD_MAX_STRING_FIELD_SIZE);
        m_response.set_field(get_rack_location_id());
        break;
    default:
        log_error("ipmb", "Unknown field instance.");
        m_response.set_cc(CompletionCode::CC_PARAMETER_OUT_OF_RANGE);
        break;
    }

    populate(msg);
}

std::vector<uint8_t> GetIdField::get_tray_ruid() const {
    const auto chassis = get_chassis();
    const auto location_offset = chassis.get_location_offset();
    log_debug("ipmb", "Drawer Chassis=" << chassis.get_uuid()
                        << " DrawerRUID=" << location_offset);
    return uint32_as_msb_array(location_offset);
}

std::vector<uint8_t> GetIdField::get_rack_puid() const {
    const auto chassis = get_chassis();
    std::uint32_t parent_id{0};
    try {
        parent_id = static_cast<std::uint32_t>(std::stoul(chassis.get_parent_id(), nullptr, 10));
    } catch (...) { /*not a number*/ }

    log_debug("ipmb", "Drawer Chassis=" << chassis.get_uuid() << " RackPUID=" << parent_id);
    return uint32_as_msb_array(parent_id);
}

std::vector<uint8_t> GetIdField::get_rack_location_id() const {
    const auto chassis = get_chassis();
    const auto& parent_id = chassis.get_parent_id();

    if (parent_id.has_value()) {
        log_debug("ipmb", "Drawer Chassis=" << chassis.get_uuid() << " Rack location id=" << parent_id.value());
        return { parent_id.value().begin(), parent_id.value().end() };
    }
    else {
        std::string empty_id{};
        return { empty_id.begin(), empty_id.end() };
    }
}

void GetIdField::populate(IpmiMessage& msg) {
    m_response.add_data(msg);
    msg.set_to_response();
}
