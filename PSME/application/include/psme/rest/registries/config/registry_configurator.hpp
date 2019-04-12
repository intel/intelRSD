/*!
 * @brief RegistryConfigurator class definition
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
 * @file registry_configurator.hpp
 * */

#pragma once



#include "agent-framework/generic/singleton.hpp"

#include <mutex>
#include <string>



namespace psme {
namespace rest {
namespace registries {

/*!
 * Class for configuring message registry module of the redfish server.
 * */
class RegistryConfigurator : public agent_framework::generic::Singleton<RegistryConfigurator> {
public:

    /*!
     * Destructor.
     * */
    ~RegistryConfigurator();


    /*!
     * Initialize all message registry files and message registries based on a provided configuration.
     *
     * @param[in] configuration Configuration string.
     * */
    void load(const std::string& configuration);


private:
    std::mutex m_mutex{};
};

}
}
}
