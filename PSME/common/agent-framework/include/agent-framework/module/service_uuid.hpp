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
 * @file service_uuid.hpp
 *
 * @brief Declares ServiceUuid class handling unique service identifier.
 * */

#pragma once



#include "agent-framework/generic/singleton.hpp"

#include <string>

/*! agent_framework namespace */
namespace agent_framework {
/*! generic namespace */
namespace module {

/*!
 * @brief Class handling unique and persistent service identifiers
 *
 * Unique service identifier is stored in a file under a path specified in
 * a configuration file. If the file containing the identifier is missing,
 * it is created on the first time the identifier is requested. If for some
 * reason the file cannot be created, a temporary identifier will be used until
 * service restart, when the described procedure will be repeated.
 */
class ServiceUuid : public agent_framework::generic::Singleton<ServiceUuid> {
public:
    /*!
     * @brief Destructor.
     */
    virtual ~ServiceUuid();


    /*!
     * @brief Gets service unique id.
     *
     * @return Service unique id.
     */
    const std::string& get_service_uuid() const {
        return m_service_uuid;
    }


    /*!
     * @brief Sets service unique id.
     *
     * @param[in] service_uuid Service unique id.
     */
    void set_service_uuid(const std::string& service_uuid) {
        m_service_uuid = service_uuid;
    }


protected:
    /*!
     * @brief Constructor.
     *
     * Protected constructor is required by the design pattern to make
     * ServiceUuid class a true singleton
     */
    ServiceUuid();


    /*! Base template must be declared a friend of ServiceUuid */
    friend agent_framework::generic::Singleton<ServiceUuid>;

private:
    std::string m_service_uuid{};
};

}
}

