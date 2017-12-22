/*!
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
 * */

#include "agent-framework/module/constants/storage.hpp"
#include "agent-framework/module/constants/regular_expressions.hpp"
#include "agent-framework/module/model/attributes/attributes.hpp"
#include "agent-framework/module/enum/storage.hpp"
#include "agent-framework/module/requests/validation/storage.hpp"
#include "agent-framework/validators/procedure_validator.hpp"

using namespace agent_framework::exceptions;
using namespace agent_framework::model::attribute;
using namespace agent_framework::model;

namespace agent_framework {
namespace model {
namespace requests {
namespace validation {


void StorageValidator::validate_set_iscsi_target_attributes(const Attributes& attributes) {
    jsonrpc::ProcedureValidator validator(
        jsonrpc::PARAMS_BY_NAME,
        literals::IscsiTarget::INITIATOR_IQN,
            VALID_OPTIONAL(VALID_NULLABLE(VALID_REGEX(literals::regex::RemoteTarget::INITIATOR_IQN))),
        literals::IscsiTarget::AUTHENTICATION_METHOD,
            VALID_OPTIONAL(VALID_NULLABLE(VALID_ENUM(enums::TargetAuthenticationMethod))),
        literals::IscsiTarget::CHAP_USERNAME,
            VALID_OPTIONAL(VALID_NULLABLE(VALID_REGEX(literals::regex::Common::NO_WHITESPACE_STRING))),
        literals::IscsiTarget::CHAP_SECRET,
            VALID_OPTIONAL(VALID_NULLABLE(VALID_REGEX(literals::regex::Common::NO_WHITESPACE_STRING))),
        literals::IscsiTarget::MUTUAL_CHAP_USERNAME,
            VALID_OPTIONAL(VALID_NULLABLE(VALID_REGEX(literals::regex::Common::NO_WHITESPACE_STRING))),
        literals::IscsiTarget::MUTUAL_CHAP_SECRET,
            VALID_OPTIONAL(VALID_NULLABLE(VALID_REGEX(literals::regex::Common::NO_WHITESPACE_STRING))),
        literals::IscsiTarget::OEM, VALID_OPTIONAL(VALID_JSON_OBJECT),
        nullptr
    );
    validator.validate(attributes.to_json());

    log_debug(GET_LOGGER("storage-agent"), "Request validation passed.");
}


void StorageValidator::validate_set_logical_drive_attributes(const Attributes& attributes) {
    jsonrpc::ProcedureValidator validator(
        jsonrpc::PARAMS_BY_NAME,
        literals::LogicalDrive::BOOTABLE, VALID_JSON_BOOLEAN,
        literals::IscsiTarget::OEM, VALID_OPTIONAL(VALID_JSON_OBJECT),
        nullptr
    );
    validator.validate(attributes.to_json());

    log_debug(GET_LOGGER("storage-agent"), "Request validation passed.");
}

}
}
}
}
