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

#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/validators/procedure_validator.hpp"
#include "agent-framework/module/constants/storage.hpp"
#include "agent-framework/module/model/attributes/attributes.hpp"
#include "agent-framework/module/model/attributes/oem.hpp"
#include "agent-framework/module/requests/validation/storage.hpp"
#include "agent-framework/module/requests/validation/json_check_type.hpp"
#include "agent-framework/module/storage_components.hpp"



using namespace agent_framework::exceptions;
using namespace agent_framework::model::attribute;

namespace agent_framework {
namespace model {
namespace requests {
namespace validation {

void StorageValidator::validate_set_volume_attributes(const Attributes& attributes) {
    static jsonrpc::ProcedureValidator validator{
        jsonrpc::PARAMS_BY_NAME,
        literals::Volume::CAPACITY_BYTES, VALID_OPTIONAL(VALID_NULLABLE(VALID_NUMERIC_RANGE(INT64, 0, INT64_MAX))),
        literals::Volume::BOOTABLE, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_BOOLEAN)),
        literals::Volume::INITIALIZATION, VALID_OPTIONAL(VALID_NULLABLE(VALID_ENUM(enums::VolumeInitializationType))),
        literals::Volume::OEM, VALID_OPTIONAL(VALID_JSON_OBJECT),
        nullptr
    };

    validator.validate(attributes.to_json());
    log_debug("storage-agent", "Request validation passed.");
}


void StorageValidator::validate_set_drive_attributes(const Attributes& attributes) {

    static jsonrpc::ProcedureValidator validator{
        jsonrpc::PARAMS_BY_NAME,
        literals::Drive::LATENCY_TRACKING_ENABLED, VALID_OPTIONAL(VALID_JSON_BOOLEAN),
        nullptr
    };

    validator.validate(attributes.to_json());
    log_debug("storage-agent", "Request validation passed.");
}

}
}
}
}
