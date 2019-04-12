/*!
 * @brief Definition of all property keys used by pnc agent.
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
 * @file database_keys.hpp
 */

#pragma once



#include "agent-framework/module/model/model_common.hpp"



namespace agent {
namespace pnc {
namespace tools {
namespace db_keys {

extern const char DEVICE[];
extern const char DEVICE_PORT[];
extern const char ENDPOINTS[];
extern const char ZONES[];
extern const char ENDPOINT_ROLE[];
extern const char PORT[];

template<typename T>
constexpr const char DEVICE_ERASED_PROPERTY[] = "";

template<>
constexpr const char DEVICE_ERASED_PROPERTY<agent_framework::model::Drive>[] = "drive_erased";

template<>
constexpr const char DEVICE_ERASED_PROPERTY<agent_framework::model::Processor>[] = "processor_erased";

template<typename T>
constexpr const char DEVICES_PROPERTY[] = "";

template<>
constexpr const char DEVICES_PROPERTY<agent_framework::model::Drive>[] = "drives";

template<>
constexpr const char DEVICES_PROPERTY<agent_framework::model::Processor>[] = "processors";

template<typename T>
constexpr const char RESOURCE_NAME[] = "";

template<>
constexpr const char RESOURCE_NAME<agent_framework::model::Drive>[] = "drive";

template<>
constexpr const char RESOURCE_NAME<agent_framework::model::Processor>[] = "processor";

template<>
constexpr const char RESOURCE_NAME<agent_framework::model::System>[] = "system";

template<>
constexpr const char RESOURCE_NAME<agent_framework::model::Chassis>[] = "chassis";

} // namespace db_keys
} // namespace tools
} // namespace pnc
} // namespace agent