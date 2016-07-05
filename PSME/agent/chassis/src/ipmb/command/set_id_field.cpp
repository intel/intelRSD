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
 * @file set_id_field.cpp
 * @brief Set ID Field for RMM.
 * */

#include "agent-framework/eventing/event_data.hpp"
#include "agent-framework/eventing/events_queue.hpp"

#include <ipmb/command/set_id_field.hpp>
#include <ipmb/ipmi_message.hpp>
#include <ipmb/utils.hpp>

#include <agent-framework/module-ref/chassis_manager.hpp>

#include <logger/logger_factory.hpp>

using namespace agent::chassis::ipmb;
using namespace agent::chassis::ipmb::command;
using namespace agent_framework::module;
using namespace agent_framework::eventing;

using ChassisComponents = agent_framework::module::ChassisManager;

void SetIDField::Response::add_data(IpmiMessage& msg) {
    auto data = msg.get_data();

    data[OFFSET_CC] = uint8_t(m_cc);
    data[OFFSET_MAX_FIELD_SIZE] = uint8_t(m_max_field_size);

    msg.add_len(get_len());
}

void  SetIDField::unpack(IpmiMessage& msg) {
    log_debug(GET_LOGGER("ipmb"), "Unpacking Set ID Field message.");

    msg.set_to_request();

    // Validate request length
    if (CMD_MIN_REQUEST_DATA_LENGTH > (msg.get_len() - IPMB_FRAME_HDR_WITH_DATA_CHCKSUM_LEN)) {
        log_error(GET_LOGGER("ipmb"), "Invalid request length.");
        m_response.set_cc(CompletionCode::CC_REQ_DATA_LENGTH_INVALID);
        return;
    }
    if (CMD_MAX_REQUEST_LENGTH < (msg.get_len() - IPMB_FRAME_HDR_WITH_DATA_CHCKSUM_LEN)) {
        log_error(GET_LOGGER("ipmb"), "Request is to long: " << unsigned(msg.get_len()));
        m_response.set_cc(CompletionCode::CC_REQ_DATA_FIELD_LENGTH_EXC);
        return;
    }

    auto data = msg.get_data();

    if (data) {
        m_request.set_type(*data);
        m_request.set_instance(*(data + 1));
        m_request.set_field_size(*(data + 2));

        uint8_t* field_data = data + 3;
        uint32_t id = 0;

        id = decode_id_field(field_data, m_request.get_field_size());
        m_request.set_id_field(id);

    }

}

void SetIDField::pack(IpmiMessage& msg) {

    log_debug(LOGUSR, "Packing Set ID Field message.");

    if (CompletionCode::CC_OK != m_response.get_cc()) {
        populate(msg);
        return;
    }

    // Verify type
    if (CMD_TYPE_DEFAULT != m_request.get_type()) {
        log_error(GET_LOGGER("ipmb"), "Unsapported type.");
        m_response.set_cc(CompletionCode::CC_PARAMETER_OUT_OF_RANGE);
        populate(msg);
        return;
    }
    // Verify Id Field size
    if (0 == m_request.get_field_size() || CMD_MAX_FIELD_SIZE < m_request.get_field_size()) {
        log_error(GET_LOGGER("ipmb"), "Can not write 0 bytes.");
        m_response.set_cc(CompletionCode::CC_PARAMETER_OUT_OF_RANGE);
        populate(msg);
        return;
    }

    m_response.set_max_field_size(CMD_MAX_FIELD_SIZE);
    // Write ID
    switch (InstanceField(m_request.get_instance())) {
    case InstanceField::RESERVED:
        log_error(GET_LOGGER("ipmb"), "Can not write to reserved field.");
        m_response.set_cc(CompletionCode::CC_PARAMETER_OUT_OF_RANGE);
        break;
    case InstanceField::RACKPUID:
        set_rack_puid(m_request.get_id_field());
        m_response.set_cc(CompletionCode::CC_OK);
        break;
    case InstanceField::RACKBPID:
        log_warning(GET_LOGGER("ipmb"), "Can not write Rack BPID read-only field.");
        m_response.set_cc(CompletionCode::CC_PARAMETER_READ_ONLY);
        break;
    case InstanceField::TRAYRUID:
        set_tray_ruid(m_request.get_id_field());
        m_response.set_cc(CompletionCode::CC_OK);
        break;
    default:
        log_error(GET_LOGGER("ipmb"), "Unknown field instance.");
        m_response.set_cc(CompletionCode::CC_PARAMETER_OUT_OF_RANGE);
        break;
    }

    populate(msg);
}

void SetIDField::set_tray_ruid(const uint32_t ruid) {
    auto drawer_manager_keys = ChassisComponents::get_instance()->get_module_manager().get_keys("");
    if (!drawer_manager_keys.size()) {
        return;
    }

    auto chassis_keys = ChassisComponents::get_instance()->
        get_chassis_manager().get_keys(drawer_manager_keys.front());

    auto chassis = ChassisComponents::get_instance()->
        get_chassis_manager().get_entry_reference(chassis_keys.front());

    chassis->set_location_offset(ruid);
    chassis->set_location_offset_size(m_request.get_field_size());

    EventData event_data{};
    event_data.set_component(chassis->get_uuid());
    event_data.set_parent(chassis->get_parent_uuid());
    event_data.set_type(agent_framework::model::enums::Component::Chassis);
    event_data.set_notification(agent_framework::eventing::Notification::Update);
    EventsQueue::get_instance()->push_back(event_data);

    log_debug(GET_LOGGER("ipmb"), "Set TrayRUID=" << unsigned(ruid)
                                    << " for Drawer Chassis=" << chassis->get_uuid());
}

void SetIDField::set_rack_puid(const uint32_t puid) {
    auto drawer_manager_keys = ChassisManager::get_instance()->
                                    get_module_manager().get_keys("");
    if (!drawer_manager_keys.size()) {
        return;
    }

    auto chassis_keys = ChassisComponents::get_instance()->
        get_chassis_manager().get_keys(drawer_manager_keys.front());

    auto chassis = ChassisManager::get_instance()->
        get_chassis_manager().get_entry_reference(chassis_keys.front());

    chassis->set_parent_id_as_uint(puid);
    chassis->set_parent_id_size(m_request.get_field_size());

    EventData event_data{};
    event_data.set_component(chassis->get_uuid());
    event_data.set_parent(chassis->get_parent_uuid());
    event_data.set_type(agent_framework::model::enums::Component::Chassis);
    event_data.set_notification(agent_framework::eventing::Notification::Update);
    EventsQueue::get_instance()->push_back(event_data);

    log_debug(GET_LOGGER("ipmb"), "Set RackPUID=" << unsigned(puid)
                                    << " for Rack Chassis=" << chassis->get_uuid());
}

uint32_t SetIDField::decode_id_field(uint8_t* data, uint8_t len) {
    uint32_t id = 0;
    if (data && len > 0 && len <= CMD_MAX_FIELD_SIZE) {
        for (uint8_t i = 0; i < len; i++) {
            id |= uint32_t(*(data + i) << ((len - i - 1) * 8));
        }
    }
    return id;
}

void SetIDField::populate(IpmiMessage& msg) {
    m_response.set_len(CMD_RESPONSE_DATA_LENGTH);
    m_response.add_data(msg);
    msg.set_to_response();
}
