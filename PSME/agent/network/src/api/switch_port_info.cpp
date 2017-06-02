/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file switch_port_info.cpp
 *
 * @brief Switch port parameters
 * */
#include "api/switch_port_info.hpp"

#include "agent-framework/exceptions/exception.hpp"

using namespace agent::network::api;
using namespace agent_framework::exceptions;

SwitchPortInfo::SwitchPortInfo(uint8_t switch_id, uint8_t port_index)
    : m_switch_id(switch_id), m_index(port_index) {}

SwitchPortInfo::PortAttributeType
SwitchPortInfo::get_port_attribute_type(const std::string& attr_name, const Json::Value& value) {
    auto attr_iter = m_attribute_map.find(attr_name);
    if (attr_iter != m_attribute_map.end()) {
        return attr_iter->second;
    }

    THROW(UnsupportedField, "network-agent", "Unsupported port attribute.", attr_name, value);
}

void SwitchPortInfo::PortAttributeValue::set_value_from_json(
                                    const Json::Value& attribute_value) {
    if (attribute_value.isBool()) {
        set(attribute_value.asBool());
    }
    else if (attribute_value.isNumeric()) {
        try {
            set(attribute_value.asUInt());
        }
        catch(...) {
            THROW(InvalidValue, "network-agent",
                  "Json attribute value is not a valid 32-bit value. "
                  "FM10000 API expects 32-bit values.");
        }
    }
    else if (attribute_value.isString()) {
        set(attribute_value.asString());
    }
}

void SwitchPortInfo::PortAttributeValue::set(bool value) {
    m_type = PortAttributeValue::BOOL;
    m_bool = value;
}

bool SwitchPortInfo::PortAttributeValue::get_bool() const {
    if (m_type != PortAttributeValue::BOOL) {
        THROW(Fm10000Error, "network-agent",
            "Unexpected port attribute type (boolean is expected).");
    }
    return m_bool;
}

void SwitchPortInfo::PortAttributeValue::set(uint32_t value) {
    m_type = PortAttributeValue::NUMBER;
    m_number = value;
}

uint32_t SwitchPortInfo::PortAttributeValue::get_number() const {
    if (m_type != PortAttributeValue::NUMBER) {
        THROW(Fm10000Error, "network-agent",
            "Unexpected port attribute type (number is expected).");
    }
    return m_number;
}

void SwitchPortInfo::PortAttributeValue::set(const std::string& value) {
    m_type = STRING;
    m_string = value;
}

const std::string& SwitchPortInfo::PortAttributeValue::get_string() const {
    if (m_type != PortAttributeValue::STRING) {
        THROW(Fm10000Error, "network",
            "Unexpected port attribute type (string is expected).");
    }
    return m_string;
}

SwitchPortInfo::~SwitchPortInfo() {}
