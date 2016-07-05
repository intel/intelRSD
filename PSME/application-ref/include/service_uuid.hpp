/*!
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
 *
 * @file service_uuid.hpp
 *
 * @brief Declares ServiceUuid class handling unique service identifier.
 * */

#ifndef PSME_APP_SERVICE_UUID_HPP
#define PSME_APP_SERVICE_UUID_HPP

#include "agent-framework/generic/singleton.hpp"
#include <string>

/*! Psme namespace */
namespace psme {
/*! Application namespace */
namespace app {

/*!
 * @brief Class handling unique service identifier persistent
 * across service restarts.
 *
 * Unique service identifier is stored in file specified in configuration.
 * If the file with unique service id is not present it will be generated.
 * If it can not be read or created a temporary generated identifier will be
 * used until next application restart.
 */
class ServiceUuid : public agent_framework::generic::Singleton<ServiceUuid> {
public:
    /*!
     * @brief Constructor.
     */
    ServiceUuid();

    /*!
     * @brief Destructor.
     */
    virtual ~ServiceUuid();

    /*!
     * @brief Gets service unique id.
     * @return Service unique id.
     */
    const std::string& get_service_uuid();

private:
    std::string m_service_uuid{};
};

}
}

#endif /* PSME_APP_SERVICE_UUID_HPP */
