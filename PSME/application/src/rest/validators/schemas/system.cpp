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

#include "psme/rest/validators/schemas/system.hpp"
#include "psme/rest/constants/constants.hpp"
#include "agent-framework/module/enum/compute.hpp"

using namespace psme::rest;
using namespace psme::rest::validators::schema;
using namespace agent_framework::model;

const jsonrpc::ProcedureValidator& SystemPatchSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
            jsonrpc::PARAMS_BY_NAME,
            constants::System::BOOT, VALID_OPTIONAL(VALID_ATTRIBUTE(BootSchema)),
            constants::Common::OEM, VALID_OPTIONAL(VALID_ATTRIBUTE(OemSchema)),
            constants::Common::ASSET_TAG, VALID_OPTIONAL(VALID_JSON_STRING),
            nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& SystemPatchSchema::BootSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
            jsonrpc::PARAMS_BY_NAME,
            constants::System::BOOT_SOURCE_OVERRIDE_ENABLED, VALID_OPTIONAL(VALID_ENUM(enums::BootOverride)),
            constants::System::BOOT_SOURCE_OVERRIDE_TARGET, VALID_OPTIONAL(VALID_ENUM(enums::BootOverrideTarget)),
            constants::System::BOOT_SOURCE_OVERRIDE_MODE, VALID_OPTIONAL(VALID_ENUM(enums::BootOverrideMode)),
            nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& SystemPatchSchema::OemSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
            jsonrpc::PARAMS_BY_NAME,
            constants::Common::RACKSCALE, VALID_OPTIONAL(VALID_ATTRIBUTE(RackScaleSchema)),
            nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& SystemPatchSchema::OemSchema::RackScaleSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
            jsonrpc::PARAMS_BY_NAME,
            constants::System::USER_MODE_ENABLED, VALID_OPTIONAL(VALID_JSON_BOOLEAN),
            constants::Common::PERFORMANCE_CONFIGURATION, VALID_OPTIONAL(VALID_ATTRIBUTE(PerformanceConfigurationSchema)),
            nullptr
    };
    return procedure;
}

const jsonrpc::ProcedureValidator& SystemPatchSchema::OemSchema::RackScaleSchema::PerformanceConfigurationSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
            jsonrpc::PARAMS_BY_NAME,
            constants::Common::CURRENT_CONFIGURATION_ID, VALID_OPTIONAL(VALID_JSON_INTEGER),
            nullptr
    };
    return procedure;
}
