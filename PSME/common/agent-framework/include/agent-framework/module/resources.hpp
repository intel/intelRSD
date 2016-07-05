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
 * @file resources.hpp
 * @brief Resources interface
 * */
#ifndef AGENT_FRAMEWORK_MODULE_RESOURCES_HPP
#define AGENT_FRAMEWORK_MODULE_RESOURCES_HPP

#include "agent-framework/module/status.hpp"

namespace agent_framework {
namespace generic {

/*! Resources */
class Resources {
public:
    Resources();
    virtual ~Resources();

    /*! Enable copy */
    Resources(const Resources&) = default;
    Resources& operator=(const Resources&) = default;

    /*!
     * @brief Get uuid
     * @return Component uuid
     */
    const std::string& get_uuid() const {
        return m_uuid;
    }

    /*!
     * Gets status
     * @return Status reference
     * */
    const Status& get_status() const {
        return m_status;
    }

    /*!
     * @brief Set status
     * @param[in] status Resources's status
     * */
    void set_status(const Status& status){
        m_status = status;
    }

private:
    std::string m_uuid{};
    Status m_status{};
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_RESOURCES_HPP */
