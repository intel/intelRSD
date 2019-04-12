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
 * */

#include "agent-framework/module/constants/pnc.hpp"
#include "agent-framework/module/model/attributes/attributes.hpp"
#include "agent-framework/module/requests/validation/pnc.hpp"
#include "agent-framework/module/requests/validation/json_check_type.hpp"
#include "agent-framework/module/pnc_components.hpp"



using namespace agent_framework::exceptions;
using namespace agent_framework::model::attribute;

namespace agent_framework {
namespace model {
namespace requests {
namespace validation {

void PncValidator::validate_set_pcie_switch_attributes(const Attributes& attributes) {
    for (const auto& name : attributes.get_names()) {
        const auto& value = attributes.get_value(name);
        if (literals::Switch::ASSET_TAG == name) {
            check_nullable_string(value, name, "pnc-agent");
        }
        else if (literals::Switch::POWER_STATE == name) {
            check_enum<enums::ResetType>(value, name, "pnc-agent");
        }
        else if (literals::Switch::OEM == name) {
            Oem::from_json(value);
        }
        else {
            THROW(InvalidField, "pnc-agent", "Unrecognized attribute.", name, value);
        }
    }
    log_debug("pnc-agent", "Request validation passed.");
}


void PncValidator::validate_set_pcie_port_attributes(const Attributes& attributes) {
    const auto& attribute_names = attributes.get_names();
    for (const auto& name : attribute_names) {
        const auto& value = attributes.get_value(name);
        if (literals::Port::POWER_STATE == name) {
            check_enum<enums::ResetType>(value, name, "pnc-agent");
        }
        else if (literals::Port::OEM == name) {
            Oem::from_json(value);
        }
        else {
            THROW(InvalidField, "pnc-agent", "Unrecognized attribute.", name, value);
        }
    }
}

}
}
}
}
