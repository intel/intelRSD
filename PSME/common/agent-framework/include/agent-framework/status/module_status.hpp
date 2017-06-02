/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#pragma once
// Standard library.
#include <string>

/*! Psme namespace */
namespace agent_framework {
/*! Generic namespace */
namespace status {

/*!
 * @brief ModuleStatus interafce.
 * */
class ModuleStatus {
public:
    /*!
     * @enum Status
     * @brief Module`s status.
     *
     * @var Status ModuleStatus::UNKNOWN
     * Agent is can not check status of device.
     *
     * @var Status ModuleStatus::PRESENT
     * Device is present.
     *
     * @var Status ModuleStatus::NOT_PRESENT
     * Device is not present.
     * */
    enum class Status {
        UNKNOWN,
        PRESENT,
        NOT_PRESENT,
        DETERMINING
    };

public:
    /*!
     * @brief Default class constructor.
     * */
    ModuleStatus() {}

    /*!
     * @brief Class destructor.
     * */
    virtual ~ModuleStatus();

    /*!
     * @brief Getter method for ModuleStatus.
     * @return Status enum.
     * */
    virtual Status read_status() = 0;

protected:
    /*!
     * @brief Value of a Status.
     * */
    Status m_status{Status::PRESENT};
};

}
}
