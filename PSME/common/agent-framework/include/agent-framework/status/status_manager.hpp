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
 * @file module_status.hpp
 * @brief Abstract ModuleStatus
 * */

#ifndef AGENT_FRAMEWORK_STATUS_STATUS_MANAGER_HPP
#define AGENT_FRAMEWORK_STATUS_STATUS_MANAGER_HPP

#include "agent-framework/status/module_status.hpp"

/*! Psme namespace */
namespace agent_framework {
/*! Generic namespace */
namespace status {

/*!
 * @brief StatusManager interafce.
 * */
class StatusManager {
public:

    /*! @brief Default constructor */
    StatusManager() = default;

    /*!
     * @brief Copy constructor.
     * @param orig reference to object which will be copied.
     */
    StatusManager(const StatusManager& orig) = default;

    /*!
     * @brief Gets hardware status of Module
     * */
    virtual ModuleStatus::Status get_hw_status() = 0;

    /*!
     * @brief Gets software status of Module
     * */
    virtual ModuleStatus::Status get_sw_status() = 0;

    virtual ~StatusManager();
};

}
}

#endif /* AGENT_FRAMEWORK_STATUS_STATUS_MANAGER_HPP */
