/*!
 * @copyright Copyright (c) 2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file processor.cpp
 * @brief Implementation of validation schema for Processor
 * */

#include "psme/rest/validators/schemas/processor.hpp"
#include "psme/rest/constants/constants.hpp"



using namespace psme::rest;
using namespace psme::rest::validators::schema;


const jsonrpc::ProcedureValidator& ProcessorPatchSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Common::OEM, VALID_OPTIONAL(VALID_ATTRIBUTE(OemSchema)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& ProcessorPatchSchema::OemSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Common::RACKSCALE, VALID_OPTIONAL(VALID_ATTRIBUTE(RackScaleSchema)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& ProcessorPatchSchema::OemSchema::RackScaleSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Processor::FPGA, VALID_OPTIONAL(VALID_ATTRIBUTE(FpgaSchema)),
        nullptr
    };
    return procedure;
}


const jsonrpc::ProcedureValidator& ProcessorPatchSchema::OemSchema::RackScaleSchema::FpgaSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::Fpga::ERASED, VALID_OPTIONAL(VALID_JSON_BOOLEAN),
        nullptr
    };
    return procedure;
}
