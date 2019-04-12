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
 *
 * @file log_helpers.hpp
 * */

#pragma once



#include "agent-framework/module/model/resource.hpp"



namespace agent_framework {

/*!
 * @brief Logs that persistent UUID was successfully generated.
 *
 * @param[in] resource_name Name of the resource
 * @param[in] temporary_uuid Resource temporary UUID
 * @param[in] persistent_uuid Resource persistent UUID
 * */
void log_persistent_uuid_generated(const std::string& resource_name,
                                   const Uuid& temporary_uuid,
                                   const Uuid& persistent_uuid);


/*!
 * @brief Logs that persistent UUID cannot be generated.
 *
 * @param[in] resource_name Name of the resource
 * @param[in] temporary_uuid Resource temporary UUID
 * */
void log_key_value_missing(const std::string& resource_name,
                           const Uuid& temporary_uuid);


/*!
 * @brief Logs that persistent UUID cannot be generated together with an exception message.
 *
 * @param[in] resource_name Name of the resource
 * @param[in] temporary_uuid Resource temporary UUID
 * @param[in] e Exception thrown during key generation attempt
 * */
void log_key_value_missing(const std::string& resource_name,
                           const Uuid& temporary_uuid,
                           const std::exception& e);

}
