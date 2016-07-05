/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @section DESCRIPTION
 *
 * */

#include "agent-framework/module/fru_info.hpp"

using namespace agent_framework::generic;

FruInfo::~FruInfo(){}

Json::Value FruInfo::to_json() const {

    Json::Value json;

    json["manufacturer"] = get_manufacturer();
    json["partNumber"] = get_part_number();
    json["serialNumber"] = get_serial_number();
    json["modelNumber"] = get_model_number();

    return json;
}
