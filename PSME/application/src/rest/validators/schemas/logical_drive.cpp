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

#include "psme/rest/validators/schemas/logical_drive.hpp"
#include "psme/rest/validators/schemas/common.hpp"
#include "psme/rest/constants/constants.hpp"

#include "agent-framework/module/enum/storage.hpp"



using namespace psme::rest;
using namespace psme::rest::validators::schema;
using namespace agent_framework::model;


const jsonrpc::ProcedureValidator& LogicalDrivePostSchema::LinksSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::LogicalDrive::LOGICAL_DRIVES, VALID_ARRAY_SIZE_OF(VALID_ATTRIBUTE(SimpleObjectSchema), 1, 1),
        constants::LogicalDrive::MASTER_DRIVE, VALID_ATTRIBUTE(SimpleObjectSchema),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& LogicalDrivePostSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Common::NAME, VALID_JSON_STRING,
        constants::LogicalDrive::TYPE, VALID_ENUM(enums::LogicalDriveType),
        constants::LogicalDrive::MODE, VALID_ENUM(enums::LogicalDriveMode),
        constants::LogicalDrive::PROTECTED, VALID_JSON_BOOLEAN,
        constants::LogicalDrive::CAPACITY, VALID_NUMERIC_EQGT(DOUBLE, 0.),
        constants::LogicalDrive::IMAGE, VALID_OPTIONAL(VALID_JSON_STRING),
        constants::LogicalDrive::BOOTABLE, VALID_JSON_BOOLEAN,
        constants::LogicalDrive::SNAPSHOT, VALID_JSON_BOOLEAN,
        constants::Common::LINKS, VALID_ATTRIBUTE(LinksSchema),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& LogicalDrivePatchSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::LogicalDrive::BOOTABLE, VALID_JSON_BOOLEAN,
        nullptr
    };
    return procedure;
}

