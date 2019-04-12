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
 *
 *
 * @file ssdp_config_loader.hpp
 *
 * @brief Declares load_ssdp_config function.
 * */

#pragma once

#include "ssdp/ssdp_service_config.hpp"
#include "json-wrapper/json-wrapper.hpp"

namespace ssdp {

/*!
 * Creates SsdpServiceConfig from JSON object.
 * @param[in] config JSON configuration object.
 * @param[in] uuid Service uuid.
 * @return Instance of SsdpServiceConfig.
 */
SsdpServiceConfig load_ssdp_config(const json::Json& config, const std::string& uuid);

}
