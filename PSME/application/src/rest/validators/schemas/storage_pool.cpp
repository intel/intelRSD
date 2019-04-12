/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file storage_pool.cpp
 * */

#include "agent-framework/module/constants/regular_expressions.hpp"
#include "psme/rest/validators/schemas/storage_pool.hpp"
#include "psme/rest/validators/schemas/common.hpp"
#include "psme/rest/constants/constants.hpp"

#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/enum/storage.hpp"



using namespace psme::rest;
using namespace psme::rest::validators::schema;
using namespace agent_framework::model;


const jsonrpc::ProcedureValidator& StoragePoolPostSchema::CapacitySourceSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Swordfish::PROVIDING_DRIVES, VALID_ARRAY_SIZE_OF(VALID_ATTRIBUTE(SimpleObjectSchema), 1, UINT64_MAX),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& StoragePoolPostSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Swordfish::BLOCK_SIZE_BYTES, VALID_NUMERIC_TYPED(UINT64),
        constants::Swordfish::CAPACITY_SOURCES, VALID_ARRAY_SIZE_OF(VALID_ATTRIBUTE(CapacitySourceSchema), 1, 1),
        nullptr
    };
    return procedure;
}

