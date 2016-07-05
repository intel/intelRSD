/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file get_id_field.cpp
 * @brief Get ID Field for RMM.
 * */

#include <ipmb/command/get_id_field.hpp>
#include <ipmb/ipmi_message.hpp>
#include <ipmb/utils.hpp>

#include <agent-framework/module-ref/chassis_manager.hpp>

#include <logger/logger_factory.hpp>

using namespace agent::chassis::ipmb;
using namespace agent::chassis::ipmb::command;
using namespace agent_framework::module;

using ChassisComponents = agent_framework::module::ChassisManager;

void GetIDField::Response::add_data(IpmiMessage& msg) {
    auto data = msg.get_data();

    data[OFFSET_CC] = uint8_t(m_cc);
    data[OFFSET_MAX_FIELD_SIZE] = uint8_t(m_max_field_size);
    data[OFFSET_CURR_FIELD_SIZE] = uint8_t(m_curr_field_size);

    auto data_ptr = data + OFFSET_ID_FIELD_0;
    for (std::uint8_t i = 0u; i < m_curr_field_size; ++i) {
        data_ptr[i] =
            GET_BYTE(m_id_field,
                    static_cast<std::uint8_t>(m_curr_field_size - 1 - i));
    }

    msg.add_len(static_cast<std::uint16_t>(m_curr_field_size + get_len()));
}

void  GetIDField::unpack(IpmiMessage& msg) {
    log_debug(GET_LOGGER("ipmb"), "Unpacking Get ID Field message.");

    msg.set_to_request();

    // Validate request length
    if (CMD_REQUEST_DATA_LENGTH > (msg.get_len() - IPMB_FRAME_HDR_WITH_DATA_CHCKSUM_LEN)) {
        log_error(GET_LOGGER("ipmb"), "Invalid request length.");
        m_response.set_cc(CompletionCode::CC_REQ_DATA_LENGTH_INVALID);
        return;
    }

    auto data = msg.get_data();

    if (data) {
        m_request.set_instance(*data);
    }
}

void GetIDField::pack(IpmiMessage& msg) {

    if (CompletionCode::CC_OK != m_response.get_cc()) {
        populate(msg);
        return;
    }

    m_response.set_max_field_size(CMD_MAX_FIELD_SIZE);

    switch (InstanceField(m_request.get_instance())) {
    case InstanceField::RESERVED:
        log_error(GET_LOGGER("ipmb"), "Can not read reserved value.");
        m_response.set_cc(CompletionCode::CC_PARAMETER_OUT_OF_RANGE);
        break;
    case InstanceField::RACKPUID:
        m_response.set_id_field(get_rack_puid());
        break;
    case InstanceField::RACKBPID:
        m_response.set_id_field(0);
        break;
    case InstanceField::TRAYRUID:
        m_response.set_id_field(get_tray_ruid());
        break;
    default:
        log_error(GET_LOGGER("ipmb"), "Unknown field instance.");
        m_response.set_cc(CompletionCode::CC_PARAMETER_OUT_OF_RANGE);
        break;
    }

    populate(msg);
}

uint32_t GetIDField::get_tray_ruid() {
    auto drawer_manager_keys = ChassisComponents::get_instance()->get_module_manager().get_keys("");
    if (!drawer_manager_keys.size()) {
        return 0;
    }

    auto chassis_keys = ChassisComponents::get_instance()->
        get_chassis_manager().get_keys(drawer_manager_keys.front());

    auto chassis = ChassisComponents::get_instance()->
        get_chassis_manager().get_entry_reference(chassis_keys.front());

    m_response.set_curr_field_size(chassis->get_location_offset_size());

    log_debug(GET_LOGGER("ipmb"), "Get Drawer Chassis=" << chassis->get_uuid()
                        << " DrawerRUID=" << chassis->get_location_offset());

    return chassis->get_location_offset();
}

uint32_t GetIDField::get_rack_puid() {
    auto drawer_manager_keys = ChassisManager::get_instance()->
                                    get_module_manager().get_keys("");
    if (!drawer_manager_keys.size()) {
        return 0;
    }

    auto chassis_keys = ChassisComponents::get_instance()->
        get_chassis_manager().get_keys(drawer_manager_keys.front());

    auto chassis = ChassisManager::get_instance()->
        get_chassis_manager().get_entry_reference(chassis_keys.front());

    m_response.set_curr_field_size(chassis->get_parent_id_size());

    log_debug(GET_LOGGER("ipmb"), "Get Rack Chassis=" << chassis->get_uuid()
                        << " RackPUID=" << chassis->get_parent_id());
    return chassis->get_parent_id_as_uint();
}

void GetIDField::populate(IpmiMessage& msg) {
    m_response.set_len(CMD_RESPONSE_DATA_LENGTH);
    m_response.add_data(msg);
    msg.set_to_response();
}
