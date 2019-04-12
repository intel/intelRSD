/*!
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
 * @file set_id_field.cpp
 * @brief Set ID Field for RMM.
 * */

#include "agent-framework/eventing/events_queue.hpp"
#include "agent-framework/module/model/attributes/event_data.hpp"
#include "agent-framework/module/common_components.hpp"

#include <ipmb/utils.hpp>
#include <ipmb/ipmi_message.hpp>
#include <ipmb/command/set_id_field.hpp>

using namespace agent::chassis::ipmb;
using namespace agent::chassis::ipmb::command;
using namespace agent_framework::module;
using namespace agent_framework::eventing;

using agent_framework::module::CommonComponents;

void SetIdField::Response::add_data(IpmiMessage& msg) {
    auto data = msg.get_data();
    uint8_t offset = 0;
    data[offset++] = static_cast<uint8_t>(get_cc());
    data[offset++] = get_max_field_size();

    msg.add_len(offset);
}

void  SetIdField::unpack(IpmiMessage& msg) {
    log_debug("ipmb", "Unpacking Set ID Field message.");

    msg.set_to_request();

    // Validate request length
    auto data_len = (msg.get_len() - IPMB_FRAME_HDR_WITH_DATA_CHCKSUM_LEN);
    if (CMD_MIN_REQUEST_DATA_LENGTH > data_len) {
        log_error("ipmb", "Invalid request length.");
        m_response.set_cc(CompletionCode::CC_REQ_DATA_LENGTH_INVALID);
        return;
    }
    auto data = msg.get_data();
    if (data) {
        auto offset = 0;
        m_request.set_type(data[offset++]);
        m_request.set_instance(data[offset++]);
        m_request.set_field_size(data[offset++]);
        if (CMD_MAX_STRING_FIELD_SIZE < m_request.get_field_size()
            || data_len < m_request.get_field_size()) {
            log_error("ipmb", "Wrong field size: " << static_cast<unsigned>(m_request.get_field_size())
                                           << " req len: " << msg.get_len());
            m_response.set_cc(CompletionCode::CC_REQ_DATA_FIELD_LENGTH_EXC);
            return;
        }
        m_request.set_field({data + offset, data + offset + m_request.get_field_size()});
    }
}

void SetIdField::pack(IpmiMessage& msg) {

    log_debug(LOGUSR, "Packing Set ID Field message.");

    if (CompletionCode::CC_OK != m_response.get_cc()) {
        populate(msg);
        return;
    }

    // Verify type
    if (CMD_TYPE_DEFAULT != m_request.get_type()) {
        log_error("ipmb", "Unsupported type.");
        m_response.set_cc(CompletionCode::CC_PARAMETER_OUT_OF_RANGE);
        populate(msg);
        return;
    }
    // Verify Id Field size
    const auto instance_field = InstanceField(m_request.get_instance());
    const auto field_size = m_request.get_field_size();
    if (CMD_MAX_STRING_FIELD_SIZE < field_size ||
        ((instance_field == InstanceField::RACKBPID
         || instance_field == InstanceField::RACKPUID
         || instance_field == InstanceField::TRAYRUID)
         && CMD_MAX_NUMERIC_FIELD_SIZE != field_size)) {
        log_error("ipmb", "Invalid field size: " << field_size);
        m_response.set_cc(CompletionCode::CC_PARAMETER_OUT_OF_RANGE);
        populate(msg);
        return;
    }

    // Write ID
    switch (instance_field) {
    case InstanceField::RESERVED:
        log_error("ipmb", "Can not write to reserved field.");
        m_response.set_cc(CompletionCode::CC_PARAMETER_OUT_OF_RANGE);
        break;
    case InstanceField::RACKPUID:
        m_response.set_max_field_size(CMD_MAX_NUMERIC_FIELD_SIZE);
        set_rack_puid(decode_id_field(m_request.get_field().data(), m_request.get_field_size()));
        m_response.set_cc(CompletionCode::CC_OK);
        break;
    case InstanceField::RACKBPID:
        log_warning("ipmb", "Can not write Rack BPID read-only field.");
        m_response.set_cc(CompletionCode::CC_PARAMETER_READ_ONLY);
        break;
    case InstanceField::TRAYRUID:
        m_response.set_max_field_size(CMD_MAX_NUMERIC_FIELD_SIZE);
        set_tray_ruid(decode_id_field(m_request.get_field().data(), m_request.get_field_size()));
        m_response.set_cc(CompletionCode::CC_OK);
        break;
    case InstanceField::RACKID:
        m_response.set_max_field_size(CMD_MAX_STRING_FIELD_SIZE);
        set_location_id({reinterpret_cast<const char*>(m_request.get_field().data()), m_request.get_field_size()});
        m_response.set_cc(CompletionCode::CC_OK);
        break;
    default:
        log_error("ipmb", "Unknown field instance.");
        m_response.set_cc(CompletionCode::CC_PARAMETER_OUT_OF_RANGE);
        break;
    }

    populate(msg);
}

void SetIdField::set_tray_ruid(const uint32_t ruid) {
    auto drawer_manager_keys = CommonComponents::get_instance()->get_module_manager().get_keys("");
    if (!drawer_manager_keys.size()) {
        return;
    }

    auto chassis_keys = CommonComponents::get_instance()->
        get_chassis_manager().get_keys(drawer_manager_keys.front());

    auto chassis = CommonComponents::get_instance()->
        get_chassis_manager().get_entry_reference(chassis_keys.front());

    chassis->set_location_offset(ruid);

    agent_framework::model::attribute::EventData event_data{};
    event_data.set_component(chassis->get_uuid());
    event_data.set_parent(chassis->get_parent_uuid());
    event_data.set_type(agent_framework::model::enums::Component::Chassis);
    event_data.set_notification(agent_framework::model::enums::Notification::Update);
    EventsQueue::get_instance()->push_back(event_data);

    log_debug("ipmb", "Set TrayRUID=" << unsigned(ruid)
                                    << " for Drawer Chassis=" << chassis->get_uuid());
}

void SetIdField::set_rack_puid(const uint32_t puid) {
    auto drawer_manager_keys = CommonComponents::get_instance()->
                                    get_module_manager().get_keys("");
    if (!drawer_manager_keys.size()) {
        return;
    }

    auto chassis_keys = CommonComponents::get_instance()->
        get_chassis_manager().get_keys(drawer_manager_keys.front());

    auto chassis = CommonComponents::get_instance()->
        get_chassis_manager().get_entry_reference(chassis_keys.front());

    chassis->set_parent_id(std::to_string(puid));

    agent_framework::model::attribute::EventData event_data{};
    event_data.set_component(chassis->get_uuid());
    event_data.set_parent(chassis->get_parent_uuid());
    event_data.set_type(agent_framework::model::enums::Component::Chassis);
    event_data.set_notification(agent_framework::model::enums::Notification::Update);
    EventsQueue::get_instance()->push_back(event_data);

    log_debug("ipmb", "Set RackPUID=" << unsigned(puid)
                                    << " for Rack Chassis=" << chassis->get_uuid());
}

void SetIdField::set_location_id(const std::string& location_id) {
    auto drawer_manager_keys = CommonComponents::get_instance()->
                                    get_module_manager().get_keys("");
    if (!drawer_manager_keys.size()) {
        return;
    }

    auto chassis_keys = CommonComponents::get_instance()->
        get_chassis_manager().get_keys(drawer_manager_keys.front());

    auto chassis = CommonComponents::get_instance()->
        get_chassis_manager().get_entry_reference(chassis_keys.front());

    chassis->set_parent_id(location_id);

    agent_framework::model::attribute::EventData event_data{};
    event_data.set_component(chassis->get_uuid());
    event_data.set_parent(chassis->get_parent_uuid());
    event_data.set_type(agent_framework::model::enums::Component::Chassis);
    event_data.set_notification(agent_framework::model::enums::Notification::Update);
    EventsQueue::get_instance()->push_back(event_data);

    log_debug("ipmb", "Set RackLocationId=" << location_id
                                    << " for Rack Chassis=" << chassis->get_uuid());
}

uint32_t SetIdField::decode_id_field(const uint8_t* data, uint8_t len) {
    uint32_t id = 0;
    if (data && len > 0 && len <= CMD_MAX_NUMERIC_FIELD_SIZE) {
        for (uint8_t i = 0; i < len; i++) {
            id |= uint32_t(*(data + i) << ((len - i - 1) * 8));
        }
    }
    return id;
}

void SetIdField::populate(IpmiMessage& msg) {
    m_response.add_data(msg);
    msg.set_to_response();
}
