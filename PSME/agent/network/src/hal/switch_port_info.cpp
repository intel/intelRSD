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
 * @file switch_port_info.cpp
 *
 * @brief Switch port parameters
 * */
#include "hal/switch_port_info.hpp"


using namespace agent::network::hal;
using namespace agent_framework::exceptions;


SwitchPortInfo::SwitchPortInfo(const std::string& port_identifier)
    : m_identifier(port_identifier) {}


SwitchPortInfo::~SwitchPortInfo() {}

SwitchPortInfo::PortAttributeType
SwitchPortInfo::get_port_attribute_type(const std::string& attr_name, const json::Json& value) {
    auto attr_iter = m_attribute_map.find(attr_name);
    if (attr_iter != m_attribute_map.end()) {
        return attr_iter->second;
    }

    THROW(UnsupportedField, "network-agent", "Unsupported port attribute.", attr_name, value);
}

void SwitchPortInfo::PortAttributeValue::set_value_from_json(
                                    const json::Json& attribute_value) {
    if (attribute_value.is_boolean()) {
        set(attribute_value.get<bool>());
    }
    else if (attribute_value.is_number()) {
        try {
            set(attribute_value.get<uint32_t>());
        }
        catch(...) {
            THROW(InvalidValue, "network-agent",
                  "Json attribute value is not a valid 32-bit value. "
                  "API expects 32-bit values.");
        }
    }
    else if (attribute_value.is_string()) {
        set(attribute_value.get<std::string>());
    }
}

void SwitchPortInfo::PortAttributeValue::set(bool value) {
    m_type = PortAttributeValue::BOOL;
    m_bool = value;
}

bool SwitchPortInfo::PortAttributeValue::get_bool() const {
    if (m_type != PortAttributeValue::BOOL) {
        THROW(NetworkError, "network-agent",
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
        THROW(NetworkError, "network-agent",
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
        THROW(NetworkError, "network",
            "Unexpected port attribute type (string is expected).");
    }
    return m_string;
}
