/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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

#include "agent-framework/module/constants/rmm.hpp"
#include "agent-framework/module/model/attributes/attributes.hpp"
#include "agent-framework/module/requests/validation/rmm.hpp"
#include "agent-framework/module/requests/validation/json_check_type.hpp"
#include "agent-framework/module/chassis_components.hpp"
#include "agent-framework/validators/procedure_validator.hpp"


using namespace agent_framework::exceptions;
using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

namespace agent_framework {
namespace model {
namespace requests {
namespace validation {

void RmmValidator::validate_set_thermal_zone_attributes(const Attributes& attributes) {
    static jsonrpc::ProcedureValidator validator(
        jsonrpc::PARAMS_BY_NAME,
        literals::ThermalZone::DESIRED_SPEED_PWM, VALID_OPTIONAL(VALID_NUMERIC_RANGE(UINT32, 0u, 100u)),
        literals::ThermalZone::OEM, VALID_OPTIONAL(VALID_JSON_OBJECT),
        nullptr
    );

    validator.validate(attributes.to_json());
    log_debug("rmm-agent", "Request validation passed.");
}

void RmmValidator::validate_set_psu_attributes(const Attributes& attributes) {
    for (const auto& name : attributes.get_names()) {
        const auto& value = attributes.get_value(name);
        if (literals::Psu::REQUESTED_STATE == name) {
            check_enum<enums::State>(value, name, "rmm-agent");
        }
        else if (literals::Psu::OEM == name) {
            Oem::from_json(value);
        }
        else {
            THROW(InvalidField, "rmm-agent", "Unrecognized attribute.", name, value);
        }
    }
    log_debug("rmm-agent", "Request validation passed.");
}

}
}
}
}
