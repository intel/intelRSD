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
 * @brief Module hardware status interface
 * */

#pragma once
#include "agent-framework/status/module_status.hpp"

/*! Psme namespace */
namespace agent_framework {
/*! Generic namespace */
namespace status {

/*!
 * @brief ModuleHardwareStatus class.
 * */
class ModuleHardwareStatus : public ModuleStatus {
public:
    /*!
     * @brief Class desctructor.
     * */
    virtual ~ModuleHardwareStatus();

    /*!
     * @brief Virtual interafce for reading current ModuleStatus.
     * @return Current ModuleStatus.
     * */
     virtual Status read_status() override;
};

}
}

