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
 *
 * @file psme.cpp
 * @brief Contains common constants used all over the model
 * */

#include "agent-framework/module/constants/psme.hpp"



namespace agent_framework {
namespace model {
namespace literals {

/*!
 * @brief Class consisting of literals for HeartBeat
 */
constexpr const char HeartBeat::GAMI_ID[];
constexpr const char HeartBeat::TIME_STAMP[];
constexpr const char HeartBeat::MIN_DELAY[];

/*!
 * @brief Class consisting of literals for registration
 */
constexpr const char Attach::GAMI_ID[];
constexpr const char Attach::VERSION[];
constexpr const char Attach::VENDOR[];
constexpr const char Attach::IPV4_ADDRESS[];
constexpr const char Attach::PORT[];
constexpr const char Attach::CAPABILITIES[];

/*!
 * @brief Class consisting of literals for eventing
 */
constexpr const char ComponentNotification::NOTIFICATIONS[];
constexpr const char ComponentNotification::GAMI_ID[];
constexpr const char ComponentNotification::COMPONENT[];
constexpr const char ComponentNotification::NOTIFICATION[];
constexpr const char ComponentNotification::TYPE[];
constexpr const char ComponentNotification::PARENT[];

}
}
}
