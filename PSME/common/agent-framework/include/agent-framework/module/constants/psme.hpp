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
 * @file psme.hpp
 * @brief Contains constants used by PSME
 * */

#pragma once

namespace agent_framework {
namespace model {
namespace literals {

/*!
 * @brief Class consisting of literals for HeartBeat
 */
class HeartBeat {
public:
    static constexpr const char GAMI_ID[] = "gamiId";
    static constexpr const char TIME_STAMP[] = "timeStamp";
    static constexpr const char MIN_DELAY[] = "minDelay";
};

/*!
 * @brief Class consisting of literals for registration
 */
class Attach {
public:
    static constexpr const char GAMI_ID[] = "gamiId";
    static constexpr const char VERSION[] = "version";
    static constexpr const char VENDOR[] = "vendor";
    static constexpr const char IPV4_ADDRESS[] = "ipv4address";
    static constexpr const char PORT[] = "port";
    static constexpr const char CAPABILITIES[] = "capabilities";
};

/*!
 * @brief Class consisting of literals for eventing
 */
class ComponentNotification {
public:
    static constexpr const char NOTIFICATIONS[] = "notifications";
    static constexpr const char GAMI_ID[] = "gamiId";
    static constexpr const char COMPONENT[] = "component";
    static constexpr const char NOTIFICATION[] = "notification";
    static constexpr const char TYPE[] = "type";
    static constexpr const char PARENT[] = "parent";
};

}
}
}
