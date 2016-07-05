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
 * @file module_hardware_status.hpp
 * @brief Module hardware status implementation
 * */

#ifndef AGENT_STUBS_NETWORK_STATUS_MODULE_HARDWARE_STATUS_HPP
#define AGENT_STUBS_NETWORK_STATUS_MODULE_HARDWARE_STATUS_HPP

#include "agent-framework/status/module_hardware_status.hpp"

/* Forward declaration */
namespace json { class Value; }

/*! Agent namespace */
namespace agent {
/*! Network namespace */
namespace network {
/*! Status namespace */
namespace status {

/*!
 * @brief ModuleHardwareStatus class.
 * */
class ModuleHardwareStatus : public agent_framework::status::ModuleHardwareStatus {
public:
    /*!
     * @brief Class desctructor.
     * */
    ~ModuleHardwareStatus();

    /*!
     * @brief Method reads Hardware Status of Module.
     * */
    Status read_status();
};

}
}
}
#endif /* AGENT_STUBS_NETWORK_STATUS_MODULE_HARDWARE_STATUS_HPP */

