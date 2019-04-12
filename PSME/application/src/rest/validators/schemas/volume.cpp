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

#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/constants/regular_expressions.hpp"
#include "psme/rest/validators/schemas/volume.hpp"
#include "psme/rest/validators/schemas/common.hpp"
#include "psme/rest/constants/constants.hpp"

#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/enum/storage.hpp"



using namespace psme::rest;
using namespace psme::rest::validators::schema;
using namespace agent_framework::model;


const jsonrpc::ProcedureValidator& VolumePostSchema::CapacitySourceSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Swordfish::PROVIDING_POOLS, VALID_OPTIONAL(VALID_NULLABLE(VALID_ARRAY_SIZE_OF(VALID_ATTRIBUTE(SimpleObjectSchema), 0, 1))),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& VolumePostSchema::ReplicaInfoSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::ReplicaInfo::REPLICA_TYPE, VALID_NULLABLE(VALID_ENUM(enums::ReplicaType)),
        constants::ReplicaInfo::REPLICA, VALID_NULLABLE(VALID_ATTRIBUTE(SimpleObjectSchema)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& VolumePostSchema::OemSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Common::RACKSCALE, VALID_OPTIONAL(VALID_ATTRIBUTE(RackScaleSchema)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& VolumePostSchema::OemSchema::RackScaleSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Swordfish::BOOTABLE, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_BOOLEAN)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& VolumePostSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Common::IDENTIFIERS, VALID_OPTIONAL(VALID_ARRAY_OF(VALID_ATTRIBUTE(IdentifierSchema))),
        constants::Swordfish::CAPACITY_BYTES, VALID_NUMERIC_RANGE(INT64, 0, INT64_MAX),
        constants::Swordfish::CAPACITY_SOURCES, VALID_OPTIONAL(VALID_NULLABLE(VALID_ARRAY_SIZE_OF(VALID_ATTRIBUTE(CapacitySourceSchema), 0, 1))),
        constants::Swordfish::ACCESS_CAPABILITIES, VALID_OPTIONAL(VALID_NULLABLE(VALID_ARRAY_OF(VALID_ENUM(enums::AccessCapability)))),
        constants::Swordfish::REPLICA_INFOS, VALID_OPTIONAL(VALID_NULLABLE(VALID_ARRAY_SIZE_OF(VALID_ATTRIBUTE(ReplicaInfoSchema), 0, 1))),
        constants::Common::OEM, VALID_OPTIONAL(VALID_ATTRIBUTE(OemSchema)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& VolumePatchSchema::OemSchema::RackScaleSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Swordfish::BOOTABLE, VALID_OPTIONAL(VALID_NULLABLE(VALID_JSON_BOOLEAN)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& VolumePatchSchema::OemSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Common::RACKSCALE, VALID_OPTIONAL(VALID_ATTRIBUTE(RackScaleSchema)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& VolumePatchSchema::CapacitySchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Data::DATA, VALID_OPTIONAL(VALID_ATTRIBUTE(DataSchema)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& VolumePatchSchema::CapacitySchema::DataSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Data::ALLOCATED_BYTES, VALID_OPTIONAL(VALID_NUMERIC_RANGE(INT64, 0, INT64_MAX)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& VolumePatchSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Swordfish::CAPACITY, VALID_OPTIONAL(VALID_ATTRIBUTE(CapacitySchema)),
        constants::Common::OEM, VALID_OPTIONAL(VALID_ATTRIBUTE(OemSchema)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& VolumeInitializePostSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Swordfish::INITIALIZE_TYPE, VALID_OPTIONAL(VALID_ENUM(enums::VolumeInitializationType)),
        nullptr
    };
    return procedure;
}
