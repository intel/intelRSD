/*!
 * @brief MessageRegistryManager class definition
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file message_registry_manager.hpp
 * */

#pragma once



#include "agent-framework/generic/singleton.hpp"



namespace psme {
namespace rest {
namespace registries {

/*!
 * Class responsible for managing all message registries within the redfish server.
 * */
class MessageRegistryManager : public agent_framework::generic::Singleton<MessageRegistryManager> {
// @TODO: implement it when message registries are supported in redfish.
public:
    ~MessageRegistryManager();


private:
};

}
}
}
