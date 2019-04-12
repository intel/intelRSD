/*!
 * @brief Implementation of DeleteDrive class.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file delete_drive.cpp
 */

#include "agent-framework/module/model/attributes/oem.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/responses/common/delete_drive.hpp"

#include "json-wrapper/json-wrapper.hpp"

using namespace agent_framework::model::responses;
using namespace agent_framework::model::literals;


json::Json DeleteDrive::to_json() const {
    return json::Json();
}

DeleteDrive DeleteDrive::from_json(const json::Json&) {
    return DeleteDrive();
}
