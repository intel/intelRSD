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

#include "psme/rest/validators/schemas/common.hpp"
#include "psme/rest/validators/schemas/zone.hpp"
#include "psme/rest/constants/constants.hpp"



using namespace psme::rest;
using namespace psme::rest::validators::schema;


const jsonrpc::ProcedureValidator& ZonePatchSchema::LinksSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Fabric::ENDPOINTS, VALID_ARRAY_OF(VALID_ATTRIBUTE(SimpleObjectSchema)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& ZonePatchSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Common::LINKS, VALID_OPTIONAL(VALID_ATTRIBUTE(LinksSchema)),
        nullptr
    };
    return procedure;
}
