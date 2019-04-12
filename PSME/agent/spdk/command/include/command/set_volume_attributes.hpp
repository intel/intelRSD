/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file set_volume_attributes.hpp
 */

#pragma once

#include "spdk_agent_commands.hpp"

namespace agent {
namespace spdk {
namespace command {

/*!
 * @brief Handles setVolumeAttributes command
 * @param[in,out] context Agent context
 * @param[in] uuid Uuid of the modified chassis
 * @param[in] attributes Request attributes
 * @param[out] response Reference to the response
 */
void set_volume_attributes(agent::spdk::SetComponentAttributes::ContextPtr context,
                           const Uuid& uuid,
                           const agent_framework::model::attribute::Attributes& attributes,
                           agent::spdk::SetComponentAttributes::Response& response);

 }
 }
 }
