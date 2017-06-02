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
 * @file module_hardware_status.hpp
 * @brief Module hardware status implementation
 * */

#pragma once

#include "agent-framework/status/module_hardware_status.hpp"
#include "agent-framework/status/module_status.hpp"

/*! Agent namespace */
namespace agent {
/*! Compute namespace */
namespace compute {
/*! Status namespace */
namespace status {

/*!
 * @brief ModuleHardwareStatus class.
 * */
class GpioStatus final {
    uint32_t m_slot = 0;
public:
    /*!
     * @brief Class default constructor.
     * */
    GpioStatus(uint32_t slot) : m_slot(slot) {
    }

    /*!
     * @brief Method reads Hardware Status of Module.
     * @return Module Status
     * */
    agent_framework::status::ModuleStatus::Status read_from_file();

    /*!
     * @brief Class destructor.
     * */
    ~GpioStatus() { }

};

}
}
}

